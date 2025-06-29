/**
 * @page shader_compiler_page Shader Compiler
 * `shader_compiler` is used to compile shader in runtime.
 *
 * ## How it works
 *
 * It use glslang internally to compile shader. It can compile GLSL to vulkan
 * supported SpirV
 *
 * ## Usage
 *
 * ```bash
 * shader_compiler <shader_file> -o <shader_output_file>
 * ```
 */

#include "lyra/lyra.hpp"
#include "nickel/common/assert.hpp"
#include "nickel/common/common.hpp"
#include "nickel/graphics/lowlevel/shader_compiler.hpp"
#include <filesystem>
#include <fstream>

nickel::graphics::ShaderType GetShaderTypeByExtension(
    const std::filesystem::path& ext) {
    if (ext == ".vert") {
        return nickel::graphics::ShaderType::Vertex;
    }
    if (ext == ".frag") {
        return nickel::graphics::ShaderType::Fragment;
    }
    if (ext == ".geom") {
        return nickel::graphics::ShaderType::Geometry;
    }
    if (ext == ".tesc") {
        return nickel::graphics::ShaderType::TesselationController;
    }
    if (ext == ".tese") {
        return nickel::graphics::ShaderType::TesselationEvaluation;
    }
    if (ext == ".comp") {
        return nickel::graphics::ShaderType::Compute;
    }

    std::cerr << "Unrecognized extension: " << ext << std::endl;
    assert(false);
}

int main(int argc, const char** argv) {
    std::filesystem::path filename;
    std::filesystem::path output_filename;
    bool show_help = false;
    auto cli = lyra::opt(output_filename, "output filename")["-o"]["--output"](
                   "output filename") |
               lyra::arg(filename, "filename")("missing input file") |
               lyra::help(show_help)["-h"]["--help"]["-?"](
                   "shader_compiler filename -o output_path");
    auto result = cli.parse({argc, argv});

    if (!result) {
        std::cerr << result.message() << std::endl;
        return 1;
    }

    if (filename.empty()) {
        std::cerr << "no input file" << std::endl;
        return 1;
    }

    std::ifstream file(filename);
    if (!file) {
        std::cerr << "read " << filename << " failed" << std::endl;
        return 1;
    }

    std::vector<char> content(std::istreambuf_iterator<char>(file), {});

    auto ext = filename.extension();

    nickel::graphics::ShaderCompiler::InitCompilerSystem();

    nickel::graphics::ShaderCompiler compiler;
    nickel::graphics::SpirVBinary spirv =
        compiler.Compile(content, GetShaderTypeByExtension(ext));

    if (!spirv) {
        std::cout << "compile spirv failed" << std::endl;
        nickel::graphics::ShaderCompiler::ShutdownCompilerSystem();
        return 2;
    }

    if (output_filename.empty()) {
        output_filename = filename.filename().replace_extension(
            filename.extension().string() + ".spv");
    }

    auto path = output_filename.parent_path();
    if (!path.empty() && !std::filesystem::exists(path)) {
        std::filesystem::create_directories(path);
    }

    std::ofstream out_file(output_filename, std::ios::binary);
    out_file.write(
        (const char*)spirv.m_data.data(),
        spirv.m_data.size() *
            sizeof(
                decltype(nickel::graphics::SpirVBinary::m_data)::value_type));

    nickel::graphics::ShaderCompiler::ShutdownCompilerSystem();

    return 0;
}