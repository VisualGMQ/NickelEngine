#include "SPIRV/GlslangToSpv.h"
#include "nickel/common/assert.hpp"
#include "nickel/graphics/lowlevel/internal/shader_compiler_impl.hpp"

namespace nickel::graphics {

SpirVBinary ShaderCompilerImpl::Compile(std::span<const char> code,
                                        ShaderType stage) {
    glslang::TShader shader{getLangFromStage(stage)};
    const char* c = code.data();
    int size = code.size();
    shader.setStringsWithLengths(&c, &size, 1);
    shader.setDebugInfo(true);
    shader.setEnvClient(glslang::EShClient::EShClientVulkan,
                        glslang::EShTargetClientVersion::EShTargetVulkan_1_4);
    shader.setEnvTarget(glslang::EShTargetLanguage::EShTargetSpv,
                        glslang::EShTargetLanguageVersion::EShTargetSpv_1_6);
    TBuiltInResource resource;
    // TODO: fill resource
    bool success = shader.parse(&resource, 4, ECoreProfile, false, false,
                                EShMsgVulkanRules);
    if (!success) {
        LOGE("compile shader failed:\n\t info log:\n {}\n\t debug log:\n {}",
             shader.getInfoLog(), shader.getInfoDebugLog());
        return {};
    }

    std::vector<unsigned int> spriv;
    glslang::GlslangToSpv(*shader.getIntermediate(), spriv);

    return {spriv};
}

EShLanguage ShaderCompilerImpl::getLangFromStage(ShaderType stage) const {
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

}  // namespace nickel::graphics