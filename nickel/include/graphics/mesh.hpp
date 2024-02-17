#pragma once
#include "common/cgmath.hpp"
#include "graphics/texture.hpp"
#include "stdpch.hpp"

namespace nickel {

class Model;

struct BufferAccessor {
    uint32_t viewIdx;
    uint32_t offset;
    uint32_t count;
};

struct BufferView {
    uint32_t bufIdx;
    uint32_t offset;
    uint32_t size;
};

struct Material {
    std::optional<uint32_t> textureIdx;
    std::optional<uint32_t> uvIdx;
    // TODO: support PBR
};

struct Primitive {
    std::optional<uint32_t> posBufIdx;
    std::optional<uint32_t> uvBufIdx;
    std::optional<uint32_t> normBufIdx;
    std::optional<uint32_t> tangentIdx;
    std::optional<uint32_t> colorIdx;
    std::optional<uint32_t> indicesIdx;
    std::optional<uint32_t> materialIdx;
};

class Model;

class Mesh final {
public:
    Model* model{};
    std::vector<Primitive> primitives;
};

class Model final {
public:
    std::vector<Mesh> meshes;
    std::vector<std::vector<unsigned char>> buffers;
    std::vector<BufferView> bufferViews;
    std::vector<BufferAccessor> accessors;
    std::vector<TextureHandle> textures;
    std::vector<Material> materials;
};

Model LoadModelFromFile(const std::filesystem::path&, TextureManager&);

}  // namespace nickel