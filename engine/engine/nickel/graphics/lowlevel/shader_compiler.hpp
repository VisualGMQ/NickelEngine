#pragma once
#include <span>

namespace nickel::graphics {

class ShaderCompilerImpl;

/// Store SpirV code
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
    /// @warning  internal usage, init glslang shader compiler system
    static void InitCompilerSystem();

    /// @warning  internal usage, shutdown glslang shader compiler system
    static void ShutdownCompilerSystem();

    ShaderCompiler();
    ~ShaderCompiler();

    SpirVBinary Compile(std::span<const char> code, ShaderType stage);

private:
    std::unique_ptr<ShaderCompilerImpl> m_impl;
};

}  // namespace nickel::graphics