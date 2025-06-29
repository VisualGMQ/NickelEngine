#pragma once
#include <span>

namespace nickel::graphics {

class ShaderCompilerImpl;

struct SpirVBinary {
    std::vector<unsigned int> m_data;

    operator bool() const noexcept { return !m_data.empty(); }
};

enum class ShaderType {
    Vertex,
    Fragment,
    Geometry,
    TesselationController,
    TesselationEvaluation,
    Compute
};

class ShaderCompiler {
public:
    static void InitCompilerSystem();
    static void ShutdownCompilerSystem();

    ShaderCompiler();
    ~ShaderCompiler();
    
    SpirVBinary Compile(std::span<const char> code, ShaderType stage);

private:
    std::unique_ptr<ShaderCompilerImpl> m_impl;
};

}  // namespace nickel::graphics