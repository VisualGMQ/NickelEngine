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

GLTFManager::GLTFManager() : m_impl{std::make_unique<GLTFManagerImpl>()} {}

GLTFManager::~GLTFManager() {}

GLTFVertexData GLTFVertexDataLoader::Load(const Path& filename,
                                          const std::string& node_name) {
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

    return Load(gltf_model, node_name);
}

GLTFVertexData GLTFVertexDataLoader::Load(const tinygltf::Model& gltf_model,
                                          const std::string& node_name) {
    NICKEL_ASSERT(gltf_model.scenes.size() == 1);
    GLTFVertexData vertex_data;
    Mat44 mat;

    for (auto& scene : gltf_model.scenes) {
        for (auto node : scene.nodes) {
            if (parseNode(gltf_model, gltf_model.nodes[node], node_name,
                          vertex_data, Mat44::Identity(), mat)) {
                break;
            }
        }
    }

    LOGE("no node name {} when load gltf vertex data", node_name);

    return {};
}

bool GLTFVertexDataLoader::parseNode(const tinygltf::Model& model,
                                     const tinygltf::Node& node,
                                     const std::string& name,
                                     GLTFVertexData& out_data,
                                     const Mat44& parent_mat, Mat44& out_mat) {
    bool found = false;
    Mat44 global_pose = parent_mat * CalcNodeTransform(node);
    if (node.name != name && node.mesh != -1) {
        for (auto child : node.children) {
            found = found || parseNode(model, model.nodes[child], name,
                                       out_data, global_pose, out_mat);
        }
        return found;
    }

    auto& mesh = model.meshes[node.mesh];
    for (auto& prim : mesh.primitives) {
        size_t triangle_count{};
        size_t old_triangle_count = out_data.m_positions.size() / 3;

        auto& attrs = prim.attributes;
        if (auto it = attrs.find("POSITION"); it != attrs.end()) {
            auto& accessor = model.accessors[it->second];
            auto& buffer_view = model.bufferViews[accessor.bufferView];
            auto& buffer = model.buffers[buffer_view.buffer];
            uint32_t offset = buffer_view.byteOffset + accessor.byteOffset;
            triangle_count = accessor.count / 3;
            out_data.m_positions.resize(out_data.m_positions.size() +
                                        sizeof(Vec3) * accessor.count);

            NICKEL_ASSERT(accessor.type == TINYGLTF_TYPE_VEC3 &&
                          accessor.componentType ==
                              TINYGLTF_COMPONENT_TYPE_FLOAT);

            ConvertRangeData(out_data.m_positions.data(),
                             (Vec3*)(buffer.data.data() + offset),
                             accessor.count, 1, 1);
        }

        if (prim.indices != -1) {
            auto& accessor = model.accessors[prim.indices];
            auto& buffer_view = model.bufferViews[accessor.bufferView];
            auto& buffer = model.buffers[buffer_view.buffer];
            uint32_t offset = buffer_view.byteOffset + accessor.byteOffset;
            out_data.m_indices.resize(out_data.m_indices.size() +
                                      sizeof(uint32_t) * accessor.count);
            if (accessor.componentType ==
                TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT) {
                ConvertRangeData(out_data.m_indices.data(),
                                 (uint16_t*)(buffer.data.data() + offset),
                                 accessor.count, 1, 1);
            } else if (accessor.componentType ==
                       TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT) {
                ConvertRangeData(out_data.m_indices.data(),
                                 (uint32_t*)(buffer.data.data() + offset),
                                 accessor.count, 1, 1);
            } else {
                NICKEL_CANT_REACH();
            }
        } else {
            out_data.m_indices.resize(3 * triangle_count);
            for (int i = 0; i < triangle_count; i++) {
                uint32_t idx = i + old_triangle_count;
                out_data.m_indices[idx * 3] = idx;
                out_data.m_indices[idx * 3 + 1] = idx + 1;
                out_data.m_indices[idx * 3 + 2] = idx + 2;
            }
        }
    }

    out_mat = global_pose;
    return true;
}

}  // namespace nickel::graphics
