#pragma once
#include "nickel/common/impl_wrapper.hpp"
#include "nickel/common/math/math.hpp"
#include "nickel/common/memory/memory.hpp"
#include "nickel/fs/path.hpp"
#include "nickel/graphics/lowlevel/adapter.hpp"
#include "nickel/graphics/lowlevel/buffer.hpp"
#include "nickel/graphics/lowlevel/sampler.hpp"
#include "nickel/graphics/material.hpp"
#include "nickel/graphics/mesh.hpp"
#include "nickel/graphics/texture.hpp"

namespace nickel::graphics {

struct GLTFGPUResource {
    Buffer pbr_parameter_buffer;
    std::vector<Buffer> dataBuffers;
    std::vector<Texture> textures;
    std::vector<Sampler> samplers;
    std::vector<Material3D> materials;
};

struct GLTFCPUData {
    std::vector<PBRParameters> pbr_parameters;
    std::vector<std::vector<unsigned char>> data_buffers;
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

class GLTFManager {
public:
    GLTFManager();
    ~GLTFManager();

    bool Load(const Path&, const GLTFLoadConfig& = {});
    GLTFModel Find(const std::string&);
    void GC();

private:
    std::unique_ptr<GLTFManagerImpl> m_impl;
};


}  // namespace nickel::graphics
