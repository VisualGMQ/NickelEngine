#pragma once
#include "nickel/common/impl_wrapper.hpp"
#include "nickel/common/transform.hpp"
#include "nickel/fs/path.hpp"
#include "nickel/graphics/lowlevel/adapter.hpp"
#include "nickel/graphics/material.hpp"
#include "nickel/importer/gltf_importer_config.hpp"

namespace tinygltf {
class Model;
class Node;
}  // namespace tinygltf

namespace nickel {
struct GLTFImportData;
}

namespace nickel::graphics {

struct GLTFCPUData {
    std::vector<PBRParameters> pbr_parameters;
    std::vector<unsigned char> vertex_buffer;
    std::vector<unsigned char> bone_weight_buffer;
    std::vector<unsigned char> indices_buffer;
};

struct GLTFModelResourceImpl;

struct GLTFModelResource final : public ImplWrapper<GLTFModelResourceImpl> {
    using ImplWrapper::ImplWrapper;
};

class GLTFModelImpl;

class GLTFModel final : public ImplWrapper<GLTFModelImpl> {
public:
    using ImplWrapper::ImplWrapper;
};

class GLTFModelManagerImpl;

class CommonResource;
class GLTFRenderPass;

class GLTFModelManager {
public:
    GLTFModelManager(Device device, CommonResource& res,
                     GLTFRenderPass& render_pass);
    ~GLTFModelManager();

    bool Load(const GLTFImportData& load_data,
              const GLTFLoadConfig& load_config);
    GLTFModel Find(const std::string&);
    void GC();
    void Clear();
    std::vector<std::string> GetAllGLTFModelNames() const;

    GLTFModelManagerImpl* GetImpl();
    const GLTFModelManagerImpl* GetImpl() const;

private:
    std::unique_ptr<GLTFModelManagerImpl> m_impl;
};

struct GLTFVertexData {
    std::string m_name;
    Transform m_transform;
    std::vector<Vec3> m_points;
    std::vector<uint32_t> m_indices;
};

struct GLTFVertexDataLoadConfig {
    bool m_use_parent_coord = true;
};

class GLTFVertexDataLoader {
public:
    std::vector<GLTFVertexData> Load(const Path& path,
                                     bool apply_transform = false);
    std::vector<GLTFVertexData> Load(const tinygltf::Model&,
                                     bool apply_transform = false);

    void parseNode(const tinygltf::Model& model, const tinygltf::Node& node,
                   const Transform& parent_transform, bool apply_transform,
                   std::vector<GLTFVertexData>& result);
    Transform calcNodeTransform(const tinygltf::Node& node);
};

}  // namespace nickel::graphics
