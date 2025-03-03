#include "nickel/graphics/internal/gltf_manager_impl.hpp"
#include "nickel/graphics/common_resource.hpp"
#include "nickel/graphics/gltf_draw.hpp"
#include "nickel/graphics/internal/gltf_loader.hpp"
#include "nickel/graphics/internal/gltf_model_impl.hpp"
#include "nickel/graphics/internal/mesh_impl.hpp"
#include "nickel/nickel.hpp"

namespace nickel::graphics {

GLTFManagerImpl::GLTFManagerImpl(Device device, CommonResource& res,
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

GLTFManagerImpl::~GLTFManagerImpl() {
    m_models.clear();

    m_model_allocator.FreeAll();
    m_mesh_allocator.FreeAll();
    m_model_resource_allocator.FreeAll();
    m_mtl_allocator.FreeAll();
}

bool GLTFManagerImpl::Load(const Path& filename,
                           const GLTFLoadConfig& load_config) {
    tinygltf::TinyGLTF tiny_gltf_loader;
    std::string err, warn;
    tinygltf::Model gltf_model;
    if (!tiny_gltf_loader.LoadASCIIFromFile(&gltf_model, &err, &warn,
                                            filename.ToString())) {
        LOGE("load model from {} failed: \n\terr: {}\n\twarn: {}", filename,
             err, warn);
        return false;
    }

    if (gltf_model.nodes.empty()) {
        LOGE("load model from {} failed: no nodes", filename);
        return false;
    }

    GLTFLoader loader(gltf_model);
    auto load_data = loader.Load(
        filename, nickel::Context::GetInst().GetGPUAdapter(), *this);

    Path pure_filename = filename.Filename().ReplaceExtension("");
    Path parent_dir = filename.ParentPath();
    std::string final_name = (parent_dir / pure_filename).ToString();
    std::replace(final_name.begin(), final_name.end(), '\\', '/');
    if (load_config.m_combine_mesh) {
        // TODO: currently we only load one scene
        GLTFModel root_model = m_model_allocator.Allocate(this);
        GLTFModelImpl* root_model_impl = root_model.GetImpl();
        for (auto& node : gltf_model.scenes[0].nodes) {
            preorderNode(gltf_model, gltf_model.nodes[node],
                         load_data.m_resource, std::span{load_data.m_meshes},
                         *root_model_impl);
        }
        if (root_model_impl->m_children.size() == 1) {
            root_model_impl->DecRefcount();
            root_model = root_model_impl->m_children[0];
        } else {
            root_model_impl->m_name = gltf_model.scenes[0].name;
        }
        m_models[final_name] = root_model_impl;
        return true;
    } else {
        for (auto& mesh : load_data.m_meshes) {
            GLTFModelImpl* model = m_model_allocator.Allocate(this);
            model->m_mesh = mesh;
            model->m_resource = load_data.m_resource;
            model->m_name = final_name + "." + mesh.GetImpl()->m_name;
            m_models[model->m_name] = model;

            if (m_models.contains(model->m_name)) {
                LOGE("model {} already loaded", model->m_name);
            }
        }
        return true;
    }
}

GLTFModel GLTFManagerImpl::Find(const std::string& name) {
    if (auto it = m_models.find(name); it != m_models.end()) {
        return it->second;
    }
    return {};
}

void GLTFManagerImpl::GC() {
    m_model_allocator.GC();
    m_mesh_allocator.GC();
    m_model_resource_allocator.GC();
    m_mtl_allocator.GC();
}

void GLTFManagerImpl::Remove(GLTFModelImpl& impl) {
    for (auto it = m_models.begin(); it != m_models.end(); it++) {
        if (it->second == &impl) {
            m_models.erase(it);
            return;
        }
    }
}

void GLTFManagerImpl::preorderNode(const tinygltf::Model& gltf_model,
                                   const tinygltf::Node& gltf_node,
                                   const GLTFModelResource& resource,
                                   std::span<Mesh> meshes,
                                   GLTFModelImpl& parent_model) {
    GLTFModelImpl* model = m_model_allocator.Allocate(this);
    model->m_name = gltf_node.name;
    model->m_transform = CalcNodeTransform(gltf_node);
    if (gltf_node.mesh != -1) {
        model->m_mesh = meshes[gltf_node.mesh];
        model->m_resource = resource;
    }
    parent_model.m_children.push_back(model);

    for (auto& child : gltf_node.children) {
        preorderNode(gltf_model, gltf_model.nodes[child], resource, meshes,
                     *parent_model.m_children.back().GetImpl());
    }
}

}  // namespace nickel::graphics