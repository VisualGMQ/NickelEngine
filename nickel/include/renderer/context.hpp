#pragma once

namespace nickel {

/**
 * @brief [resource][inner] render context
 */
struct RenderContext final {
    float depthBias = 0;
    float depthBiasStep = 0.0001;

    void Reset() {
        depthBias = 0;
    }
};

}