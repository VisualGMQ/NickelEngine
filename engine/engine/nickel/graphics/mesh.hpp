#pragma once
#include "nickel/common/math/math.hpp"
#include "nickel/common/transform.hpp"
#include "nickel/graphics/material.hpp"

namespace nickel::graphics {
struct Primitive final {
    std::string name;
    BufferView posBufView;
    BufferView normBufView;
    BufferView tanBufView;
    BufferView uvBufView;
    BufferView colorBufView;
    BufferView indicesBufView;
    IndexType index_type;
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

    void updateTransform(const Transform&);

private:
    void preorderGPUMesh(const Mat44& parent_mat, GPUMesh& mesh);
};
}
