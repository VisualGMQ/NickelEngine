#pragma once
#include "nickel/graphics/material.hpp"
#include "nickel/graphics/lowlevel/buffer.hpp"
#include "nickel/common/math/math.hpp"

namespace nickel::graphics {
struct Primitive final {
    std::string name;
    std::optional<uint32_t> posBufView{};
    std::optional<uint32_t> normBufView{};
    std::optional<uint32_t> tanBufView{};
    std::optional<uint32_t> uvBufView{};
    std::optional<uint32_t> colorBufView{};
    std::optional<uint32_t> indicesBufView{};
    std::optional<uint32_t> material;
};

struct GPUMesh final {
    Mat44 localModelMat = Mat44::Identity();
    Mat44 modelMat = Mat44::Identity();
    std::vector<Primitive> primitives;
    std::vector<std::unique_ptr<GPUMesh>> children;
};

struct Scene final {
    std::vector<std::unique_ptr<GPUMesh>> nodes;
};
}
