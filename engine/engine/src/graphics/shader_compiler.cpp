#include "nickel/graphics/lowlevel/shader_compiler.hpp"
#include "glslang/Public/ShaderLang.h"
#include "nickel/graphics/lowlevel/internal/shader_compiler_impl.hpp"

namespace nickel::graphics {

void ShaderCompiler::InitCompilerSystem() {
    glslang::InitializeProcess();
}

void ShaderCompiler::ShutdownCompilerSystem() {
    glslang::FinalizeProcess();
}

ShaderCompiler::ShaderCompiler()
    : m_impl{std::make_unique<ShaderCompilerImpl>()} {}

SpirVBinary ShaderCompiler::Compile(std::span<const char> code,
                                    ShaderType stage) {
    return m_impl->Compile(code, stage);
}

ShaderCompiler::~ShaderCompiler() {}

}  // namespace nickel::graphics