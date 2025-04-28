#include "nickel/graphics/internal/gltf_manager_impl.hpp"
#include "nickel/common/common.hpp"
#include "nickel/common/macro.hpp"
#include "nickel/context.hpp"
#include "nickel/graphics/common_resource.hpp"
#include "nickel/graphics/gltf_draw.hpp"
#include "nickel/graphics/internal/gltf_model_impl.hpp"
#include "nickel/importer/internal/gltf_importer_impl.hpp"

#include <unordered_set>

namespace nickel::graphics {

GLTFModelManagerImpl::GLTFModelManagerImpl(Device device, CommonResource& res,
                                 GLTFRenderPass& gltf_render_pass) {
    {
        Buffer::Descriptor desc;
        desc.m_memory_type = MemoryType::GPULocal;
        desc.m_usage = Flags{BufferUsage::Uniform} | BufferUsage::CopyDst;
        desc.m_size = sizeof(PBRParameters);
        PBRParameters param;
        param.m_base_color = Vec4(1, 1, 1, 1);
        param.m_metallic = 0.3;
        param.m_roughness = 0.3;
        m_default_pbr_param_buffer = device.CreateBuffer(desc);
        m_default_pbr_param_buffer.BuffData(&param, sizeof(param), 0);
    }

    {
        Material3D::Descriptor desc;
        desc.basicTexture.image = res.m_white_image;
        desc.basicTexture.sampler = res.m_default_sampler;
        desc.normalTexture.image = res.m_default_normal_image;
        desc.normalTexture.sampler = res.m_default_sampler;
        desc.occlusionTexture.image = res.m_white_image;
        desc.occlusionTexture.sampler = res.m_default_sampler;
        desc.metalicRoughnessTexture.image = res.m_black_image;
        desc.metalicRoughnessTexture.sampler = res.m_default_sampler;
        desc.pbr_param_buffer = m_default_pbr_param_buffer;
        desc.pbrParameters.m_offset = 0;
        desc.pbrParameters.m_size = sizeof(PBRParameters);
        desc.pbrParameters.m_count = 1;
        m_default_material = m_mtl_allocator.Allocate(
            this, desc, res.m_camera_buffer, res.m_view_buffer,
            gltf_render_pass.GetBindGroupLayout());
    }
}

GLTFModelManagerImpl::~GLTFModelManagerImpl() {
    m_models.clear();

    m_model_allocator.FreeAll();
    m_mesh_allocator.FreeAll();
    m_model_resource_allocator.FreeAll();
    m_mtl_allocator.FreeAll();
}

bool GLTFModelManagerImpl::Load(const GLTFImportData& load_data,
                                const GLTFLoadConfig& load_config) {
    if (!load_config.ShouldLoadMesh()) {
        return true;
    }

    if (load_config.ShouldCombineMeshes()) {
        // NOTE: currently we only load one scene
        GLTFModelImpl* root_model_impl = m_model_allocator.Allocate(this);
        std::optional<uint32_t> unique_mesh_root_node;
        for (auto idx : load_data.m_root_nodes) {
            auto& root_node = load_data.m_nodes[idx];
            if (root_node.IsInMeshTree()) {
                if (!unique_mesh_root_node) {
                    unique_mesh_root_node = idx;
                } else {
                    unique_mesh_root_node = std::nullopt;
                    break;
                }
            }
        }

        if (unique_mesh_root_node) {
            auto& node = load_data.m_nodes[*unique_mesh_root_node];
            if (node.m_mesh) {
                root_model_impl->m_mesh = load_data.m_meshes[node.m_mesh.value()];
            }
            root_model_impl->m_resource = load_data.m_resource;
            root_model_impl->m_name = node.m_name;
            root_model_impl->m_transform = node.m_local_transform.ToMat();
            for (auto& node_idx : node.m_children) {
                auto& root_node = load_data.m_nodes[node_idx];
                NICKEL_CONTINUE_IF_FALSE(root_node.IsInMeshTree());
                preorderNode(load_data, root_node, load_data.m_resource,
                             std::span{load_data.m_meshes}, *root_model_impl);
                root_model_impl->m_name = root_node.m_name;
            }
        } else {
            root_model_impl->m_name = "gltf_import_root";
            for (auto& root_node_idx : load_data.m_root_nodes) {
                auto& root_node = load_data.m_nodes[root_node_idx];
                NICKEL_CONTINUE_IF_FALSE(root_node.IsInMeshTree());
                preorderNode(load_data, root_node, load_data.m_resource,
                             std::span{load_data.m_meshes}, *root_model_impl);
                root_model_impl->m_name = root_node.m_name;
            }
        }

        
        m_models[load_data.m_asset_name] = root_model_impl;
    } else {
        std::unordered_set<std::string> mesh_names;
        const std::unordered_set<std::string>* using_mesh_set =
            &load_config.m_meshes;

        if (load_config.ShouldLoadAllMeshes()) {
            for (auto& node : load_data.m_nodes) {
                if (node.m_mesh) {
                    mesh_names.emplace(node.m_name);
                }
            }
            using_mesh_set = &mesh_names;
        }

        for (auto& node : load_data.m_nodes) {
            NICKEL_CONTINUE_IF_FALSE(node.m_mesh &&
                                     using_mesh_set->contains(node.m_name));
            auto& mesh = load_data.m_meshes[node.m_mesh.value()];

            GLTFModelImpl* model = m_model_allocator.Allocate(this);
            model->m_mesh = mesh;
            model->m_resource = load_data.m_resource;
            model->m_name = load_data.m_asset_name + "." + node.m_name;
            model->m_transform = node.m_global_transform.ToMat();

            if (auto it = m_models.find(model->m_name); it != m_models.end()) {
                it->second->DecRefcount();
                LOGE("model {} already loaded, will replace it", model->m_name);
            }

            m_models[model->m_name] = model;
        }
    }
    return true;
}

GLTFModel GLTFModelManagerImpl::Find(const std::string& name) {
    if (auto it = m_models.find(name); it != m_models.end()) {
        return it->second;
    }
    return {};
}

void GLTFModelManagerImpl::GC() {
    for (auto pending_delete : m_pending_delete) {
        m_models.erase(pending_delete);
    }

    m_model_allocator.GC();
    m_mesh_allocator.GC();
    m_model_resource_allocator.GC();
    m_mtl_allocator.GC();
}

void GLTFModelManagerImpl::Remove(GLTFModelImpl& impl) {
    for (auto it = m_models.begin(); it != m_models.end(); it++) {
        if (it->second == &impl) {
            m_pending_delete.insert(it->first);
            return;
        }
    }
}

void GLTFModelManagerImpl::Clear() {
    m_models.clear();

    m_model_allocator.GC();
    m_mesh_allocator.GC();
    m_model_resource_allocator.GC();
    m_mtl_allocator.GC();
}

std::vector<std::string> GLTFModelManagerImpl::GetAllGLTFModelNames() const {
    std::vector<std::string> names;
    for (auto& [name, _] : m_models) {
        names.push_back(name);
    }
    return names;
}

void GLTFModelManagerImpl::preorderNode(const GLTFImportData& load_data,
                                        const Node& node,
                                        const GLTFModelResource& resource,
                                        std::span<const Mesh> meshes,
                                        GLTFModelImpl& parent_model) {
    NICKEL_RETURN_IF_FALSE(node.IsInMeshTree());

    GLTFModelImpl* model = m_model_allocator.Allocate(this);
    model->m_name = node.m_name;
    model->m_transform = node.m_local_transform.ToMat();
    if (node.m_mesh) {
        model->m_mesh = meshes[node.m_mesh.value()];
        model->m_resource = resource;
    }
    parent_model.m_children.push_back(model);

    for (auto& child : node.m_children) {
        preorderNode(load_data, load_data.m_nodes[child], resource, meshes,
                     *parent_model.m_children.back().GetImpl());
    }
}

}  // namespace nickel::graphics