#include "nickel/graphics/gltf.hpp"

#include "nickel/common/memory/refcountable.hpp"
#include "nickel/graphics/gltf_draw.hpp"
#include "nickel/graphics/internal/context_impl.hpp"
#include "nickel/graphics/internal/material3d_impl.hpp"
#include "nickel/graphics/internal/texture_impl.hpp"
#include "nickel/graphics/lowlevel/adapter.hpp"
#include "nickel/graphics/material.hpp"
#include "nickel/graphics/mesh.hpp"
#include "nickel/graphics/texture_manager.hpp"
#include "nickel/nickel.hpp"

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
    bufView.offset = dst.size();
    bufView.size = size;
    bufView.count = accessor.count;

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

class GLTFModel::Impl : public RefCountable {
public:
    static GLTFModel Null;

    Buffer pbr_parameter_buffer;
    std::vector<BufferView> buffer_views;
    std::vector<Buffer> dataBuffers;
    std::vector<Texture> textures;
    std::vector<Sampler> samplers;
    std::vector<Scene> scenes;
    std::vector<Material3D> materials;

    operator bool() const;

    BlockMemoryAllocator<Material3DImpl> m_mtl_allocator;

private:
    bool valid_ = false;
};

GLTFModel::GLTFModel(Impl* impl) : m_impl{impl} {}

GLTFModel::GLTFModel(const GLTFModel& o) noexcept : m_impl{o.m_impl} {
    if (m_impl) {
        m_impl->IncRefcount();
    }
}

GLTFModel::GLTFModel(GLTFModel&& o) noexcept : m_impl{o.m_impl} {
    o.m_impl = nullptr;
}

GLTFModel& GLTFModel::operator=(const GLTFModel& o) noexcept {
    if (&o != this) {
        m_impl = o.m_impl;
        m_impl->IncRefcount();
    }
    return *this;
}

GLTFModel& GLTFModel::operator=(GLTFModel&& o) noexcept {
    if (&o != this) {
        m_impl = o.m_impl;
        o.m_impl = nullptr;
    }
    return *this;
}

GLTFModel::~GLTFModel() {
    if (m_impl) {
        m_impl->DecRefcount();
    }
}

GLTFModel::operator bool() const {
    return m_impl;
}

struct GLTFLoader {
    GLTFModel Load(const Path& filename, const Adapter& adapter,
                   BlockMemoryAllocator<GLTFModel::Impl>& allocator) {
        return loadGLTF(filename, adapter, allocator,
                        nickel::Context::GetInst().GetTextureManager());
    }

private:
    tinygltf::Model m_gltf_model;

    GLTFModel loadGLTF(const Path& filename, const Adapter& adapter,
                       BlockMemoryAllocator<GLTFModel::Impl>& allocator,
                       TextureManager& texture_mgr) {
        Device device = adapter.GetDevice();
        tinygltf::TinyGLTF loader;
        std::string err, warn;
        if (!loader.LoadASCIIFromFile(&m_gltf_model, &err, &warn,
                                      filename.ToString())) {
            LOGE("load model from ", filename, " failed:\n\terr:", err,
                 "\n\twarn:", warn);
            return {};
        }

        Path root_dir = filename.ParentPath();

        GLTFModel::Impl* model = allocator.Allocate();
        model->textures.reserve(m_gltf_model.images.size());
        model->materials.reserve(m_gltf_model.materials.size());

        // load images
        for (int i = 0; i < m_gltf_model.images.size(); i++) {
            auto& image = m_gltf_model.images[i];
            model->textures.push_back(
                texture_mgr.Load(root_dir / Path{ParseURI2Path(image.uri)},
                                 Format::R8G8B8A8_UNORM));
        }

        // load gltfs
        for (auto& sampler : m_gltf_model.samplers) {
            model->samplers.emplace_back(createSampler(device, sampler));
        }

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
            pbrParam.baseColor.r = colorFactor[0];
            pbrParam.baseColor.g = colorFactor[1];
            pbrParam.baseColor.b = colorFactor[2];
            pbrParam.baseColor.a = colorFactor[3];

            pbrParam.metalness = mtl.pbrMetallicRoughness.metallicFactor;
            pbrParam.roughness = mtl.pbrMetallicRoughness.roughnessFactor;

            memcpy(pbrParams.data() + pbr_parameter_offset, &pbrParam,
                   sizeof(PBRParameters));

            if (auto idx = mtl.pbrMetallicRoughness.baseColorTexture.index;
                idx != -1) {
                mtl3d->basicTexture = parseTextureInfo(idx, *model);
            }

            if (auto idx =
                    mtl.pbrMetallicRoughness.metallicRoughnessTexture.index;
                idx != -1) {
                mtl3d->metalicRoughnessTexture = parseTextureInfo(idx, *model);
            }

            if (auto idx = mtl.normalTexture.index; idx != -1) {
                mtl3d->normalTexture = parseTextureInfo(idx, *model);
            }

            if (auto idx = mtl.occlusionTexture.index; idx != -1) {
                mtl3d->occlusionTexture = parseTextureInfo(idx, *model);
            }

            model->materials.push_back(Material3D{mtl3d});
        }

        model->pbr_parameter_buffer =
            copyBuffer2GPU(device, pbrParams, BufferUsage::Uniform);

        model->buffer_views.resize(model->buffer_views.size());

        for (auto& material : model->materials) {
            material.GetImpl()->bindGroup = createBindGroup(
                model->pbr_parameter_buffer, *material.GetImpl(),
                nickel::Context::GetInst()
                    .GetGraphicsContext()
                    .GetImpl()
                    ->GetGLTFRenderPass()
                    .GetBindGroupLayout());
        }

        std::vector<Scene> scenes;
        for (auto& gltf_scene : m_gltf_model.scenes) {
            for (auto& n : gltf_scene.nodes) {
                Scene scene;
                preorderNodes(device, m_gltf_model.nodes[n], *model, nullptr,
                              scene);
                scenes.emplace_back(std::move(scene));
            }
        }

        return {model};
    }

    Material3DImpl::TextureInfo parseTextureInfo(int idx,
                                                 GLTFModel::Impl& model) {
        Material3DImpl::TextureInfo texture_info;
        auto& info = m_gltf_model.textures[idx];
        if (info.source != -1) {
            texture_info.image = model.textures[info.source].GetImpl().m_view;
            if (info.sampler != -1) {
                texture_info.sampler = model.samplers[info.sampler];
            } else {
                // TODO: add default sampler
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
        memcpy(buffer.GetMappedRange(), src.data(), desc.m_size);
        buffer.Unmap();
        return buffer;
    }

    Sampler createSampler(Device device, const tinygltf::Sampler& gltfSampler) {
        Sampler::Descriptor desc;
        desc.minFilter = GLTFFilter2RHI(gltfSampler.minFilter);
        desc.magFilter = GLTFFilter2RHI(gltfSampler.magFilter);
        desc.addressModeU = GLTFWrapper2RHI(gltfSampler.wrapS);
        desc.addressModeV = GLTFWrapper2RHI(gltfSampler.wrapT);
        return device.CreateSampler(desc);
    }

    BindGroup createBindGroup(Buffer pbrParamsBuffer,
                              const Material3DImpl& material,
                              BindGroupLayout layout) {
        BindGroup::Descriptor desc;

        // pbr parameters uniform buffer
        {
            BindGroup::Entry entry;
            entry.shader_stage = ShaderStage::Fragment;
            entry.arraySize = 1;
            BindGroup::BufferBinding binding;
            binding.buffer = pbrParamsBuffer;
            binding.type = BindGroup::BufferBinding::Type::DynamicUniform;
            entry.binding.entry = binding;

            desc.entries[1] = entry;
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
        entry.arraySize = 1;
        entry.shader_stage = ShaderStage::Fragment;
        BindGroup::ImageBinding binding;
        binding.type = BindGroup::ImageBinding::Type::Image;
        binding.view = view;
        entry.binding.entry = binding;
        desc.entries[slot] = entry;
    }

    void pushSamplerBindingPoint(BindGroup::Descriptor& desc,
                                 const Sampler& sampler, uint32_t slot) {
        BindGroup::Entry entry;
        entry.arraySize = 1;
        entry.shader_stage = ShaderStage::Fragment;
        BindGroup::SamplerBinding binding;
        binding.sampler = sampler;
        entry.binding.entry = binding;
        desc.entries[slot] = entry;
    }

    void preorderNodes(Device device, const tinygltf::Node& node,
                       GLTFModel::Impl& model, GPUMesh* parent, Scene& scene) {
        std::unique_ptr<GPUMesh> newNode = std::make_unique<GPUMesh>();
        newNode->localModelMat = calcNodeTransform(node);

        if (node.mesh != -1) {
            std::vector<unsigned char> data_buffer;
            auto& gltfMesh = m_gltf_model.meshes[node.mesh];
            for (uint32_t i = 0; i < gltfMesh.primitives.size(); i++) {
                auto prim = recordPrimInfo(model.buffer_views, data_buffer,
                                           model.dataBuffers.size(),
                                           gltfMesh.primitives[i]);
                newNode->primitives.emplace_back(prim);
            }
            model.dataBuffers.emplace_back(copyBuffer2GPU(
                device, data_buffer,
                Flags{BufferUsage::Index} | BufferUsage::Vertex));
        }

        GPUMesh* final_parent_mesh{};
        if (parent) {
            final_parent_mesh =
                parent->children.emplace_back(std::move(newNode)).get();
        } else {
            final_parent_mesh =
                scene.nodes.emplace_back(std::move(newNode)).get();
        }

        for (auto child : node.children) {
            preorderNodes(device, m_gltf_model.nodes[child], model,
                          final_parent_mesh, scene);
        }
    }

    Primitive recordPrimInfo(std::vector<BufferView>& buffer_views,
                             std::vector<unsigned char>& data_buffer,
                             uint32_t buffer_idx,
                             const tinygltf::Primitive& prim) {
        Primitive primitive;

        auto record_buffer_view =
            [](const tinygltf::Model& model, const tinygltf::Accessor& accessor,
               std::vector<unsigned char>& buffer, uint32_t buffer_idx,
               std::vector<BufferView>& buffer_views) {
                if (buffer_views[accessor.bufferView]) {
                    return accessor.bufferView;
                }

                BufferView& view = buffer_views[accessor.bufferView];
                view = CopyBufferFromGLTF<float>(buffer, TINYGLTF_TYPE_VEC3,
                                                 accessor, model);
                view.buffer = buffer_idx;
                return accessor.bufferView;
            };

        auto& attrs = prim.attributes;
        if (auto it = attrs.find("POSITION"); it != attrs.end()) {
            auto& accessor = m_gltf_model.accessors[it->second];
            primitive.posBufView = record_buffer_view(
                m_gltf_model, accessor, data_buffer, buffer_idx, buffer_views);
        }

        if (prim.indices != -1) {
            auto& accessor = m_gltf_model.accessors[prim.indices];
            primitive.indicesBufView = record_buffer_view(
                m_gltf_model, accessor, data_buffer, buffer_idx, buffer_views);
        }

        if (auto it = attrs.find("TEXCOORD_0"); it != attrs.end()) {
            auto& accessor = m_gltf_model.accessors[it->second];
            primitive.uvBufView = record_buffer_view(
                m_gltf_model, accessor, data_buffer, buffer_idx, buffer_views);
        } else {
            // some trivial data
            primitive.uvBufView = primitive.posBufView;
        }

        const BufferView& position_buffer_view =
            buffer_views[primitive.posBufView.value()];

        if (auto it = attrs.find("NORMAL"); it != attrs.end()) {
            auto& accessor = m_gltf_model.accessors[it->second];

            primitive.normBufView = record_buffer_view(
                m_gltf_model, accessor, data_buffer, buffer_idx, buffer_views);
        } else {
            size_t old_size = data_buffer.size();
            uint32_t pos_count = position_buffer_view.count;
            size_t size = pos_count * sizeof(Vec3);
            data_buffer.resize(data_buffer.size() + size, 0);
            Vec3* norm_ptr = (Vec3*)(data_buffer.data() + old_size);

            BufferView view;
            view.count = pos_count;
            view.size = size;
            view.offset = old_size;
            view.buffer = buffer_idx;
            primitive.normBufView = view;

            auto posPtr =
                (const Vec3*)(data_buffer.data() + position_buffer_view.offset);
            if (primitive.indicesBufView) {
                const BufferView& indices_buffer_view =
                    buffer_views[primitive.indicesBufView.value()];
                auto indices_ptr =
                    data_buffer.data() + indices_buffer_view.offset;

                for (int i = 0; i < indices_buffer_view.count / 3; i++) {
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
                for (int i = 0; i < position_buffer_view.count / 3; i++) {
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

            primitive.tanBufView = record_buffer_view(
                m_gltf_model, accessor, data_buffer, buffer_idx, buffer_views);
        } else {
            size_t old_size = data_buffer.size();
            uint32_t pos_count = position_buffer_view.count;
            size_t size = pos_count * sizeof(Vec4);
            data_buffer.resize(data_buffer.size() + size, 0);
            Vec4* tan_ptr = (Vec4*)(data_buffer.data() + old_size);

            BufferView view;
            view.count = pos_count;
            view.size = size;
            view.offset = old_size;
            view.buffer = buffer_idx;
            primitive.tanBufView = view;

            const BufferView& uv_buffer_view =
                buffer_views[primitive.uvBufView.value()];

            auto posPtr =
                (const Vec3*)(data_buffer.data() + position_buffer_view.offset);
            auto uvPtr =
                (const Vec2*)(data_buffer.data() + uv_buffer_view.offset);
            auto tanPtr = (Vec4*)(data_buffer.data() + view.offset);
            if (primitive.indicesBufView) {
                const BufferView& indices_buffer_view =
                    buffer_views[primitive.indicesBufView.value()];
                auto indicesPtr = (const uint32_t*)(data_buffer.data() +
                                                    indices_buffer_view.offset);

                NICKEL_ASSERT(indices_buffer_view.count % 3 == 0,
                              "indices can't be triangle list");
                for (int i = 0; i < indices_buffer_view.count / 3; i++) {
                    uint32_t idx1 = *(indicesPtr + i * 3);
                    uint32_t idx2 = *(indicesPtr + i * 3 + 1);
                    uint32_t idx3 = *(indicesPtr + i * 3 + 2);

                    auto uv1 = *(uvPtr + idx1);
                    auto uv2 = *(uvPtr + idx2);
                    auto uv3 = *(uvPtr + idx3);

                    auto pos1 = *(posPtr + idx1);
                    auto pos2 = *(posPtr + idx2);
                    auto pos3 = *(posPtr + idx3);

                    Vec4 tangent;

                    if (uv1 == Vec2{} && uv2 == Vec2{} && uv3 == Vec2{}) {
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

        primitive.material = prim.material;

        return primitive;
    }

    Mat44 calcNodeTransform(const tinygltf::Node& node) {
        auto cvtMat = [](const std::vector<double>& datas) {
            Mat44 mat;
            for (int i = 0; i < datas.size(); i++) {
                mat.Ptr()[i] = datas[i];
            }
            return mat;
        };

        auto m = Mat44::Identity();
        if (!node.matrix.empty()) {
            m = cvtMat(node.matrix);
        } else if (!node.scale.empty() || !node.translation.empty() ||
                   !node.rotation.empty()) {
            m = Mat44::Identity();
            if (!node.scale.empty()) {
                m = CreateScale(
                        Vec3(node.scale[0], node.scale[1], node.scale[2])) *
                    m;
            }
            if (!node.rotation.empty()) {
                m = Quat(node.rotation[0], node.rotation[1], node.rotation[2],
                         node.rotation[3])
                        .ToMat() *
                    m;
            }
            if (!node.translation.empty()) {
                m = CreateTranslation(Vec3(node.translation[0],
                                           node.translation[1],
                                           node.translation[2])) *
                    m;
            }
        }

        return m;
    }
};

GLTFModel GLTFManager::Load(const Path& filename) {
    GLTFLoader loader;
    GLTFModel model =
        loader.Load(filename, nickel::Context::GetInst().GetGPUAdapter(),
                    m_model_allocator);
    return model;
}

}  // namespace nickel::graphics
