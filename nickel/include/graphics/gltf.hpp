#pragma once
#include "common/transform.hpp"
#include "graphics/mesh.hpp"
#include "graphics/context.hpp"

namespace nickel {

struct GLTFModel final: public Asset {
    std::vector<Scene> scenes;
    std::vector<rhi::Sampler> samplers;
    std::vector<std::unique_ptr<Material3D>> materials;
    rhi::Buffer pbrParamBuffer;

    static GLTFModel Null;

    GLTFModel() = default;
    GLTFModel(std::vector<Scene>&& scenes, std::vector<rhi::Sampler>&& samplers,
              std::vector<std::unique_ptr<Material3D>>&& materials, rhi::Buffer buffers);
    GLTFModel(GLTFModel&&) = default;
    GLTFModel& operator=(GLTFModel&&) = default;

    bool Load(const std::filesystem::path&) override;
    bool Load(const toml::table&) override;
    bool Save(toml::table&) const override;

    ~GLTFModel();

    operator bool() const;

private:
    bool valid_ = false;
};

using GLTFHandle = Handle<GLTFModel>;
struct GLTFBundle {
    Transform transform;
    GLTFHandle gltf; 
};

}  // namespace nickel