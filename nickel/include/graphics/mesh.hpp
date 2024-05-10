#pragma once

#include "stdpch.hpp"
#include "graphics/material.hpp"

namespace nickel {

struct Primitive final {
    BufferView posBufView;
    BufferView normBufView;
    BufferView tanBufView;
    BufferView uvBufView;
    BufferView colorBufView;
    BufferView indicesBufView;
    uint32_t material;
};

struct GPUMesh final {
    rhi::Buffer posBuf;
    rhi::Buffer normBuf;
    rhi::Buffer tanBuf;
    rhi::Buffer uvBuf;
    rhi::Buffer indicesBuf;

    std::vector<Primitive> primitives;

    GPUMesh() = default;
    GPUMesh(GPUMesh&& o) = default;
    GPUMesh& operator=(GPUMesh&& o) = default;

    ~GPUMesh() {
        posBuf.Destroy();
        normBuf.Destroy();
        tanBuf.Destroy();
        uvBuf.Destroy();
        indicesBuf.Destroy();
    }

    operator bool() const {
        // at least we need position information
        return posBuf;
    }
};

struct Node final {
    cgmath::Mat44 localModelMat = cgmath::Mat44::Identity();
    cgmath::Mat44 modelMat = cgmath::Mat44::Identity();
    GPUMesh mesh;
    std::vector<std::unique_ptr<Node>> children;
};

struct Scene final {
    std::unique_ptr<Node> node;
};

}