#include "nickel/graphics/gltf.hpp"

#include "nickel/common/memory/refcountable.hpp"
#include "nickel/graphics/gltf_draw.hpp"
#include "nickel/graphics/internal/context_impl.hpp"
#include "nickel/graphics/internal/material3d_impl.hpp"
#include "nickel/graphics/lowlevel/adapter.hpp"
#include "nickel/graphics/material.hpp"
#include "nickel/graphics/mesh.hpp"
#include "nickel/graphics/texture_manager.hpp"
#include "nickel/nickel.hpp"

#include "nickel/graphics/internal/gltf_model_impl.hpp"
#include "nickel/graphics/internal/texture_impl.hpp"
#include "nickel/graphics/lowlevel/internal/image_view_impl.hpp"
#include "tiny_gltf.h"

namespace nickel::graphics {

template <typename SrcT, typename DstT>
void ConvertRangeData(const SrcT* src, DstT* dst, size_t blockCount,
                      size_t elemCount, size_t stride) {
    static_assert(std::is_convertible_v<SrcT, DstT>);
    size_t eCount = 0;
    while (blockCount > 0) {
        if (eCount < elemCount) {
            *(dst++) = *(src++);
        } else if (eCount < stride) {
            ++src;
        }
        eCount++;

        if (eCount >= stride) {
            blockCount--;
            eCount = 0;
        }
    }
}

template <typename RequireT>
BufferView CopyBufferFromGLTF(std::vector<unsigned char>& dst, int type,
                              const tinygltf::Accessor& accessor,
                              const tinygltf::Model& model) {
    auto& view = model.bufferViews[accessor.bufferView];
    auto& buffer = model.buffers[view.buffer];
    auto offset = accessor.byteOffset + view.byteOffset;
    auto size = accessor.count * sizeof(RequireT) *
                tinygltf::GetNumComponentsInType(type);
    BufferView bufView;
    bufView.m_offset = dst.size();
    bufView.m_size = size;
    bufView.m_count = accessor.count;

    dst.resize(dst.size() + size);
    auto copySrc = buffer.data.data() + offset;
    auto dstSrc = dst.data() + dst.size() - size;
    auto srcComponentNum = tinygltf::GetNumComponentsInType(accessor.type);
    auto dstComponentNum = tinygltf::GetNumComponentsInType(type);
    switch (accessor.componentType) {
        case TINYGLTF_COMPONENT_TYPE_FLOAT:
            ConvertRangeData((const float*)copySrc, (RequireT*)dstSrc,
                             accessor.count, dstComponentNum, srcComponentNum);
            break;
        case TINYGLTF_COMPONENT_TYPE_DOUBLE:
            ConvertRangeData((const double*)copySrc, (RequireT*)dstSrc,
                             accessor.count, dstComponentNum, srcComponentNum);
            break;
        case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:
            ConvertRangeData((const unsigned int*)copySrc, (RequireT*)dstSrc,
                             accessor.count, dstComponentNum, srcComponentNum);
            break;
        case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
            ConvertRangeData((const unsigned short*)copySrc, (RequireT*)dstSrc,
                             accessor.count, dstComponentNum, srcComponentNum);
            break;
        case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:
            ConvertRangeData((const unsigned char*)copySrc, (RequireT*)dstSrc,
                             accessor.count, dstComponentNum, srcComponentNum);
            break;
        case TINYGLTF_COMPONENT_TYPE_INT:
            ConvertRangeData((const int*)copySrc, (RequireT*)dstSrc,
                             accessor.count, dstComponentNum, srcComponentNum);
            break;
        case TINYGLTF_COMPONENT_TYPE_SHORT:
            ConvertRangeData((const short*)copySrc, (RequireT*)dstSrc,
                             accessor.count, dstComponentNum, srcComponentNum);
            break;
        case TINYGLTF_COMPONENT_TYPE_BYTE:
            ConvertRangeData((const char*)copySrc, (RequireT*)dstSrc,
                             accessor.count, dstComponentNum, srcComponentNum);
            break;
        default:
            NICKEL_CANT_REACH();
    }

    return bufView;
}

inline Filter GLTFFilter2RHI(int type) {
    switch (type) {
        case TINYGLTF_TEXTURE_FILTER_LINEAR:
        // TODO: support mipmap;
        case TINYGLTF_TEXTURE_FILTER_LINEAR_MIPMAP_LINEAR:
        case TINYGLTF_TEXTURE_FILTER_LINEAR_MIPMAP_NEAREST:
            return Filter::Linear;
        case TINYGLTF_TEXTURE_FILTER_NEAREST:
        // TODO: support mipmap;
        case TINYGLTF_TEXTURE_FILTER_NEAREST_MIPMAP_NEAREST:
        case TINYGLTF_TEXTURE_FILTER_NEAREST_MIPMAP_LINEAR:
            return Filter::Nearest;
    }
    return Filter::Linear;
}

inline SamplerAddressMode GLTFWrapper2RHI(int type) {
    switch (type) {
        case TINYGLTF_TEXTURE_WRAP_REPEAT:
            return SamplerAddressMode::Repeat;
        case TINYGLTF_TEXTURE_WRAP_MIRRORED_REPEAT:
            return SamplerAddressMode::MirrorRepeat;
        case TINYGLTF_TEXTURE_WRAP_CLAMP_TO_EDGE:
            return SamplerAddressMode::ClampToEdge;
        default:
            return SamplerAddressMode::ClampToEdge;
    }
}

inline std::string ParseURI2Path(std::string_view str) {
    std::string path;
    path.reserve(str.size());
    int idx = 0;
    while (idx < str.size()) {
        if (str[idx] == '%') {
            if (str[idx + 1] == '2' && str[idx + 2] == '0') {
                path.push_back(' ');
                idx += 3;
            }
            // TODO?: other encode parse
        } else {
            path.push_back(str[idx++]);
        }
    }
    return path;
}

void GLTFModel::Move(const Vec3& offset) {
    m_impl->Move(offset);
}

void GLTFModel::Scale(const Vec3& delta) {
    m_impl->Scale(delta);
}

void GLTFModel::Rotate(const Quat& delta) {
    m_impl->Rotate(delta);
}

void GLTFModel::MoveTo(const Vec3& position) {
    m_impl->MoveTo(position);
}

void GLTFModel::ScaleTo(const Vec3& scale) {
    m_impl->ScaleTo(scale);
}

void GLTFModel::RotateTo(const Quat& q) {
    m_impl->RotateTo(q);
}

void GLTFModel::UpdateTransform() {
    m_impl->UpdateTransform();
}

struct GLTFLoader {
    GLTFModel Load(const Path& filename, const Adapter& adapter,
                   BlockMemoryAllocator<GLTFModelImpl>& allocator) {
        return loadGLTF(filename, adapter, allocator,
                        nickel::Context::GetInst().GetTextureManager());
    }

private:
    tinygltf::Model m_gltf_model;

    GLTFModel loadGLTF(const Path& filename, const Adapter& adapter,
                       BlockMemoryAllocator<GLTFModelImpl>& allocator,
                       TextureManager& texture_mgr) {
        Device device = adapter.GetDevice();
        tinygltf::TinyGLTF loader;
        std::string err, warn;
        if (!loader.LoadASCIIFromFile(&m_gltf_model, &err, &warn,
                                      filename.ToString())) {
            LOGE("load model from {} failed: \n\terr: {}\n\twarn: {}", filename,
                 err, warn);
            return {};
        }

        Path root_dir = filename.ParentPath();

        GLTFModelImpl* model = allocator.Allocate();
        model->textures.reserve(m_gltf_model.images.size());
        model->materials.reserve(m_gltf_model.materials.size());

        // load images
        for (int i = 0; i < m_gltf_model.images.size(); i++) {
            auto& image = m_gltf_model.images[i];
            model->textures.push_back(
                texture_mgr.Load(root_dir / Path{ParseURI2Path(image.uri)},
                                 Format::R8G8B8A8_UNORM));
        }

        // load samplers
        for (auto& sampler : m_gltf_model.samplers) {
            model->samplers.emplace_back(createSampler(device, sampler));
        }

        auto& common_res = nickel::Context::GetInst()
                               .GetGraphicsContext()
                               .GetImpl()
                               ->GetCommonResource();

        // load material
        std::vector<unsigned char> pbrParams;
        for (auto& mtl : m_gltf_model.materials) {
            // Material3D material;
            auto align =
                adapter.GetLimits().min_uniform_buffer_offset_alignment;

            uint32_t pbr_parameter_offset =
                std::ceil(pbrParams.size() / (float)align) * align;
            uint32_t pbr_parameter_size = sizeof(PBRParameters);
            pbrParams.resize(pbr_parameter_offset + pbr_parameter_size);

            Material3DImpl* mtl3d = model->m_mtl_allocator.Allocate();

            PBRParameters pbrParam;

            auto& colorFactor = mtl.pbrMetallicRoughness.baseColorFactor;
            pbrParam.m_base_color.r = colorFactor[0];
            pbrParam.m_base_color.g = colorFactor[1];
            pbrParam.m_base_color.b = colorFactor[2];
            pbrParam.m_base_color.a = colorFactor[3];

            pbrParam.m_metallic = mtl.pbrMetallicRoughness.metallicFactor;
            pbrParam.m_roughness = mtl.pbrMetallicRoughness.roughnessFactor;

            memcpy(pbrParams.data() + pbr_parameter_offset, &pbrParam,
                   sizeof(PBRParameters));

            if (auto idx = mtl.pbrMetallicRoughness.baseColorTexture.index;
                idx != -1) {
                mtl3d->basicTexture = parseTextureInfo(idx, *model);
            } else {
                mtl3d->basicTexture.sampler = common_res.m_default_sampler;
                mtl3d->basicTexture.image = common_res.m_white_image;
            }

            if (auto idx =
                    mtl.pbrMetallicRoughness.metallicRoughnessTexture.index;
                idx != -1) {
                mtl3d->metalicRoughnessTexture = parseTextureInfo(idx, *model);
            } else {
                mtl3d->metalicRoughnessTexture.sampler =
                    common_res.m_default_sampler;
                mtl3d->metalicRoughnessTexture.image = common_res.m_black_image;
            }

            if (auto idx = mtl.normalTexture.index; idx != -1) {
                mtl3d->normalTexture = parseTextureInfo(idx, *model);
            } else {
                mtl3d->normalTexture.sampler = common_res.m_default_sampler;
                mtl3d->normalTexture.image = common_res.m_default_normal_image;
            }

            if (auto idx = mtl.occlusionTexture.index; idx != -1) {
                mtl3d->occlusionTexture = parseTextureInfo(idx, *model);
            } else {
                mtl3d->occlusionTexture.sampler = common_res.m_default_sampler;
                mtl3d->occlusionTexture.image = common_res.m_white_image;
            }

            model->materials.push_back(Material3D{mtl3d});
        }

        model->pbr_parameter_buffer =
            copyBuffer2GPU(device, pbrParams,
                           Flags{BufferUsage::Uniform} | BufferUsage::CopyDst);

        for (auto& material : model->materials) {
            material.GetImpl()->bindGroup = createBindGroup(
                model->pbr_parameter_buffer, *material.GetImpl(),
                nickel::Context::GetInst()
                    .GetGraphicsContext()
                    .GetImpl()
                    ->GetGLTFRenderPass()
                    .GetBindGroupLayout());
        }

        for (auto& gltf_scene : m_gltf_model.scenes) {
            for (auto& n : gltf_scene.nodes) {
                Scene scene;
                preorderNodes(device, m_gltf_model.nodes[n], *model, nullptr,
                              scene);
                model->scenes.emplace_back(std::move(scene));
            }
        }

        return {model};
    }

    Material3DImpl::TextureInfo parseTextureInfo(int idx,
                                                 GLTFModelImpl& model) {
        Material3DImpl::TextureInfo texture_info;
        auto& info = m_gltf_model.textures[idx];
        if (info.source != -1) {
            texture_info.image =
                model.textures[info.source].GetImpl()->m_view;
            if (info.sampler != -1) {
                texture_info.sampler = model.samplers[info.sampler];
            } else {
                texture_info.sampler = nickel::Context::GetInst()
                                           .GetGraphicsContext()
                                           .GetImpl()
                                           ->GetCommonResource()
                                           .m_default_sampler;
            }
        }
        return texture_info;
    }

    Buffer copyBuffer2GPU(Device device, const std::vector<unsigned char>& src,
                          Flags<BufferUsage> usage) {
        Buffer::Descriptor desc;
        desc.m_usage = usage;
        desc.m_memory_type = MemoryType::GPULocal;
        desc.m_size = src.size();
        auto buffer = device.CreateBuffer(desc);
        buffer.BuffData((void*)src.data(), desc.m_size, 0);
        return buffer;
    }

    Sampler createSampler(Device device, const tinygltf::Sampler& gltfSampler) {
        Sampler::Descriptor desc;
        desc.m_min_filter = GLTFFilter2RHI(gltfSampler.minFilter);
        desc.m_mag_filter = GLTFFilter2RHI(gltfSampler.magFilter);
        desc.m_address_mode_u = GLTFWrapper2RHI(gltfSampler.wrapS);
        desc.m_address_mode_v = GLTFWrapper2RHI(gltfSampler.wrapT);
        return device.CreateSampler(desc);
    }

    BindGroup createBindGroup(Buffer pbrParamsBuffer,
                              const Material3DImpl& material,
                              BindGroupLayout layout) {
        BindGroup::Descriptor desc;

        // camera buffer
        {
            BindGroup::Entry entry;
            entry.m_shader_stage = ShaderStage::Vertex;
            entry.m_array_size = 1;
            BindGroup::BufferBinding binding;
            binding.m_buffer = nickel::Context::GetInst()
                                   .GetGraphicsContext()
                                   .GetImpl()
                                   ->GetCommonResource()
                                   .m_camera_buffer;
            binding.m_type = BindGroup::BufferBinding::Type::Uniform;
            entry.m_binding.m_entry = binding;

            desc.m_entries[0] = entry;
        }

        // camera buffer
        {
            BindGroup::Entry entry;
            entry.m_shader_stage = ShaderStage::Fragment;
            entry.m_array_size = 1;
            BindGroup::BufferBinding binding;
            binding.m_buffer = nickel::Context::GetInst()
                                   .GetGraphicsContext()
                                   .GetImpl()
                                   ->GetCommonResource()
                                   .m_view_buffer;
            binding.m_type = BindGroup::BufferBinding::Type::Uniform;
            entry.m_binding.m_entry = binding;

            desc.m_entries[10] = entry;
        }

        // pbr parameters uniform buffer
        {
            BindGroup::Entry entry;
            entry.m_shader_stage = ShaderStage::Fragment;
            entry.m_array_size = 1;
            BindGroup::BufferBinding binding;
            binding.m_buffer = pbrParamsBuffer;
            binding.m_type = BindGroup::BufferBinding::Type::DynamicUniform;
            binding.m_offset = material.pbrParameters.m_offset;
            entry.m_binding.m_entry = binding;

            desc.m_entries[1] = entry;
        }

        pushTextureInfoBinding(desc, material.basicTexture, 2, 6);
        pushTextureInfoBinding(desc, material.normalTexture, 3, 7);
        pushTextureInfoBinding(desc, material.metalicRoughnessTexture, 4, 8);
        pushTextureInfoBinding(desc, material.occlusionTexture, 5, 9);

        return layout.RequireBindGroup(desc);
    }

    void pushTextureInfoBinding(BindGroup::Descriptor& desc,
                                const Material3DImpl::TextureInfo& info,
                                uint32_t image_slot, uint32_t sampler_slot) {
        pushTextureBindingPoint(desc, info.image, image_slot);
        pushSamplerBindingPoint(desc, info.sampler, sampler_slot);
    }

    void pushTextureBindingPoint(BindGroup::Descriptor& desc,
                                 const ImageView& view, uint32_t slot) {
        BindGroup::Entry entry;
        entry.m_array_size = 1;
        entry.m_shader_stage = ShaderStage::Fragment;
        BindGroup::ImageBinding binding;
        binding.m_type = BindGroup::ImageBinding::Type::Image;
        binding.m_view = view;
        entry.m_binding.m_entry = binding;
        desc.m_entries[slot] = entry;
    }

    void pushSamplerBindingPoint(BindGroup::Descriptor& desc,
                                 const Sampler& sampler, uint32_t slot) {
        BindGroup::Entry entry;
        entry.m_array_size = 1;
        entry.m_shader_stage = ShaderStage::Fragment;
        BindGroup::SamplerBinding binding;
        binding.m_sampler = sampler;
        entry.m_binding.m_entry = binding;
        desc.m_entries[slot] = entry;
    }

    void preorderNodes(Device device, const tinygltf::Node& node,
                       GLTFModelImpl& model, GPUMesh* parent, Scene& scene) {
        std::unique_ptr<GPUMesh> newNode = std::make_unique<GPUMesh>();
        newNode->m_name = node.name;
        newNode->m_transform = calcNodeTransform(node);
        if (parent) {
            newNode->m_global_transform = parent->m_global_transform * newNode->m_transform;
        } else {
            newNode->m_global_transform = newNode->m_transform;
        }

        if (node.mesh != -1) {
            std::vector<unsigned char> data_buffer;
            auto& gltfMesh = m_gltf_model.meshes[node.mesh];
            for (uint32_t i = 0; i < gltfMesh.primitives.size(); i++) {
                auto prim =
                    recordPrimInfo(data_buffer, model.dataBuffers.size(),
                                   gltfMesh.primitives[i]);
                newNode->m_primitives.emplace_back(prim);
            }
            model.dataBuffers.emplace_back(
                copyBuffer2GPU(device, data_buffer,
                               Flags{BufferUsage::Index} |
                                   BufferUsage::CopyDst | BufferUsage::Vertex));
        }

        GPUMesh* final_parent_mesh{};
        if (parent) {
            final_parent_mesh =
                parent->m_children.emplace_back(std::move(newNode)).get();
        } else {
            final_parent_mesh =
                scene.m_nodes.emplace_back(std::move(newNode)).get();
        }

        for (auto child : node.children) {
            preorderNodes(device, m_gltf_model.nodes[child], model,
                          final_parent_mesh, scene);
        }
    }

    Primitive recordPrimInfo(std::vector<unsigned char>& data_buffer,
                             uint32_t buffer_idx,
                             const tinygltf::Primitive& prim) {
        Primitive primitive;

        auto& attrs = prim.attributes;
        if (auto it = attrs.find("POSITION"); it != attrs.end()) {
            auto& accessor = m_gltf_model.accessors[it->second];
            primitive.m_pos_buf_view =
                recordBufferView<float>(m_gltf_model, accessor, data_buffer,
                                        buffer_idx, TINYGLTF_TYPE_VEC3);
        }

        if (prim.indices != -1) {
            auto& accessor = m_gltf_model.accessors[prim.indices];
            if (accessor.componentType ==
                TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT) {
                primitive.m_index_type = IndexType::Uint16;
                primitive.m_indices_buf_view = recordBufferView<uint16_t>(
                    m_gltf_model, accessor, data_buffer, buffer_idx,
                    TINYGLTF_TYPE_SCALAR);
            } else if (accessor.componentType ==
                       TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT) {
                primitive.m_index_type = IndexType::Uint32;
                primitive.m_indices_buf_view = recordBufferView<uint32_t>(
                    m_gltf_model, accessor, data_buffer, buffer_idx,
                    TINYGLTF_TYPE_SCALAR);
            } else {
                NICKEL_CANT_REACH();
            }
        }

        bool has_uv = true;
        if (auto it = attrs.find("TEXCOORD_0"); it != attrs.end()) {
            auto& accessor = m_gltf_model.accessors[it->second];
            primitive.m_uv_buf_view =
                recordBufferView<float>(m_gltf_model, accessor, data_buffer,
                                        buffer_idx, TINYGLTF_TYPE_VEC2);
        } else {
            // some trivial data
            primitive.m_uv_buf_view = primitive.m_pos_buf_view;
            has_uv = false;
        }

        const BufferView& position_buffer_view = primitive.m_pos_buf_view;

        if (auto it = attrs.find("NORMAL"); it != attrs.end()) {
            auto& accessor = m_gltf_model.accessors[it->second];

            primitive.m_norm_buf_view =
                recordBufferView<float>(m_gltf_model, accessor, data_buffer,
                                        buffer_idx, TINYGLTF_TYPE_VEC3);
        } else {
            size_t old_size = data_buffer.size();
            uint32_t pos_count = position_buffer_view.m_count;
            size_t size = pos_count * sizeof(Vec3);
            data_buffer.resize(data_buffer.size() + size, 0);
            Vec3* norm_ptr = (Vec3*)(data_buffer.data() + old_size);

            BufferView view;
            view.m_count = pos_count;
            view.m_size = size;
            view.m_offset = old_size;
            view.m_buffer = buffer_idx;
            primitive.m_norm_buf_view = view;

            auto posPtr = (const Vec3*)(data_buffer.data() +
                                        position_buffer_view.m_offset);
            if (primitive.m_indices_buf_view) {
                const BufferView& indices_buffer_view =
                    primitive.m_indices_buf_view;
                auto indices_ptr =
                    data_buffer.data() + indices_buffer_view.m_offset;

                for (int i = 0; i < indices_buffer_view.m_count / 3; i++) {
                    auto idx1 = indices_ptr[i * 3];
                    auto idx2 = indices_ptr[i * 3 + 1];
                    auto idx3 = indices_ptr[i * 3 + 2];

                    auto pos1 = posPtr[idx1];
                    auto pos2 = posPtr[idx2];
                    auto pos3 = posPtr[idx3];
                    auto normal = Cross(Normalize(pos2 - pos1), pos3 - pos1);
                    norm_ptr[idx1] = normal;
                    norm_ptr[idx2] = normal;
                    norm_ptr[idx3] = normal;
                }
            } else {
                for (int i = 0; i < position_buffer_view.m_count / 3; i++) {
                    auto pos1 = posPtr[i * 3];
                    auto pos2 = posPtr[i * 3 + 1];
                    auto pos3 = posPtr[i * 3 + 2];
                    auto normal = Cross(Normalize(pos2 - pos1), pos3 - pos1);
                    norm_ptr[i * 3] = normal;
                    norm_ptr[i * 3 + 1] = normal;
                    norm_ptr[i * 3 + 2] = normal;
                }
            }
        }

        if (auto it = attrs.find("TANGENT"); it != attrs.end()) {
            auto& accessor = m_gltf_model.accessors[it->second];

            primitive.m_tan_buf_view =
                recordBufferView<float>(m_gltf_model, accessor, data_buffer,
                                        buffer_idx, TINYGLTF_TYPE_VEC4);
        } else {
            size_t old_size = data_buffer.size();
            uint32_t pos_count = position_buffer_view.m_count;
            size_t size = pos_count * sizeof(Vec4);
            data_buffer.resize(data_buffer.size() + size, 0);

            BufferView view;
            view.m_count = pos_count;
            view.m_size = size;
            view.m_offset = old_size;
            view.m_buffer = buffer_idx;
            primitive.m_tan_buf_view = view;

            const BufferView& uv_buffer_view = primitive.m_uv_buf_view;

            auto posPtr = (const Vec3*)(data_buffer.data() +
                                        position_buffer_view.m_offset);
            auto uvPtr =
                (const Vec2*)(data_buffer.data() + uv_buffer_view.m_offset);
            auto tanPtr = (Vec4*)(data_buffer.data() + view.m_offset);
            if (primitive.m_indices_buf_view) {
                const BufferView& indices_buffer_view =
                    primitive.m_indices_buf_view;
                auto indicesPtr =
                    (const uint16_t*)(data_buffer.data() +
                                      indices_buffer_view.m_offset);

                NICKEL_ASSERT(indices_buffer_view.m_count % 3 == 0,
                              "indices can't be triangle list");
                for (int i = 0; i < indices_buffer_view.m_count / 3; i++) {
                    uint32_t idx1, idx2, idx3;
                    switch (primitive.m_index_type) {
                        case IndexType::Uint16:
                            idx1 = *(indicesPtr + i * 3);
                            idx2 = *(indicesPtr + i * 3 + 1);
                            idx3 = *(indicesPtr + i * 3 + 2);
                            break;
                        case IndexType::Uint32:
                            idx1 = *(indicesPtr + 2 * (i * 3));
                            idx2 = *(indicesPtr + 2 * (i * 3 + 1));
                            idx3 = *(indicesPtr + 2 * (i * 3 + 2));
                            break;
                    }

                    auto uv1 = *(uvPtr + idx1);
                    auto uv2 = *(uvPtr + idx2);
                    auto uv3 = *(uvPtr + idx3);

                    auto pos1 = *(posPtr + idx1);
                    auto pos2 = *(posPtr + idx2);
                    auto pos3 = *(posPtr + idx3);

                    Vec4 tangent;

                    if (!has_uv ||
                        (uv1 == Vec2{} && uv2 == Vec2{} && uv3 == Vec2{})) {
                        auto t = Normalize(pos2 - pos1);
                        tangent.x = t.x;
                        tangent.y = t.y;
                        tangent.z = t.z;
                        tangent.w = 1;
                    } else {
                        auto [tan, _] =
                            GetNormalMapTB(pos1, pos2, pos3, uv1, uv2, uv3);

                        tangent = Vec4{tan.x, tan.y, tan.z, 1};
                    }
                    *(tanPtr + idx1) = tangent;
                    *(tanPtr + idx2) = tangent;
                    *(tanPtr + idx3) = tangent;
                }
            } else {
                // TODO:
            }
        }

        primitive.m_material = prim.material;

        return primitive;
    }

    Transform calcNodeTransform(const tinygltf::Node& node) {
        auto cvtMat = [](const std::vector<double>& datas) {
            Mat44 mat;
            for (int i = 0; i < datas.size(); i++) {
                mat.Ptr()[i] = datas[i];
            }
            return mat;
        };
        
        if (!node.matrix.empty()) {
            Mat44 mat;
            mat = cvtMat(node.matrix);
            // TODO: calculate SRT from matrix;
            return {};
        }

        Transform transform;

        transform.p = node.translation.empty()
                          ? Vec3{}
                          : Vec3(node.translation[0], node.translation[1],
                                 node.translation[2]);
        transform.q = node.rotation.empty()
                          ? Quat{}
                          : Quat(node.rotation[0], node.rotation[1],
                                 node.rotation[2], node.rotation[3]);
        transform.scale =
            node.scale.empty()
                ? Vec3{1, 1, 1}
                : Vec3(node.scale[0], node.scale[1], node.scale[2]);
        return transform;
    }

    template <typename RequireT>
    BufferView recordBufferView(const tinygltf::Model& model,
                                const tinygltf::Accessor& accessor,
                                std::vector<unsigned char>& buffer,
                                uint32_t buffer_idx, int type) {
        BufferView view;
        view = CopyBufferFromGLTF<RequireT>(buffer, type, accessor, model);
        view.m_buffer = buffer_idx;
        return view;
    }
};

class GLTFManager::Impl {
public:
    GLTFModel Load(const Path&);
    void GC();
    void Update();

    std::unordered_map<Path, GLTFModelImpl*> m_models;
    BlockMemoryAllocator<GLTFModelImpl> m_model_allocator;
};

GLTFModel GLTFManager::Impl::Load(const Path& filename) {
    if (auto it = m_models.find(filename); it != m_models.end()) {
        LOGW("{} model already exists", filename);
        GLTFModelImpl* impl = it->second;
        impl->IncRefcount();
        return GLTFModel{impl};
    }

    GLTFLoader loader;
    GLTFModel model =
        loader.Load(filename, nickel::Context::GetInst().GetGPUAdapter(),
                    m_model_allocator);

    if (model) {
        m_models[filename] = model.GetImpl();
        return model;
    }

    LOGE("{} load failed");
    return {};
}

void GLTFManager::Impl::GC() {
    m_model_allocator.GC();
    // TODO: remove models from m_models;
}

void GLTFManager::Impl::Update() {
    for (auto& [_, model] : m_models) {
        model->UpdateTransform();
    }
}

GLTFModel GLTFManager::Load(const Path& filename) {
    return m_impl->Load(filename);
}

void GLTFManager::GC() {
    m_impl->GC();
}

GLTFManager::GLTFManager() : m_impl{std::make_unique<GLTFManager::Impl>()} {}

GLTFManager::~GLTFManager() {}

void GLTFManager::Update() {
    m_impl->Update();
}

}  // namespace nickel::graphics
