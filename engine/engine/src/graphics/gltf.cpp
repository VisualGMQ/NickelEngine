#include "nickel/graphics/gltf.hpp"

#include "nickel/graphics/internal/gltf_loader.hpp"
#include "nickel/graphics/internal/gltf_manager_impl.hpp"
#include "nickel/graphics/texture_manager.hpp"
#include "tiny_gltf.h"

namespace nickel::graphics {

bool GLTFManager::Load(const Path& filename,
                       const GLTFLoadConfig& load_config) {
    return m_impl->Load(filename, load_config);
}

GLTFModel GLTFManager::Find(const std::string& name) {
    return m_impl->Find(name);
}

void GLTFManager::GC() {
    m_impl->GC();
}

GLTFManager::GLTFManager(Device device, CommonResource& res,
                         GLTFRenderPass& render_pass)
    : m_impl{std::make_unique<GLTFManagerImpl>(device, res, render_pass)} {}

GLTFManager::~GLTFManager() {}

std::vector<GLTFVertexData> GLTFVertexDataLoader::Load(const Path& filename) {
    tinygltf::TinyGLTF tiny_gltf_loader;
    std::string err, warn;
    tinygltf::Model gltf_model;
    if (!tiny_gltf_loader.LoadASCIIFromFile(&gltf_model, &err, &warn,
                                            filename.ToString())) {
        LOGE("load model from {} failed: \n\terr: {}\n\twarn: {}", filename,
             err, warn);
        return {};
    }

    if (gltf_model.nodes.empty()) {
        LOGE("load model from {} failed: no nodes", filename);
        return {};
    }

    return Load(gltf_model);
}

std::vector<GLTFVertexData> GLTFVertexDataLoader::Load(
    const tinygltf::Model& gltf_model) {
    NICKEL_ASSERT(gltf_model.scenes.size() == 1);

    std::vector<GLTFVertexData> result;
    for (auto& scene : gltf_model.scenes) {
        for (auto node : scene.nodes) {
            if (parseNode(gltf_model, gltf_model.nodes[node],
                          {}, result)) {
                break;
            }
        }
    }

    return result;
}

bool GLTFVertexDataLoader::parseNode(const tinygltf::Model& model,
                                     const tinygltf::Node& node,
                                     const Transform& parent_transform,
                                     std::vector<GLTFVertexData>& result) {
    Transform global_pose = parent_transform * calcNodeTransform(node);

    auto& mesh = model.meshes[node.mesh];
    GLTFVertexData vertex_data;
    vertex_data.m_name = node.name;
    for (auto& prim : mesh.primitives) {
        auto& attrs = prim.attributes;
        size_t old_size = vertex_data.m_positions.size();
        if (auto it = attrs.find("POSITION"); it != attrs.end()) {
            auto& accessor = model.accessors[it->second];
            auto& buffer_view = model.bufferViews[accessor.bufferView];
            auto& buffer = model.buffers[buffer_view.buffer];
            uint32_t offset = buffer_view.byteOffset + accessor.byteOffset;
            vertex_data.m_positions.resize(vertex_data.m_positions.size() +
                                           accessor.count);

            NICKEL_ASSERT(accessor.type == TINYGLTF_TYPE_VEC3 &&
                          accessor.componentType ==
                              TINYGLTF_COMPONENT_TYPE_FLOAT);

            ConvertRangeData((Vec3*)(buffer.data.data() + offset),
                             vertex_data.m_positions.data(),
                             accessor.count, 1, 1);
        }

        if (prim.indices != -1) {
            auto& accessor = model.accessors[prim.indices];
            auto& buffer_view = model.bufferViews[accessor.bufferView];
            auto& buffer = model.buffers[buffer_view.buffer];
            uint32_t offset = buffer_view.byteOffset + accessor.byteOffset;
            vertex_data.m_indices.resize(vertex_data.m_indices.size() +
                                         accessor.count);
            if (accessor.componentType ==
                TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT) {
                ConvertRangeData((uint16_t*)(buffer.data.data() + offset),
                                 vertex_data.m_indices.data(), accessor.count,
                                 1, 1);
            } else if (accessor.componentType ==
                       TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT) {
                ConvertRangeData((uint32_t*)(buffer.data.data() + offset),
                                 vertex_data.m_indices.data(), accessor.count,
                                 1, 1);
            } else {
                NICKEL_CANT_REACH();
            }

            auto data = std::span{vertex_data.m_indices};
            std::ranges::transform(
                data.begin(), data.end(), data.begin(),
                [=](uint32_t value) { return value + old_size; });
        }
    }

    if (!vertex_data.m_positions.empty()) {
        if (global_pose.scale != Vec3{1, 1, 1}) {
            std::ranges::transform(
                vertex_data.m_positions.begin(), vertex_data.m_positions.end(),
                vertex_data.m_positions.begin(),
                [=](const Vec3& p) { return global_pose.scale * p; });
        }
        vertex_data.m_transform.p = global_pose.p;
        vertex_data.m_transform.q = global_pose.q;
        result.push_back(vertex_data);
    }

    for (auto& child : node.children) {
        parseNode(model, model.nodes[child], global_pose, result);
    }

    return true;
}

Transform GLTFVertexDataLoader::calcNodeTransform(const tinygltf::Node& node) {
    if (!node.matrix.empty()) {
        // TODO: decompose SRT matrix
        return {};
    }
    Transform transform;
    if (!node.scale.empty() || !node.translation.empty() ||
        !node.rotation.empty()) {
        if (!node.scale.empty()) {
            transform.scale = Vec3(node.scale[0], node.scale[1], node.scale[2]);
        }
        if (!node.rotation.empty()) {
            transform.q = Quat(node.rotation[0], node.rotation[1],
                               node.rotation[2], node.rotation[3]);
        }
        if (!node.translation.empty()) {
            transform.p = Vec3(node.translation[0], node.translation[1],
                               node.translation[2]);
        }
    }

    return transform;
}

}  // namespace nickel::graphics
