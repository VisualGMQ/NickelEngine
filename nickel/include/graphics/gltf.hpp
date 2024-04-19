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

    toml::table Save2Toml() const override;

    ~GLTFModel();

    operator bool() const;

private:
    bool valid_ = false;
};

template <>
std::unique_ptr<GLTFModel> LoadAssetFromMeta<GLTFModel>(class toml::v3::table const &);

using GLTFHandle = Handle<GLTFModel>;

class GLTFManager : public Manager<GLTFModel> {
public:
    static FileType GetFileType() { return FileType::GLTF; }

    GLTFHandle Load(const std::filesystem::path& filename);
};

struct GLTFBundle {
    Transform transform;
    GLTFHandle gltf; 
};

}  // namespace nickel