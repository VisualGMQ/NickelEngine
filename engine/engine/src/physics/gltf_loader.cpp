#include "nickel/graphics/internal/gltf_loader.hpp"

#include "nickel/graphics/internal/context_impl.hpp"
#include "nickel/graphics/internal/gltf_manager_impl.hpp"
#include "nickel/graphics/internal/material3d_impl.hpp"
#include "nickel/graphics/internal/mesh_impl.hpp"
#include "nickel/graphics/internal/texture_impl.hpp"

#include <numeric>

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

    size_t totle_buffer_size = std::accumulate(
        m_gltf_model.buffers.begin(), m_gltf_model.buffers.end(), 0,
        [&](size_t acc, const tinygltf::Buffer& buffer) {
            return acc + buffer.data.size();
        });
    std::vector<unsigned char> buffer;
    buffer.reserve(totle_buffer_size);

    std::vector<BufferView> buffer_accessors;
    buffer_accessors.reserve(m_gltf_model.accessors.size());

    std::set<uint32_t> color_textures, metallic_roughness_textures,
        normal_textures, occlusion_textures, emmisive_textures;
    analyzeImageUsage(color_textures, normal_textures, occlusion_textures,
                      metallic_roughness_textures, emmisive_textures);
    auto textures = loadTextures(root_dir, texture_mgr, color_textures);
    auto samplers = loadSamplers(device);

    std::vector<Material3D::Descriptor> mtl_desces;

    std::vector<unsigned char> pbr_parameter_buffer;

    auto materials = loadMaterials(
        adapter, gltf_manager, render_pass, resource->m_cpu_data.pbr_parameters,
        pbr_parameter_buffer, textures, samplers, common_res);

    load_data.m_meshes.reserve(m_gltf_model.meshes.size());

    std::set<uint32_t> vertex_accessors, index_accessors;
    size_t vertex_buffer_size{}, index_buffer_size{};
    analyzeAccessorUsage(vertex_accessors, index_accessors, vertex_buffer_size,
                         index_buffer_size);
    resource->m_cpu_data.vertex_buffer.reserve(vertex_buffer_size);
    resource->m_cpu_data.indices_buffer.reserve(index_buffer_size);
    auto accessors = loadVertexBuffer(resource->m_cpu_data.vertex_buffer,
                                      resource->m_cpu_data.indices_buffer,
                                      vertex_accessors, index_accessors);

    for (auto& m : m_gltf_model.meshes) {
        Mesh mesh =
            createMesh(m, &gltf_manager, resource->m_cpu_data.vertex_buffer,
                       resource->m_cpu_data.indices_buffer, accessors,
                       materials, *gltf_manager.m_default_material);
        load_data.m_meshes.push_back(mesh);
    }

    Buffer gpu_vertex_buffer =
        copyBuffer2GPU(device, std::span{resource->m_cpu_data.vertex_buffer},
                       Flags{BufferUsage::Vertex} | BufferUsage::CopyDst);

    Buffer gpu_index_buffer;
    if (!resource->m_cpu_data.indices_buffer.empty()) {
        gpu_index_buffer = copyBuffer2GPU(
            device, std::span{resource->m_cpu_data.indices_buffer},
            Flags{BufferUsage::Index} | BufferUsage::CopyDst);
    }

    for (auto& m : load_data.m_meshes) {
        for (auto& prim : m.GetImpl()->m_primitives) {
            if (prim.m_indices_buf_view.m_size > 0) {
                prim.m_indices_buf_view.m_buffer = gpu_index_buffer;
            }
            prim.m_norm_buf_view.m_buffer = gpu_vertex_buffer;
            prim.m_pos_buf_view.m_buffer = gpu_vertex_buffer;
            prim.m_tan_buf_view.m_buffer = gpu_vertex_buffer;
            prim.m_uv_buf_view.m_buffer = gpu_vertex_buffer;
        }
    }

    load_data.m_resource = resource;

    return load_data;
}

Material3D::TextureInfo GLTFLoader::parseTextureInfo(
    int idx, std::vector<Texture>& textures, ImageView& default_texture,
    std::vector<Sampler>& samplers, Sampler& default_sampler) {
    Material3D::TextureInfo texture_info;
    const tinygltf::Texture* info =
        idx == -1 ? nullptr : &m_gltf_model.textures[idx];
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

Mesh GLTFLoader::createMesh(const tinygltf::Mesh& gltf_mesh,
                            GLTFManagerImpl* mgr,
                            std::vector<unsigned char>& vertex_buffer,
                            std::vector<unsigned char>& indices_buffer,
                            const std::vector<BufferView>& accessors,
                            std::vector<Material3D>& materials,
                            Material3DImpl& default_material) const {
    MeshImpl* newNode = mgr->m_mesh_allocator.Allocate(mgr);
    newNode->m_name = gltf_mesh.name;

    for (auto& primitive : gltf_mesh.primitives) {
        auto prim = recordPrimInfo(vertex_buffer, indices_buffer, accessors,
                                   primitive, materials, default_material);
        newNode->m_primitives.emplace_back(prim);
    }

    return newNode;
}

Primitive GLTFLoader::recordPrimInfo(
    std::vector<unsigned char>& vertex_buffer,
    std::vector<unsigned char>& indices_buffer,
    const std::vector<BufferView>& buffer_views,
    const tinygltf::Primitive& prim, std::vector<Material3D>& materials,
    Material3DImpl& default_material) const {
    Primitive primitive;

    auto& attrs = prim.attributes;
    if (auto it = attrs.find("POSITION"); it != attrs.end()) {
        primitive.m_pos_buf_view = buffer_views[it->second];
    }

    if (prim.indices != -1) {
        auto& accessor = m_gltf_model.accessors[prim.indices];

        if (accessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT) {
            primitive.m_index_type = IndexType::Uint16;
        } else if (accessor.componentType ==
                   TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT) {
            primitive.m_index_type = IndexType::Uint32;
        } else {
            NICKEL_CANT_REACH();
        }
        primitive.m_indices_buf_view = buffer_views[prim.indices];
    }

    bool has_uv = true;
    if (auto it = attrs.find("TEXCOORD_0"); it != attrs.end()) {
        primitive.m_uv_buf_view = buffer_views[it->second];
    } else {
        // some trivial data
        primitive.m_uv_buf_view = primitive.m_pos_buf_view;
        has_uv = false;
    }

    const BufferView& position_buffer_view = primitive.m_pos_buf_view;

    if (auto it = attrs.find("NORMAL"); it != attrs.end()) {
        primitive.m_norm_buf_view = buffer_views[it->second];
    } else {
        size_t old_size = vertex_buffer.size();
        uint32_t pos_count = position_buffer_view.m_count;
        size_t size = pos_count * sizeof(Vec3);
        vertex_buffer.resize(vertex_buffer.size() + size, 0);
        Vec3* norm_ptr = (Vec3*)(vertex_buffer.data() + old_size);

        BufferView view;
        view.m_count = pos_count;
        view.m_size = size;
        view.m_offset = old_size;
        primitive.m_norm_buf_view = view;

        auto posPtr =
            (const Vec3*)(vertex_buffer.data() + position_buffer_view.m_offset);
        if (primitive.m_indices_buf_view) {
            const BufferView& indices_buffer_view =
                primitive.m_indices_buf_view;
            auto indices_ptr =
                indices_buffer.data() + indices_buffer_view.m_offset;

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
        primitive.m_tan_buf_view = buffer_views[it->second];
    } else {
        size_t old_size = vertex_buffer.size();
        uint32_t pos_count = position_buffer_view.m_count;
        size_t size = pos_count * sizeof(Vec4);
        vertex_buffer.resize(vertex_buffer.size() + size, 0);

        BufferView view;
        view.m_count = pos_count;
        view.m_size = size;
        view.m_offset = old_size;
        primitive.m_tan_buf_view = view;

        const BufferView& uv_buffer_view = primitive.m_uv_buf_view;

        auto posPtr =
            (const Vec3*)(vertex_buffer.data() + position_buffer_view.m_offset);
        auto uvPtr =
            (const Vec2*)(vertex_buffer.data() + uv_buffer_view.m_offset);
        auto tanPtr = (Vec4*)(vertex_buffer.data() + view.m_offset);

        if (primitive.m_indices_buf_view.m_size > 0) {
            const BufferView& indices_buffer_view =
                primitive.m_indices_buf_view;
            auto indicesPtr = (const uint16_t*)(indices_buffer.data() +
                                                indices_buffer_view.m_offset);
            NICKEL_ASSERT(indices_buffer_view.m_count % 3 == 0,
                          "indices must be triangle list");
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
            NICKEL_ASSERT(primitive.m_pos_buf_view.m_count % 3 == 0,
                          "vertices must be triangle list");
            for (int i = 0; i < primitive.m_pos_buf_view.m_count / 3; i++) {
                uint32_t idx1 = i * 3, idx2 = i * 3 + 1, idx3 = i * 3 + 2;
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
        }
    }

    default_material.IncRefcount();
    Material3D mtl{&default_material};
    primitive.m_material = prim.material != -1 ? materials[prim.material] : mtl;

    return primitive;
}

void GLTFLoader::analyzeAccessorUsage(std::set<uint32_t>& out_vertex_accessors,
                                      std::set<uint32_t>& out_index_accessors,
                                      size_t& out_vertex_buffer_size,
                                      size_t& out_index_buffer_size) {
    for (auto& mesh : m_gltf_model.meshes) {
        uint32_t vertex_count{};
        for (auto& primitive : mesh.primitives) {
            if (auto it = primitive.attributes.find("POSITION");
                it != primitive.attributes.end()) {
                out_vertex_accessors.insert(it->second);

                auto& accessor = m_gltf_model.accessors[it->second];
                vertex_count = accessor.count;
            }
            out_vertex_buffer_size += sizeof(Vec3) * vertex_count;

            if (auto it = primitive.attributes.find("TEXCOORD_0");
                it != primitive.attributes.end()) {
                out_vertex_accessors.insert(it->second);

                auto& accessor = m_gltf_model.accessors[it->second];
                out_vertex_buffer_size +=
                    accessor.count *
                    tinygltf::GetComponentSizeInBytes(accessor.componentType) *
                    tinygltf::GetNumComponentsInType(accessor.type);
            }
            out_vertex_buffer_size += sizeof(Vec2) * vertex_count;

            if (auto it = primitive.attributes.find("NORMAL");
                it != primitive.attributes.end()) {
                out_vertex_accessors.insert(it->second);

                auto& accessor = m_gltf_model.accessors[it->second];
                out_vertex_buffer_size +=
                    accessor.count *
                    tinygltf::GetComponentSizeInBytes(accessor.componentType) *
                    tinygltf::GetNumComponentsInType(accessor.type);
            }
            out_vertex_buffer_size += sizeof(Vec3) * vertex_count;

            if (auto it = primitive.attributes.find("TANGENT");
                it != primitive.attributes.end()) {
                out_vertex_accessors.insert(it->second);
            }
            out_vertex_buffer_size += sizeof(Vec4) * vertex_count;

            if (primitive.indices != -1) {
                out_index_accessors.insert(primitive.indices);

                auto& accessor = m_gltf_model.accessors[primitive.indices];
                out_index_buffer_size +=
                    accessor.count *
                    tinygltf::GetNumComponentsInType(accessor.type) *
                    tinygltf::GetComponentSizeInBytes(accessor.componentType);
            }
        }
    }
}

void GLTFLoader::analyzeImageUsage(
    std::set<uint32_t>& out_color_texture,
    std::set<uint32_t>& out_normal_texture,
    std::set<uint32_t>& out_occlusion_texture,
    std::set<uint32_t>& out_metallic_roughness_texture,
    std::set<uint32_t>& out_emissive_texture) const {
    for (auto& mtl : m_gltf_model.materials) {
        if (mtl.pbrMetallicRoughness.baseColorTexture.index != -1) {
            out_color_texture.insert(
                mtl.pbrMetallicRoughness.baseColorTexture.index);
        }
        if (mtl.normalTexture.index != -1) {
            out_normal_texture.insert(mtl.normalTexture.index);
        }
        if (mtl.occlusionTexture.index != -1) {
            out_occlusion_texture.insert(mtl.occlusionTexture.index);
        }
        if (mtl.pbrMetallicRoughness.metallicRoughnessTexture.index != -1) {
            out_metallic_roughness_texture.insert(
                mtl.pbrMetallicRoughness.metallicRoughnessTexture.index);
        }
        if (mtl.emissiveTexture.index != -1) {
            out_emissive_texture.insert(mtl.emissiveTexture.index);
        }
    }
}

std::vector<Texture> GLTFLoader::loadTextures(
    const Path& root_dir, TextureManager& texture_mgr,
    const std::set<uint32_t>& color_textures) {
    std::vector<Texture> textures;
    textures.reserve(m_gltf_model.images.size());
    for (int i = 0; i < m_gltf_model.images.size(); i++) {
        auto& image = m_gltf_model.images[i];
        Format fmt = Format::R8G8B8A8_UNORM;
        if (color_textures.contains(i)) {
            fmt = Format::R8G8B8A8_SRGB;
        }
        textures.emplace_back(
            texture_mgr.Load(root_dir / Path{ParseURI2Path(image.uri)}, fmt));
    }
    return textures;
}

std::vector<Sampler> GLTFLoader::loadSamplers(Device& device) {
    std::vector<Sampler> samplers;
    for (auto& sampler : m_gltf_model.samplers) {
        Sampler::Descriptor desc;
        desc.m_min_filter = GLTFFilter2RHI(sampler.minFilter);
        desc.m_mag_filter = GLTFFilter2RHI(sampler.magFilter);
        desc.m_address_mode_u = GLTFWrapper2RHI(sampler.wrapS);
        desc.m_address_mode_v = GLTFWrapper2RHI(sampler.wrapT);
        samplers.emplace_back(device.CreateSampler(desc));
    }
    return samplers;
}

std::vector<Material3D> GLTFLoader::loadMaterials(
    const Adapter& adapter, GLTFManagerImpl& gltf_mgr,
    GLTFRenderPass& render_pass, std::vector<PBRParameters>& pbr_parameters,
    std::vector<unsigned char>& data_buffer, std::vector<Texture>& textures,
    std::vector<Sampler>& samplers, CommonResource& common_res) {
    NICKEL_ASSERT(pbr_parameters.empty());

    std::vector<Material3D::Descriptor> mtl_desces;
    std::vector<Material3D> materials;

    // load material
    for (auto& mtl : m_gltf_model.materials) {
        // Material3D material;
        auto align = adapter.GetLimits().min_uniform_buffer_offset_alignment;
        Material3D::Descriptor desc;

        uint32_t pbr_parameter_offset =
            std::ceil(data_buffer.size() / (float)align) * align;
        uint32_t pbr_parameter_size = sizeof(PBRParameters);
        data_buffer.resize(pbr_parameter_offset + pbr_parameter_size);
        desc.pbrParameters.m_count = 1;
        desc.pbrParameters.m_offset = pbr_parameter_offset;
        desc.pbrParameters.m_size = pbr_parameter_size;

        PBRParameters pbr_param;

        auto& colorFactor = mtl.pbrMetallicRoughness.baseColorFactor;
        pbr_param.m_base_color.r = colorFactor[0];
        pbr_param.m_base_color.g = colorFactor[1];
        pbr_param.m_base_color.b = colorFactor[2];
        pbr_param.m_base_color.a = colorFactor[3];

        pbr_param.m_metallic = mtl.pbrMetallicRoughness.metallicFactor;
        pbr_param.m_roughness = mtl.pbrMetallicRoughness.roughnessFactor;

        memcpy(data_buffer.data() + pbr_parameter_offset, &pbr_param,
               sizeof(PBRParameters));

        desc.basicTexture = parseTextureInfo(
            mtl.pbrMetallicRoughness.baseColorTexture.index, textures,
            common_res.m_default_image, samplers, common_res.m_default_sampler);

        desc.metalicRoughnessTexture = parseTextureInfo(
            mtl.pbrMetallicRoughness.metallicRoughnessTexture.index, textures,
            common_res.m_white_image, samplers, common_res.m_default_sampler);

        desc.normalTexture =
            parseTextureInfo(mtl.normalTexture.index, textures,
                             common_res.m_default_normal_image, samplers,
                             common_res.m_default_sampler);

        desc.occlusionTexture = parseTextureInfo(
            mtl.occlusionTexture.index, textures, common_res.m_white_image,
            samplers, common_res.m_default_sampler);

        mtl_desces.push_back(std::move(desc));
        pbr_parameters.push_back(pbr_param);
    }

    Buffer pbr_parameter_buffer;
    if (!data_buffer.empty()) {
        pbr_parameter_buffer =
            copyBuffer2GPU(adapter.GetDevice(), std::span{data_buffer},
                           Flags{BufferUsage::Uniform} | BufferUsage::CopyDst);

        for (auto& desc : mtl_desces) {
            desc.pbr_param_buffer = pbr_parameter_buffer;
            materials.push_back(Material3D{gltf_mgr.m_mtl_allocator.Allocate(
                &gltf_mgr, desc, common_res.m_camera_buffer,
                common_res.m_view_buffer, render_pass.GetBindGroupLayout())});
        }
    }

    return materials;
}

std::vector<BufferView> GLTFLoader::loadVertexBuffer(
    std::vector<unsigned char>& out_vertex_buffer,
    std::vector<unsigned char>& out_index_buffer,
    const std::set<uint32_t>& vertex_accessor,
    const std::set<uint32_t>& index_accessor) const {
    NICKEL_ASSERT(out_vertex_buffer.empty());

    std::vector<BufferView> views;
    views.reserve(m_gltf_model.accessors.size());

    for (int i = 0; i < m_gltf_model.accessors.size(); i++) {
        std::vector<unsigned char>* buffer =
            index_accessor.contains(i) ? &out_index_buffer : &out_vertex_buffer;

        auto& accessor = m_gltf_model.accessors[i];
        BufferView buffer_view;
        switch (accessor.componentType) {
            case TINYGLTF_COMPONENT_TYPE_INT:
                buffer_view = CopyBufferFromGLTF<int>(*buffer, accessor.type,
                                                      accessor, m_gltf_model);
                break;
            case TINYGLTF_COMPONENT_TYPE_BYTE:
                buffer_view = CopyBufferFromGLTF<char>(*buffer, accessor.type,
                                                       accessor, m_gltf_model);
                break;
            case TINYGLTF_COMPONENT_TYPE_FLOAT:
                buffer_view = CopyBufferFromGLTF<float>(*buffer, accessor.type,
                                                        accessor, m_gltf_model);
                break;
            case TINYGLTF_COMPONENT_TYPE_DOUBLE:
                buffer_view = CopyBufferFromGLTF<double>(
                    *buffer, accessor.type, accessor, m_gltf_model);
                break;
            case TINYGLTF_COMPONENT_TYPE_SHORT:
                buffer_view = CopyBufferFromGLTF<short>(*buffer, accessor.type,
                                                        accessor, m_gltf_model);
                break;
            case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:
                buffer_view = CopyBufferFromGLTF<uint32_t>(
                    *buffer, accessor.type, accessor, m_gltf_model);
                break;
            case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
                buffer_view = CopyBufferFromGLTF<uint16_t>(
                    *buffer, accessor.type, accessor, m_gltf_model);
                break;
            case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:
                buffer_view = CopyBufferFromGLTF<uint8_t>(
                    *buffer, accessor.type, accessor, m_gltf_model);
                break;
            default:
                NICKEL_CANT_REACH();
        }
        views.push_back(buffer_view);
    }

    return views;
}

}  // namespace nickel::graphics