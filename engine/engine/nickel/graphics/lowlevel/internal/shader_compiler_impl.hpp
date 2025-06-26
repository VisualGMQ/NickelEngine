#pragma once

#include "glslang/Include/glslang_c_interface.h"
#include "glslang/Public/ResourceLimits.h"
#include "glslang/Public/ShaderLang.h"
#include "nickel/graphics/lowlevel/shader_compiler.hpp"

namespace nickel::graphics {

class ShaderCompilerImpl {
public:
    SpirVBinary Compile(std::span<const char> code, ShaderType);

private:
    EShLanguage getLangFromStage(ShaderType stage) const;
};

}  // namespace nickel::graphics