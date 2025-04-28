#include "nickel/importer/internal/gltf_importer_impl.hpp"

#include "nickel/common/macro.hpp"
#include "nickel/graphics/internal/context_impl.hpp"
#include "nickel/graphics/internal/gltf_manager_impl.hpp"
#include "nickel/graphics/internal/material3d_impl.hpp"
#include "nickel/graphics/internal/mesh_impl.hpp"
#include "nickel/graphics/internal/texture_impl.hpp"
#include "nickel/graphics/lowlevel/render_pass.hpp"
#include "nickel/importer/gltf_importer.hpp"

#include <numeric>

namespace nickel {

template <typename T>
static graphics::Buffer copyBuffer2GPU(graphics::Device device,
                                       std::span<T> src,
                                       Flags<graphics::BufferUsage> usage) {
    graphics::Buffer::Descriptor desc;
    desc.m_usage = usage;
    desc.m_memory_type = graphics::MemoryType::GPULocal;
    desc.m_size = src.size();
    auto buffer = device.CreateBuffer(desc);
    buffer.BuffData((void*)src.data(), desc.m_size, 0);
    return buffer;
}

GLTFLoader::GLTFLoader(const tinygltf::Model& model) : m_gltf_model{model} {}

GLTFImportData GLTFLoader::Load(const Path& filename,
                              const graphics::Adapter& adapter,
                              graphics::GLTFModelManagerImpl& gltf_manager) {
    auto graphics_ctx = Context::GetInst().GetGraphicsContext().GetImpl();
    auto& gltf_render_pass = graphics_ctx->GetGLTFRenderPass();
    return loadGLTF(
        filename, adapter, gltf_manager, gltf_render_pass,
        Context::GetInst().GetTextureManager(),
        Context::GetInst().GetGraphicsContext().GetImpl()->GetCommonResource());
}

GLTFImportData GLTFLoader::loadGLTF(const Path& filename,
                                  const graphics::Adapter& adapter,
                                  graphics::GLTFModelManagerImpl& gltf_manager,
                                  graphics::GLTFRenderPass& render_pass,
                                  graphics::TextureManager& texture_mgr,
                                  graphics::CommonResource& common_res) {
    graphics::Device device = adapter.GetDevice();

    Path root_dir = filename.ParentPath();
    GLTFImportData load_data;
    load_data.m_filename = filename;

    Path pure_filename = filename.Filename().ReplaceExtension("");
    Path parent_dir = filename.ParentPath();
    std::string final_name = (parent_dir / pure_filename).ToString();
    std::ranges::replace(final_name, '\\', '/');
    load_data.m_asset_name = final_name;

    load_data.m_resource =
        gltf_manager.m_model_resource_allocator.Allocate(&gltf_manager);
    graphics::GLTFModelResourceImpl* resource = load_data.m_resource.GetImpl();

    size_t totle_buffer_size = std::accumulate(
        m_gltf_model.buffers.begin(), m_gltf_model.buffers.end(), 0,
        [&](size_t acc, const tinygltf::Buffer& buffer) {
            return acc + buffer.data.size();
        });
    std::vector<unsigned char> buffer;
    buffer.reserve(totle_buffer_size);

    std::vector<graphics::BufferView> buffer_accessors;
    buffer_accessors.reserve(m_gltf_model.accessors.size());

    std::set<uint32_t> color_textures, metallic_roughness_textures,
        normal_textures, occlusion_textures, emmisive_textures;
    analyzeImageUsage(color_textures, normal_textures, occlusion_textures,
                      metallic_roughness_textures, emmisive_textures);
    auto textures = loadTextures(root_dir, texture_mgr, color_textures);
    auto samplers = loadSamplers(device);

    std::vector<graphics::Material3D::Descriptor> mtl_desces;

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
        graphics::Mesh mesh =
            createMesh(m, &gltf_manager, resource->m_cpu_data.vertex_buffer,
                       resource->m_cpu_data.indices_buffer, accessors,
                       materials, *gltf_manager.m_default_material);
        load_data.m_meshes.push_back(mesh);
    }

    graphics::Buffer gpu_vertex_buffer = copyBuffer2GPU(
        device, std::span{resource->m_cpu_data.vertex_buffer},
        Flags{graphics::BufferUsage::Vertex} | graphics::BufferUsage::CopyDst);

    graphics::Buffer gpu_index_buffer;
    if (!resource->m_cpu_data.indices_buffer.empty()) {
        gpu_index_buffer = copyBuffer2GPU(
            device, std::span{resource->m_cpu_data.indices_buffer},
            Flags{graphics::BufferUsage::Index} |
                graphics::BufferUsage::CopyDst);
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

    for (auto& gltf_node : m_gltf_model.nodes) {
        Node node;
        node.m_name = gltf_node.name;
        node.m_local_transform = CalcNodeTransform(gltf_node);
        for (int child : gltf_node.children) {
            node.m_children.emplace_back(child);
        }
        bool is_mesh_node = gltf_node.mesh != -1;
        if (is_mesh_node) {
            node.m_mesh = gltf_node.mesh;
        }
        node.m_flags |= Node::Flag::InMeshTree;
        load_data.m_nodes.push_back(std::move(node));
    }

    for (int i = 0; i < m_gltf_model.skins.size(); i++) {
        auto& gltf_skin = m_gltf_model.skins[i];
        Skin skin;
        skin.m_name = gltf_skin.name;
        if (gltf_skin.skeleton == -1) {
            skin.m_root = gltf_skin.skeleton;
        } else {
            for (auto& node : m_gltf_model.nodes) {
                NICKEL_CONTINUE_IF_FALSE(node.skin != -1 && node.skin == i);
                skin.m_root = node.skin;
            }
        }

        skin.m_bone_indices.resize(gltf_skin.joints.size());
        for (int j : gltf_skin.joints) {
            load_data.m_nodes[j].m_flags |= Node::Flag::HasBone;
            skin.m_bone_indices.push_back(j);
        }
        if (skin) {
            load_data.m_skins.emplace_back(std::move(skin));
        }
    }

    // currently we only load one scene
    const tinygltf::Scene& scene = m_gltf_model.scenes[0];
    for (int root_node : scene.nodes) {
        load_data.m_root_nodes.push_back(root_node);
    }

    for (auto root_node_idx : load_data.m_root_nodes) {
        recordNodeInfoRecursive(load_data.m_nodes,
                                load_data.m_nodes[root_node_idx], nullptr);
    }

    return load_data;
}

graphics::Material3D::TextureInfo GLTFLoader::parseTextureInfo(
    int idx, std::vector<graphics::Texture>& textures,
    graphics::ImageView& default_texture,
    std::vector<graphics::Sampler>& samplers,
    graphics::Sampler& default_sampler) {
    graphics::Material3D::TextureInfo texture_info;
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

graphics::Sampler GLTFLoader::createSampler(
    graphics::Device device, const tinygltf::Sampler& gltfSampler) {
    graphics::Sampler::Descriptor desc;
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
        NICKEL_CANT_REACH();
        return 0;
    }
}

graphics::Mesh GLTFLoader::createMesh(
    const tinygltf::Mesh& gltf_mesh, graphics::GLTFModelManagerImpl* mgr,
    std::vector<unsigned char>& vertex_buffer,
    std::vector<unsigned char>& indices_buffer,
    const std::vector<graphics::BufferView>& accessors,
    std::vector<graphics::Material3D>& materials,
    graphics::Material3DImpl& default_material) const {
    graphics::MeshImpl* newNode = mgr->m_mesh_allocator.Allocate(mgr);
    newNode->m_name = gltf_mesh.name;

    for (auto& primitive : gltf_mesh.primitives) {
        auto prim = recordPrimInfo(vertex_buffer, indices_buffer, accessors,
                                   primitive, materials, default_material);
        newNode->m_primitives.emplace_back(prim);
    }

    return newNode;
}

graphics::Primitive GLTFLoader::recordPrimInfo(
    std::vector<unsigned char>& vertex_buffer,
    std::vector<unsigned char>& indices_buffer,
    const std::vector<graphics::BufferView>& buffer_views,
    const tinygltf::Primitive& prim,
    std::vector<graphics::Material3D>& materials,
    graphics::Material3DImpl& default_material) const {
    graphics::Primitive primitive;

    auto& attrs = prim.attributes;
    if (auto it = attrs.find("POSITION"); it != attrs.end()) {
        primitive.m_pos_buf_view = buffer_views[it->second];
    }

    if (prim.indices != -1) {
        auto& accessor = m_gltf_model.accessors[prim.indices];

        if (accessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT) {
            primitive.m_index_type = graphics::IndexType::Uint16;
        } else if (accessor.componentType ==
                   TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT) {
            primitive.m_index_type = graphics::IndexType::Uint32;
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

    const graphics::BufferView& position_buffer_view = primitive.m_pos_buf_view;

    if (auto it = attrs.find("NORMAL"); it != attrs.end()) {
        primitive.m_norm_buf_view = buffer_views[it->second];
    } else {
        size_t old_size = vertex_buffer.size();
        uint32_t pos_count = position_buffer_view.m_count;
        size_t size = pos_count * sizeof(Vec3);
        vertex_buffer.resize(vertex_buffer.size() + size, 0);
        Vec3* norm_ptr = (Vec3*)(vertex_buffer.data() + old_size);

        graphics::BufferView view;
        view.m_count = pos_count;
        view.m_size = size;
        view.m_offset = old_size;
        primitive.m_norm_buf_view = view;

        auto posPtr =
            (const Vec3*)(vertex_buffer.data() + position_buffer_view.m_offset);
        if (primitive.m_indices_buf_view) {
            const graphics::BufferView& indices_buffer_view =
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

        graphics::BufferView view;
        view.m_count = pos_count;
        view.m_size = size;
        view.m_offset = old_size;
        primitive.m_tan_buf_view = view;

        const graphics::BufferView& uv_buffer_view = primitive.m_uv_buf_view;

        auto posPtr =
            (const Vec3*)(vertex_buffer.data() + position_buffer_view.m_offset);
        auto uvPtr =
            (const Vec2*)(vertex_buffer.data() + uv_buffer_view.m_offset);
        auto tanPtr = (Vec4*)(vertex_buffer.data() + view.m_offset);

        if (primitive.m_indices_buf_view.m_size > 0) {
            const graphics::BufferView& indices_buffer_view =
                primitive.m_indices_buf_view;
            auto indicesPtr = (const uint16_t*)(indices_buffer.data() +
                                                indices_buffer_view.m_offset);
            NICKEL_ASSERT(indices_buffer_view.m_count % 3 == 0,
                          "indices must be triangle list");
            for (int i = 0; i < indices_buffer_view.m_count / 3; i++) {
                uint32_t idx1, idx2, idx3;
                switch (primitive.m_index_type) {
                    case graphics::IndexType::Uint16:
                        idx1 = *(indicesPtr + i * 3);
                        idx2 = *(indicesPtr + i * 3 + 1);
                        idx3 = *(indicesPtr + i * 3 + 2);
                        break;
                    case graphics::IndexType::Uint32:
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
    graphics::Material3D mtl{&default_material};
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

std::vector<graphics::Texture> GLTFLoader::loadTextures(
    const Path& root_dir, graphics::TextureManager& texture_mgr,
    const std::set<uint32_t>& color_textures) {
    std::vector<graphics::Texture> textures;
    textures.reserve(m_gltf_model.images.size());
    for (int i = 0; i < m_gltf_model.images.size(); i++) {
        auto& image = m_gltf_model.images[i];
        graphics::Format fmt = graphics::Format::R8G8B8A8_UNORM;
        if (color_textures.contains(i)) {
            fmt = graphics::Format::R8G8B8A8_SRGB;
        }
        textures.emplace_back(
            texture_mgr.Load(root_dir / Path{ParseURI2Path(image.uri)}, fmt));
    }
    return textures;
}

std::vector<graphics::Sampler> GLTFLoader::loadSamplers(
    graphics::Device& device) {
    std::vector<graphics::Sampler> samplers;
    for (auto& sampler : m_gltf_model.samplers) {
        graphics::Sampler::Descriptor desc;
        desc.m_min_filter = GLTFFilter2RHI(sampler.minFilter);
        desc.m_mag_filter = GLTFFilter2RHI(sampler.magFilter);
        desc.m_address_mode_u = GLTFWrapper2RHI(sampler.wrapS);
        desc.m_address_mode_v = GLTFWrapper2RHI(sampler.wrapT);
        samplers.emplace_back(device.CreateSampler(desc));
    }
    return samplers;
}

std::vector<graphics::Material3D> GLTFLoader::loadMaterials(
    const graphics::Adapter& adapter, graphics::GLTFModelManagerImpl& gltf_mgr,
    graphics::GLTFRenderPass& render_pass,
    std::vector<graphics::PBRParameters>& pbr_parameters,
    std::vector<unsigned char>& data_buffer,
    std::vector<graphics::Texture>& textures,
    std::vector<graphics::Sampler>& samplers,
    graphics::CommonResource& common_res) {
    NICKEL_ASSERT(pbr_parameters.empty());

    std::vector<graphics::Material3D::Descriptor> mtl_desces;
    std::vector<graphics::Material3D> materials;

    // load material
    for (auto& mtl : m_gltf_model.materials) {
        // Material3D material;
        auto align = adapter.GetLimits().min_uniform_buffer_offset_alignment;
        graphics::Material3D::Descriptor desc;

        uint32_t pbr_parameter_offset =
            std::ceil(data_buffer.size() / (float)align) * align;
        uint32_t pbr_parameter_size = sizeof(graphics::PBRParameters);
        data_buffer.resize(pbr_parameter_offset + pbr_parameter_size);
        desc.pbrParameters.m_count = 1;
        desc.pbrParameters.m_offset = pbr_parameter_offset;
        desc.pbrParameters.m_size = pbr_parameter_size;

        graphics::PBRParameters pbr_param;

        auto& colorFactor = mtl.pbrMetallicRoughness.baseColorFactor;
        pbr_param.m_base_color.r = colorFactor[0];
        pbr_param.m_base_color.g = colorFactor[1];
        pbr_param.m_base_color.b = colorFactor[2];
        pbr_param.m_base_color.a = colorFactor[3];

        pbr_param.m_metallic = mtl.pbrMetallicRoughness.metallicFactor;
        pbr_param.m_roughness = mtl.pbrMetallicRoughness.roughnessFactor;

        memcpy(data_buffer.data() + pbr_parameter_offset, &pbr_param,
               sizeof(graphics::PBRParameters));

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

    graphics::Buffer pbr_parameter_buffer;
    if (!data_buffer.empty()) {
        pbr_parameter_buffer =
            copyBuffer2GPU(adapter.GetDevice(), std::span{data_buffer},
                           Flags{graphics::BufferUsage::Uniform} |
                               graphics::BufferUsage::CopyDst);

        for (auto& desc : mtl_desces) {
            desc.pbr_param_buffer = pbr_parameter_buffer;
            materials.push_back(
                graphics::Material3D{gltf_mgr.m_mtl_allocator.Allocate(
                    &gltf_mgr, desc, common_res.m_camera_buffer,
                    common_res.m_view_buffer,
                    render_pass.GetBindGroupLayout())});
        }
    }

    return materials;
}

std::vector<graphics::BufferView> GLTFLoader::loadVertexBuffer(
    std::vector<unsigned char>& out_vertex_buffer,
    std::vector<unsigned char>& out_index_buffer,
    const std::set<uint32_t>& vertex_accessor,
    const std::set<uint32_t>& index_accessor) const {
    NICKEL_ASSERT(out_vertex_buffer.empty());

    std::vector<graphics::BufferView> views;
    views.reserve(m_gltf_model.accessors.size());

    for (int i = 0; i < m_gltf_model.accessors.size(); i++) {
        std::vector<unsigned char>* buffer =
            index_accessor.contains(i) ? &out_index_buffer : &out_vertex_buffer;

        auto& accessor = m_gltf_model.accessors[i];
        graphics::BufferView buffer_view;
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

Flags<Node::Flag> GLTFLoader::recordNodeInfoRecursive(
    std::vector<Node>& nodes, Node& node, Transform* parent_transform) const {
    if (!parent_transform) {
        node.m_global_transform = node.m_local_transform;
    } else {
        node.m_global_transform = *parent_transform * node.m_local_transform;
    }

    for (auto& child : node.m_children) {
        auto child_flag = recordNodeInfoRecursive(nodes, nodes[child],
                                                  &node.m_global_transform);
        node.m_flags |= child_flag;
    }

    return node.m_flags;
}

}  // namespace nickel