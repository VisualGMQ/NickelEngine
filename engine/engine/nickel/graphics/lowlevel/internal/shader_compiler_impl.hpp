#pragma once

#include "nickel/graphics/lowlevel/shader_compiler.hpp"

namespace nickel::graphics {

class ShaderCompilerImpl {
public:
    SpirVBinary Compile(std::span<const char> code, ShaderType);
};

}  // namespace nickel::graphics