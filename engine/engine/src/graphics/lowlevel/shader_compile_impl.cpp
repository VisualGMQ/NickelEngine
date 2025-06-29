#include "SPIRV/GlslangToSpv.h"
#include "glslang/Include/glslang_c_interface.h"
#include "glslang/Public/ResourceLimits.h"
#include "glslang/Public/ShaderLang.h"
#include "nickel/common/assert.hpp"
#include "nickel/common/flags.hpp"
#include "nickel/graphics/lowlevel/internal/shader_compiler_impl.hpp"

namespace nickel::graphics {

EShLanguage CvtStage(ShaderType stage) {
    switch (stage) {
        case ShaderType::Vertex:
            return EShLangVertex;
        case ShaderType::Fragment:
            return EShLangFragment;
        case ShaderType::Geometry:
            return EShLangGeometry;
        case ShaderType::TesselationController:
            return EShLangTessControl;
        case ShaderType::TesselationEvaluation:
            return EShLangTessEvaluation;
        case ShaderType::Compute:
            return EShLangCompute;
    }

    NICKEL_CANT_REACH();
    return {};
}

SpirVBinary ShaderCompilerImpl::Compile(std::span<const char> code,
                                        ShaderType stage) {
    auto language = CvtStage(stage);
    const char* c = code.data();
    int size = code.size();

    // Compile
    glslang::TShader shader{language};
    shader.setStringsWithLengths(&c, &size, 1);
    shader.setDebugInfo(true);
    shader.setEnvClient(glslang::EShClient::EShClientVulkan,
                        glslang::EShTargetClientVersion::EShTargetVulkan_1_3);
    shader.setEnvTarget(glslang::EShTargetLanguage::EShTargetSpv,
                        glslang::EShTargetLanguageVersion::EShTargetSpv_1_6);
    Flags messages = Flags{EShMsgSpvRules}|EShMsgVulkanRules;
    const TBuiltInResource* resource = GetDefaultResources();
    bool success =
        shader.parse(resource, 450, ECoreProfile, false, false, messages);
    if (!success) {
        LOGE("compile shader failed:\n\t info log:\n {}\n\t debug log:\n {}",
             shader.getInfoLog(), shader.getInfoDebugLog());
        return {};
    }

    // Link
    glslang::TProgram program;
    program.addShader(&shader);
    program.buildReflection(EShReflectionDefault);
    if (!program.link(messages)) {
        LOGE("link shader failed:\n\t info log:\n {}\n\t debug log:\n {}",
             program.getInfoLog(), program.getInfoDebugLog());
    }
    glslang::SpvOptions options{};
#ifdef NICKEL_DEBUG
    options.generateDebugInfo = true;
    options.stripDebugInfo = false;
    options.disableOptimizer = false;
    options.validate = true;
    options.optimizeSize = true;
#else
    options.generateDebugInfo = false;
    options.stripDebugInfo = true;
    options.disableOptimizer = false;
#endif
    std::vector<unsigned int> spirv;
    glslang::GlslangToSpv(*program.getIntermediate(language), spirv, &options);

    return {spirv};
}

}  // namespace nickel::graphics