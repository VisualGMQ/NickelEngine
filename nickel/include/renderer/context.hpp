#pragma once

#include "core/log_tag.hpp"
#include "freetype/freetype.h"
#include "pch.hpp"
#include "core/cgmath.hpp"
#include "renderer/texture.hpp"
#include "renderer/vertex.hpp"

namespace nickel {

/**
 * @brief [resource][inner] render context
 */
struct RenderContext final {
    struct RenderInfo {
        std::array<Vertex, 4> vertices;
        cgmath::Mat44 model;
        const Texture* texture;
    };

    std::vector<RenderInfo> renderInfos;
    float depthBias = 0;
    float depthBiasStep = 0.0001;

    void ResetBias() {
        depthBias = 0;
    }

    void ClearRenderInfo() {
        renderInfos.clear();
    }
};

}