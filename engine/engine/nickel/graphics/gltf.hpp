#pragma once
#include "nickel/common/impl_wrapper.hpp"
#include "nickel/fs/path.hpp"
#include "nickel/graphics/lowlevel/adapter.hpp"
#include "nickel/graphics/lowlevel/buffer.hpp"
#include "nickel/graphics/lowlevel/sampler.hpp"
#include "nickel/graphics/material.hpp"
#include "nickel/graphics/texture.hpp"

namespace tinygltf {
class Model;
class Node;
}  // namespace tinygltf

namespace nickel::graphics {

struct GLTFCPUData {
    std::vector<PBRParameters> pbr_parameters;
    std::vector<unsigned char> vertex_buffer;
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

class GLTFManagerImpl;

struct GLTFLoadConfig {
    bool m_combine_mesh = true;
};

class CommonResource;
class GLTFRenderPass;

class GLTFManager {
public:
    GLTFManager(Device device, CommonResource& res,
                GLTFRenderPass& render_pass);
    ~GLTFManager();

    bool Load(const Path&, const GLTFLoadConfig& = {});
    GLTFModel Find(const std::string&);
    void GC();

private:
    std::unique_ptr<GLTFManagerImpl> m_impl;
};

struct GLTFVertexData {
    std::vector<Vec3> m_positions;
    std::vector<uint32_t> m_indices;
};

class GLTFVertexDataLoader {
public:
    GLTFVertexData Load(const Path& path, const std::string& node_name);
    GLTFVertexData Load(const tinygltf::Model&, const std::string& node_name);

    bool parseNode(const tinygltf::Model& model, const tinygltf::Node& node,
                   const std::string& name, GLTFVertexData& out_data,
                   const Mat44& parent_mat, Mat44& out_mat);
};

}  // namespace nickel::graphics
