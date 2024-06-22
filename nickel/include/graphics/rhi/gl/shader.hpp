#pragma once
#include "graphics/rhi/impl/shader.hpp"
#include "graphics/rhi/shader.hpp"
#include "graphics/rhi/common.hpp"
#include "graphics/rhi/gl/glpch.hpp"

namespace nickel::rhi::gl {

class ShaderModuleImpl: public rhi::ShaderModuleImpl {
public:
    ShaderModuleImpl(const ShaderModule::Descriptor&);

    GLuint CreateShader(GLenum stage);

private:
    ShaderModule::Descriptor desc_;
};

}