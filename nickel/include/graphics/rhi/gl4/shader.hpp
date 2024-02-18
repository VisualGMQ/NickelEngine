#pragma once
#include "graphics/rhi/impl/shader.hpp"
#include "graphics/rhi/shader.hpp"
#include "graphics/rhi/common.hpp"
#include "glad/glad.h"

namespace nickel::rhi::gl4 {

class ShaderModuleImpl: public rhi::ShaderModuleImpl {
public:
    ShaderModuleImpl(const ShaderModule::Descriptor&);
    ~ShaderModuleImpl();

    GLuint CreateShader(GLenum stage);

private:
    ShaderModule::Descriptor desc_;
};

}