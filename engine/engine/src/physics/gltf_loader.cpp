#include "nickel/graphics/internal/gltf_loader.hpp"

#include "nickel/graphics/internal/context_impl.hpp"
#include "nickel/graphics/internal/gltf_manager_impl.hpp"
#include "nickel/graphics/internal/gltf_model_impl.hpp"
#include "nickel/graphics/internal/material3d_impl.hpp"
#include "nickel/graphics/internal/mesh_impl.hpp"
#include "nickel/graphics/internal/texture_impl.hpp"

namespace nickel::graphics {

template <typename T>
static Buffer copyBuffer2GPU(Device device, std::span<T> src,
                             Flags<BufferUsage> usage) {
    Buffer::Descriptor desc;
    desc.m_usage = usage;
    desc.m_memory_type = MemoryType::GPULocal;
    desc.m_size = src.size();
    auto buffer = device.CreateBuffer(desc);
    buffer.BuffData((void*)src.data(), desc.m_size, 0);
    return buffer;
}


GLTFLoader::GLTFLoader(const tinygltf::Model& model) : m_gltf_model{model} {}

GLTFLoadData GLTFLoader::Load(const Path& filename, const Adapter& adapter,
                              GLTFManagerImpl& gltf_manager) {
    auto graphics_ctx =
        nickel::Context::GetInst().GetGraphicsContext().GetImpl();
    auto& gltf_render_pass = graphics_ctx->GetGLTFRenderPass();
    return loadGLTF(filename, adapter, gltf_manager, gltf_render_pass,
                    nickel::Context::GetInst().GetTextureManager(),
                    nickel::Context::GetInst()
                        .GetGraphicsContext()
                        .GetImpl()
                        ->GetCommonResource());
}

GLTFLoadData GLTFLoader::loadGLTF(const Path& filename, const Adapter& adapter,
                                  GLTFManagerImpl& gltf_manager,
                                  GLTFRenderPass& render_pass,
                                  TextureManager& texture_mgr,
                                  CommonResource& common_res) {
    Device device = adapter.GetDevice();

    Path root_dir = filename.ParentPath();
    GLTFLoadData load_data;
    load_data.m_resource =
        gltf_manager.m_model_resource_allocator.Allocate(&gltf_manager);
    GLTFModelResourceImpl* resource = load_data.m_resource.GetImpl();

    std::vector<Texture> textures;
    textures.reserve(m_gltf_model.images.size());

    std::vector<Material3D> materials;
    materials.reserve(m_gltf_model.materials.size());

    std::vector<Sampler> samplers;
    samplers.reserve(m_gltf_model.samplers.size());

    // load images
    for (int i = 0; i < m_gltf_model.images.size(); i++) {
        auto& image = m_gltf_model.images[i];
        textures.push_back(texture_mgr.Load(
            root_dir / Path{ParseURI2Path(image.uri)}, Format::R8G8B8A8_UNORM));
    }

    // load samplers
    for (auto& sampler : m_gltf_model.samplers) {
        samplers.emplace_back(createSampler(device, sampler));
    }

    std::vector<Material3D::Descriptor> mtl_desces;

    // load material
    for (auto& mtl : m_gltf_model.materials) {
        // Material3D material;
        auto align = adapter.GetLimits().min_uniform_buffer_offset_alignment;
        Material3D::Descriptor desc;

        uint32_t pbr_parameter_offset =
            std::ceil(resource->m_cpu_data.pbr_parameters.size() /
                      (float)align) *
            align;
        uint32_t pbr_parameter_size = sizeof(PBRParameters);
        resource->m_cpu_data.pbr_parameters.resize(pbr_parameter_offset +
                                                   pbr_parameter_size);
        desc.pbrParameters.m_count = 1;
        desc.pbrParameters.m_offset = pbr_parameter_offset;
        desc.pbrParameters.m_size = pbr_parameter_size;

        PBRParameters pbrParam;

        auto& colorFactor = mtl.pbrMetallicRoughness.baseColorFactor;
        pbrParam.m_base_color.r = colorFactor[0];
        pbrParam.m_base_color.g = colorFactor[1];
        pbrParam.m_base_color.b = colorFactor[2];
        pbrParam.m_base_color.a = colorFactor[3];

        pbrParam.m_metallic = mtl.pbrMetallicRoughness.metallicFactor;
        pbrParam.m_roughness = mtl.pbrMetallicRoughness.roughnessFactor;

        memcpy(
            resource->m_cpu_data.pbr_parameters.data() + pbr_parameter_offset,
            &pbrParam, sizeof(PBRParameters));

        desc.basicTexture = parseTextureInfo(
            mtl.pbrMetallicRoughness.baseColorTexture.index, textures,
            common_res.m_default_image, samplers, common_res.m_default_sampler);

        desc.metalicRoughnessTexture = parseTextureInfo(
            mtl.pbrMetallicRoughness.metallicRoughnessTexture.index, textures,
            common_res.m_default_image, samplers, common_res.m_default_sampler);

        desc.normalTexture = parseTextureInfo(
            mtl.normalTexture.index, textures, common_res.m_default_image,
            samplers, common_res.m_default_sampler);

        desc.occlusionTexture = parseTextureInfo(
            mtl.occlusionTexture.index, textures, common_res.m_default_image,
            samplers, common_res.m_default_sampler);

        mtl_desces.push_back(std::move(desc));
    }

    Buffer pbr_parameter_buffer =
        copyBuffer2GPU(device, std::span{resource->m_cpu_data.pbr_parameters},
                       Flags{BufferUsage::Uniform} | BufferUsage::CopyDst);

    for (auto& desc : mtl_desces) {
        desc.pbr_param_buffer = pbr_parameter_buffer;
        materials.push_back(Material3D{gltf_manager.m_mtl_allocator.Allocate(
            &gltf_manager, desc, common_res.m_camera_buffer,
            common_res.m_view_buffer, render_pass.GetBindGroupLayout())});
    }

    load_data.m_meshes.reserve(m_gltf_model.meshes.size());

    std::unordered_map<uint32_t, Buffer> data_buffers;
    std::vector<unsigned char> generated_data_buffer;
    for (auto& m : m_gltf_model.meshes) {
        Mesh mesh =
            createMesh(device, m, &gltf_manager, *resource, data_buffers,
                       generated_data_buffer, materials, *gltf_manager.m_default_material);
        load_data.m_meshes.push_back(mesh);
    }

    return load_data;
}

Material3D::TextureInfo GLTFLoader::parseTextureInfo(
    int idx, std::vector<Texture>& textures, ImageView& default_texture,
    std::vector<Sampler>& samplers, Sampler& default_sampler) {
    Material3D::TextureInfo texture_info;
    const tinygltf::Texture* info = idx == -1 ? nullptr : &m_gltf_model.textures[idx];
    if (info && info->source != -1) {
        texture_info.image = textures[info->source].GetImpl()->m_view;
    } else {
        texture_info.image = default_texture;
    }
    if (info && info->sampler != -1) {
        texture_info.sampler = samplers[info->sampler];
    } else {
        texture_info.sampler = default_sampler;
    }
    return texture_info;
}

Sampler GLTFLoader::createSampler(Device device,
                                  const tinygltf::Sampler& gltfSampler) {
    Sampler::Descriptor desc;
    desc.m_min_filter = GLTFFilter2RHI(gltfSampler.minFilter);
    desc.m_mag_filter = GLTFFilter2RHI(gltfSampler.magFilter);
    desc.m_address_mode_u = GLTFWrapper2RHI(gltfSampler.wrapS);
    desc.m_address_mode_v = GLTFWrapper2RHI(gltfSampler.wrapT);
    return device.CreateSampler(desc);
}

template <typename ComponentType>
int ComponentType2GLTF() {
    if constexpr (std::is_same_v<ComponentType, float>) {
        return TINYGLTF_COMPONENT_TYPE_FLOAT;
    } else if constexpr (std::is_same_v<ComponentType, double>) {
        return TINYGLTF_COMPONENT_TYPE_DOUBLE;
    } else if constexpr (std::is_same_v<ComponentType, int>) {
        return TINYGLTF_COMPONENT_TYPE_INT;
    } else if constexpr (std::is_same_v<ComponentType, unsigned int>) {
        return TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT;
    } else if constexpr (std::is_same_v<ComponentType, char>) {
        return TINYGLTF_COMPONENT_TYPE_BYTE;
    } else if constexpr (std::is_same_v<ComponentType, unsigned char>) {
        return TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE;
    } else if constexpr (std::is_same_v<ComponentType, short>) {
        return TINYGLTF_COMPONENT_TYPE_SHORT;
    } else if constexpr (std::is_same_v<ComponentType, unsigned short>) {
        return TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT;
    } else {
        static_assert(false, "invalid gltf component type");
        return 0;
    }
}

template <typename ComponentType>
BufferView recordPrimComponentInfo(
    const tinygltf::Model& model, Device& device,
    const tinygltf::Accessor& accessor,
    std::unordered_map<uint32_t, Buffer>& data_buffers) {
    NICKEL_ASSERT(accessor.componentType ==
                  ComponentType2GLTF<ComponentType>());
    auto gltf_buffer_idx = model.bufferViews[accessor.bufferView].buffer;
    if (!data_buffers.contains(gltf_buffer_idx)) {
        auto& gltf_buffer = model.buffers[gltf_buffer_idx];
        // TODO: not all buffer need Index usage
        data_buffers[gltf_buffer_idx] =
            copyBuffer2GPU(device, std::span{gltf_buffer.data},
                           Flags{BufferUsage::Index} | BufferUsage::CopyDst |
                               BufferUsage::Vertex);
    }
    Buffer& buffer = data_buffers[gltf_buffer_idx];
    const tinygltf::BufferView& gltf_buffer_view =
        model.bufferViews[accessor.bufferView];
    auto offset = accessor.byteOffset + gltf_buffer_view.byteOffset;
    auto size = accessor.count * sizeof(ComponentType) *
                tinygltf::GetNumComponentsInType(accessor.type);
    BufferView buffer_view;
    buffer_view.m_buffer = buffer;
    buffer_view.m_offset = offset;
    buffer_view.m_size = size;
    buffer_view.m_count = accessor.count;
    buffer_view.m_stride = gltf_buffer_view.byteStride == 0
                               ? sizeof(ComponentType)
                               : gltf_buffer_view.byteStride;
    return buffer_view;
}

Mesh GLTFLoader::createMesh(Device device, const tinygltf::Mesh& gltf_mesh,
                            GLTFManagerImpl* mgr, GLTFModelResourceImpl& model,
                            std::unordered_map<uint32_t, Buffer> data_buffers,
                            std::vector<unsigned char>& generated_data_buffer,
                            std::vector<Material3D>& materials,
                            Material3DImpl& default_material) {
    MeshImpl* newNode = mgr->m_mesh_allocator.Allocate(mgr);
    newNode->m_name = gltf_mesh.name;

    for (auto& primitive : gltf_mesh.primitives) {
        auto prim = recordPrimInfo(device, data_buffers, generated_data_buffer,
                                   primitive, materials, default_material);
        newNode->m_primitives.emplace_back(prim);
    }

    // TODO: load cpu data
    // model.m_cpu_data.data_buffers.push_back(data_buffer);
    Buffer gpu_generated_data_buffer = copyBuffer2GPU(
        device, std::span{generated_data_buffer},
        Flags{BufferUsage::Index} | BufferUsage::CopyDst | BufferUsage::Vertex);

    for (auto& primitive : newNode->m_primitives) {
        if (!primitive.m_norm_buf_view.m_buffer) {
            primitive.m_norm_buf_view.m_buffer = gpu_generated_data_buffer;
        }
        if (!primitive.m_tan_buf_view.m_buffer) {
            primitive.m_tan_buf_view.m_buffer = gpu_generated_data_buffer;
        }
    }

    return newNode;
}

Primitive GLTFLoader::recordPrimInfo(
    Device& device, std::unordered_map<uint32_t, Buffer>& data_buffers,
    std::vector<unsigned char>& generated_data_buffer,
    const tinygltf::Primitive& prim, std::vector<Material3D>& materials,
    Material3DImpl& default_material) const {
    Primitive primitive;

    auto& attrs = prim.attributes;
    if (auto it = attrs.find("POSITION"); it != attrs.end()) {
        primitive.m_pos_buf_view = recordPrimComponentInfo<float>(
            m_gltf_model, device, m_gltf_model.accessors[it->second],
            data_buffers);
    }

    if (prim.indices != -1) {
        auto& accessor = m_gltf_model.accessors[prim.indices];

        if (accessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT) {
            primitive.m_index_type = IndexType::Uint16;

            primitive.m_indices_buf_view = recordPrimComponentInfo<uint16_t>(
                m_gltf_model, device, accessor, data_buffers);
        } else if (accessor.componentType ==
                   TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT) {
            primitive.m_index_type = IndexType::Uint32;

            primitive.m_indices_buf_view = recordPrimComponentInfo<uint32_t>(
                m_gltf_model, device, accessor, data_buffers);
        } else {
            NICKEL_CANT_REACH();
        }
    }

    bool has_uv = true;
    if (auto it = attrs.find("TEXCOORD_0"); it != attrs.end()) {
        auto& accessor = m_gltf_model.accessors[it->second];

        primitive.m_uv_buf_view = recordPrimComponentInfo<float>(
            m_gltf_model, device, accessor, data_buffers);
    } else {
        // some trivial data
        primitive.m_uv_buf_view = primitive.m_pos_buf_view;
        has_uv = false;
    }

    const BufferView& position_buffer_view = primitive.m_pos_buf_view;

    if (auto it = attrs.find("NORMAL"); it != attrs.end()) {
        auto& accessor = m_gltf_model.accessors[it->second];

        primitive.m_norm_buf_view = recordPrimComponentInfo<float>(
            m_gltf_model, device, accessor, data_buffers);
    } else {
        size_t old_size = generated_data_buffer.size();
        uint32_t pos_count = position_buffer_view.m_count;
        size_t size = pos_count * sizeof(Vec3);
        generated_data_buffer.resize(generated_data_buffer.size() + size, 0);
        Vec3* norm_ptr = (Vec3*)(generated_data_buffer.data() + old_size);

        BufferView view;
        view.m_count = pos_count;
        view.m_size = size;
        view.m_offset = old_size;
        primitive.m_norm_buf_view = view;

        auto posPtr = (const Vec3*)(generated_data_buffer.data() +
                                    position_buffer_view.m_offset);
        if (primitive.m_indices_buf_view) {
            const BufferView& indices_buffer_view =
                primitive.m_indices_buf_view;
            auto indices_ptr =
                generated_data_buffer.data() + indices_buffer_view.m_offset;

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

        primitive.m_tan_buf_view = recordPrimComponentInfo<float>(
            m_gltf_model, device, accessor, data_buffers);
    } else {
        size_t old_size = generated_data_buffer.size();
        uint32_t pos_count = position_buffer_view.m_count;
        size_t size = pos_count * sizeof(Vec4);
        generated_data_buffer.resize(generated_data_buffer.size() + size, 0);

        BufferView view;
        view.m_count = pos_count;
        view.m_size = size;
        view.m_offset = old_size;
        primitive.m_tan_buf_view = view;

        const BufferView& uv_buffer_view = primitive.m_uv_buf_view;

        auto posPtr = (const Vec3*)(generated_data_buffer.data() +
                                    position_buffer_view.m_offset);
        auto uvPtr = (const Vec2*)(generated_data_buffer.data() +
                                   uv_buffer_view.m_offset);
        auto tanPtr = (Vec4*)(generated_data_buffer.data() + view.m_offset);
        if (primitive.m_indices_buf_view) {
            const BufferView& indices_buffer_view =
                primitive.m_indices_buf_view;
            auto indicesPtr = (const uint16_t*)(generated_data_buffer.data() +
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

    default_material.IncRefcount();
    Material3D mtl{&default_material};
    primitive.m_material =
        prim.material != -1 ? materials[prim.material] : mtl;

    return primitive;
}

}  // namespace nickel::graphics