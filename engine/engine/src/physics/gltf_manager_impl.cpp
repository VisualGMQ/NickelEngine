#include "nickel/graphics/internal/gltf_manager_impl.hpp"
#include "nickel/graphics/internal/gltf_loader.hpp"
#include "nickel/graphics/internal/gltf_model_impl.hpp"
#include "nickel/graphics/internal/mesh_impl.hpp"
#include "nickel/nickel.hpp"

namespace nickel::graphics {

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
        m_models[pure_filename.ToString()] = root_model;
    } else {
        for (auto& mesh : load_data.m_meshes) {
            GLTFModelImpl* model = m_model_allocator.Allocate(this);
            model->m_mesh = mesh;
            model->m_resource = load_data.m_resource;
            model->m_name =
                pure_filename.ToString() + "." + mesh.GetImpl()->m_name;
            m_models[model->m_name] = model;

            if (m_models.contains(model->m_name)) {
                LOGE("model {} already loaded", model->m_name);
            }
        }
        return true;
    }

    LOGE("{} load failed");
    return false;
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
        if (it->second.GetImpl() == &impl) {
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
    if (gltf_node.mesh != -1) {
        model->m_transform = calcNodeTransform(gltf_node);
        model->m_mesh = meshes[gltf_node.mesh];
        model->m_resource = resource;
    }
    parent_model.m_children.push_back(model);

    for (auto& child : gltf_node.children) {
        preorderNode(gltf_model, gltf_model.nodes[child], resource, meshes,
                     *parent_model.m_children.back().GetImpl());
    }
}

Mat44 GLTFManagerImpl::calcNodeTransform(const tinygltf::Node& node) {
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
            m = CreateScale(Vec3(node.scale[0], node.scale[1], node.scale[2])) *
                m;
        }
        if (!node.rotation.empty()) {
            m = Quat(node.rotation[0], node.rotation[1], node.rotation[2],
                     node.rotation[3])
                    .ToMat() *
                m;
        }
        if (!node.translation.empty()) {
            m = CreateTranslation(Vec3(node.translation[0], node.translation[1],
                                       node.translation[2])) *
                m;
        }
    }

    return m;
}

}  // namespace nickel::graphics