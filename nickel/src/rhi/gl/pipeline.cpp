#include "rhi/gl/pipeline.hpp"
#include "rhi/gl/enum_convert.hpp"
#include "rhi/gl/glcall.hpp"

namespace nickel::rhi::gl {

GraphicsPipeline::GraphicsPipeline(const VertexLayout& vertexLayout,
                                   const InputAssemblyState& assemblyState,
                                   const std::vector<ShaderModule*>& shaders,
                                   const ViewportState& viewportState,
                                   const RasterizationState& rasterState,
                                   const DepthStencilState& depthStencilState,
                                   const MultisampleState& multisampleState,
                                   const ColorBlendState& colorBlendState)
    : vertexLayout_{vertexLayout},
      assemblyState_{assemblyState},
      viewportState_(viewportState),
      rasterState_(rasterState),
      depthStencilState_(depthStencilState),
      multisampleState_(multisampleState),
      colorBlendState_(colorBlendState) {
    createShader(shaders);
}

void GraphicsPipeline::createShader(const std::vector<ShaderModule*>& modules) {
    shader_ = GL_RET_CALL(glCreateProgram());

    for (auto module : modules) {
        GL_CALL(glAttachShader(
            shader_, static_cast<const gl::ShaderModule*>(module)->Raw()));
    }
    GL_CALL(glLinkProgram(shader_));

    int success;
    char infoLog[1024];
    GL_CALL(glGetProgramiv(shader_, GL_LINK_STATUS, &success));
    if (!success) {
        glGetProgramInfoLog(shader_, 1024, NULL, infoLog);
        LOGF(log_tag::GL, "shader link failed:\r\n", infoLog);
    }
}

void GraphicsPipeline::Apply() {
    if (assemblyState_.primitiveRestartEnable) {
        GL_CALL(glEnable(GL_PRIMITIVE_RESTART_FIXED_INDEX));
    } else {
        GL_CALL(glDisable(GL_PRIMITIVE_RESTART_FIXED_INDEX));
    }

    // shader apply
    GL_CALL(glUseProgram(shader_));
    auto& viewport = viewportState_.viewports[0];

    // viewport apply
    GL_CALL(
        glViewport(viewport.x, viewport.y, viewport.width, viewport.height));
    GL_CALL(glDepthRange(viewport.minDepth, viewport.maxDepth));
    if (viewportState_.sissors.empty()) {
        GL_CALL(glEnable(GL_SCISSOR_TEST));
        auto& scissor = viewportState_.sissors[0];
        GL_CALL(glScissor(scissor.x, scissor.y, scissor.width, scissor.height));
    } else {
        GL_CALL(glDisable(GL_SCISSOR_TEST));
    }

    // rasterization apply
    if (rasterState_.cullMode != CullMode::None) {
        GL_CALL(glEnable(GL_CULL_FACE));
        GL_CALL(glCullFace(CullMode2GL(rasterState_.cullMode)));
    } else {
        GL_CALL(glDisable(GL_CULL_FACE));
    }
    GL_CALL(glLineWidth(rasterState_.lineWidth));

    GL_CALL(glFrontFace(FrontFace2GL(rasterState_.frontFace)));
    if (rasterState_.depthClampEnable) {
        GL_CALL(glEnable(GL_DEPTH_CLAMP));
    } else {
        GL_CALL(glDisable(GL_DEPTH_CLAMP));
    }

    // depth stencil apply
    // TODO: depth bound test(may use GL_EXT_depth_bounds_test)
    if (depthStencilState_.depthTestEnable) {
        GL_CALL(glEnable(GL_DEPTH_TEST));
        GL_CALL(glDepthFunc(CompareOp2GL(depthStencilState_.depthCompare)));
    } else {
        GL_CALL(glDisable(GL_DEPTH_TEST));
    }
    if (depthStencilState_.depthWriteEnable) {
        GL_CALL(glDepthMask(GL_FALSE));
    } else {
        GL_CALL(glDepthMask(GL_TRUE));
    }

    if (depthStencilState_.stencilTestEnable) {
        GL_CALL(glEnable(GL_STENCIL_TEST));
        GL_CALL(glStencilOpSeparate(
            GL_FRONT, StencilOpEnum2GL(depthStencilState_.front.onFail),
            StencilOpEnum2GL(depthStencilState_.front.onDepthFail),
            StencilOpEnum2GL(depthStencilState_.front.onDepthPass)));
        GL_CALL(glStencilOpSeparate(
            GL_BACK, StencilOpEnum2GL(depthStencilState_.back.onFail),
            StencilOpEnum2GL(depthStencilState_.back.onDepthFail),
            StencilOpEnum2GL(depthStencilState_.back.onDepthPass)));
        GL_CALL(glStencilFuncSeparate(
            GL_FRONT, CompareOp2GL(depthStencilState_.front.compare),
            depthStencilState_.front.reference,
            depthStencilState_.front.compareMask));
        GL_CALL(glStencilFuncSeparate(
            GL_BACK, CompareOp2GL(depthStencilState_.back.compare),
            depthStencilState_.back.reference,
            depthStencilState_.back.compareMask));
        GL_CALL(glStencilMaskSeparate(GL_FRONT,
                                      depthStencilState_.front.writeMask));
        GL_CALL(
            glStencilMaskSeparate(GL_BACK, depthStencilState_.back.writeMask));
    } else {
        GL_CALL(glDisable(GL_STENCIL_TEST));
    }

    // multisample apply
    if (multisampleState_.sampleShadingEnable) {
        GL_CALL(glEnable(GL_MULTISAMPLE));
    } else {
        GL_CALL(glDisable(GL_MULTISAMPLE));
    }
    if (multisampleState_.alphaToCoverageEnable) {
        GL_CALL(glEnable(GL_SAMPLE_ALPHA_TO_COVERAGE));
    } else {
        GL_CALL(glEnable(GL_SAMPLE_ALPHA_TO_COVERAGE));
    }
    if (multisampleState_.alphaToOneEnable) {
        GL_CALL(glEnable(GL_SAMPLE_ALPHA_TO_ONE));
    } else {
        GL_CALL(glDisable(GL_SAMPLE_ALPHA_TO_ONE));
    }
    // TODO: sample coverage
    // TODO: sample mask

    // color blend
    GL_CALL(glEnable(GL_BLEND));
    if (colorBlendState_.logicOpEnable) {
        GL_CALL(glEnable(GL_COLOR_LOGIC_OP));
    } else {
        GL_CALL(glDisable(GL_COLOR_LOGIC_OP));
    }

    for (int i = 0; i < colorBlendState_.attachmentStates.size(); i++) {
        auto& att = colorBlendState_.attachmentStates[i];
        if (att.blendEnable) {
            GL_CALL(glEnablei(GL_BLEND, i));
            GL_CALL(glBlendEquationSeparatei(i, BlendOp2GL(att.colorBlendOp),
                                             BlendOp2GL(att.alphaBlendOp)));
            GL_CALL(
                glBlendFuncSeparatei(i, BlendFactor2GL(att.srcColorBlendFactor),
                                     BlendFactor2GL(att.dstColorBlendFactor),
                                     BlendFactor2GL(att.srcAlphaBlendFactor),
                                     BlendFactor2GL(att.dstAlphaBlendFactor)));
        } else {
            GL_CALL(glDisablei(GL_BLEND, i));
        }
    }

    GL_CALL(glBlendColor(colorBlendState_.blendConstants[0],
                         colorBlendState_.blendConstants[1],
                         colorBlendState_.blendConstants[2],
                         colorBlendState_.blendConstants[3]));
}

}  // namespace nickel::rhi::gl