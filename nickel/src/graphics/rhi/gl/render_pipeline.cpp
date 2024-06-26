#include "graphics/rhi/gl/render_pipeline.hpp"
#include "graphics/rhi/gl/convert.hpp"
#include "graphics/rhi/gl/device.hpp"
#include "graphics/rhi/gl/glcall.hpp"
#include "graphics/rhi/gl/pipeline_layout.hpp"
#include "graphics/rhi/gl/shader.hpp"

namespace nickel::rhi::gl {

RenderPipelineImpl::RenderPipelineImpl(DeviceImpl& device,
                                       const RenderPipeline::Descriptor& desc)
    : dev_{device}, desc_{desc} {
    createShader(desc);
    GL_CALL(glGenVertexArrays(1, &vao_));
}

RenderPipelineImpl::~RenderPipelineImpl() {
    GL_CALL(glDeleteVertexArrays(1, &vao_));
    GL_CALL(glDeleteProgram(shaderId_));
}

void RenderPipelineImpl::createShader(const RenderPipeline::Descriptor& desc) {
    shaderId_ = glCreateProgram();

    rhi::ShaderModuleImpl* vertexModule = desc.vertex.module.Impl().get();
    GLuint vertexId = static_cast<ShaderModuleImpl*>(vertexModule)
                          ->CreateShader(GL_VERTEX_SHADER);
    GL_CALL(glAttachShader(shaderId_, vertexId));

    rhi::ShaderModuleImpl* fragModule = desc.fragment.module.Impl().get();
    GLuint fragId = static_cast<ShaderModuleImpl*>(fragModule)
                        ->CreateShader(GL_FRAGMENT_SHADER);
    GL_CALL(glAttachShader(shaderId_, fragId));

#ifdef NICKEL_HAS_GL
    GLuint geomId = 0;
    if (desc.geometry) {
        rhi::ShaderModuleImpl* geomModule = desc.geometry->module.Impl().get();
        GLuint geomId = static_cast<ShaderModuleImpl*>(geomModule)
                            ->CreateShader(GL_GEOMETRY_SHADER);
        GL_CALL(glAttachShader(shaderId_, geomId));
    }
#endif

    GL_CALL(glLinkProgram(shaderId_));

    int success;
    char infoLog[1024];
    GL_CALL(glGetProgramiv(shaderId_, GL_LINK_STATUS, &success));
    if (!success) {
        glGetProgramInfoLog(shaderId_, 1024, NULL, infoLog);
        LOGF(log_tag::GL, "shader link failed:\r\n", infoLog);
    }

    GL_CALL(glDeleteShader(vertexId));
    GL_CALL(glDeleteShader(fragId));

#ifdef NICKEL_HAS_GL
    if (desc.geometry) {
        GL_CALL(glDeleteShader(geomId));
    }
#endif
}

void RenderPipelineImpl::Apply() const {
    // shader apply
    GL_CALL(glUseProgram(shaderId_));
    GL_CALL(glBindVertexArray(vao_));

#ifdef NICKEL_HAS_GL
    GL_CALL(glPolygonMode(GL_FRONT_AND_BACK,
                          PolygonMode2GL(desc_.primitive.polygonMode)));
#endif

    // viewport apply
    // auto& viewport = desc_.viewport.viewport;
    // GL_CALL(glViewport(viewport.x, viewport.y, viewport.w, viewport.h));
    // GL_CALL(glEnable(GL_SCISSOR_TEST));
    // auto& scissor = desc_.viewport.scissor;
    // GL_CALL(glScissor(scissor.offset.x, scissor.offset.y, scissor.extent.width,
    //                   scissor.extent.height));

    // rasterization apply
    if (desc_.primitive.cullMode != CullMode::None) {
        GL_CALL(glEnable(GL_CULL_FACE));
        GL_CALL(glCullFace(CullMode2GL(desc_.primitive.cullMode)));
    } else {
        GL_CALL(glDisable(GL_CULL_FACE));
    }
    GL_CALL(glLineWidth(1.0));

    GL_CALL(glFrontFace(FrontFace2GL(desc_.primitive.frontFace)));

    // depth stencil apply
    // TODO: depth bound test(may use GL_EXT_depth_bounds_test)
    if (desc_.depthStencil) {
        GL_CALL(glEnable(GL_STENCIL_TEST));
        GL_CALL(glEnable(GL_DEPTH_TEST));

        if (desc_.depthStencil->depthWriteEnabled) {
            GL_CALL(glDepthMask(GL_TRUE));
        } else {
            GL_CALL(glDepthMask(GL_FALSE));
        }
        GL_CALL(glDepthFunc(CompareOp2GL(desc_.depthStencil->depthCompare)));

        GL_CALL(glStencilOpSeparate(
            GL_FRONT,
            StencilOpEnum2GL(desc_.depthStencil->stencilFront.failedOp),
            StencilOpEnum2GL(desc_.depthStencil->stencilFront.depthFailOp),
            StencilOpEnum2GL(desc_.depthStencil->stencilFront.passOp)));
        GL_CALL(glStencilOpSeparate(
            GL_BACK, StencilOpEnum2GL(desc_.depthStencil->stencilBack.failedOp),
            StencilOpEnum2GL(desc_.depthStencil->stencilBack.depthFailOp),
            StencilOpEnum2GL(desc_.depthStencil->stencilBack.passOp)));
        GL_CALL(glStencilFuncSeparate(
            GL_FRONT, CompareOp2GL(desc_.depthStencil->stencilFront.compare), 0,
            0xFFFFFFFF));
        GL_CALL(glStencilFuncSeparate(
            GL_BACK, CompareOp2GL(desc_.depthStencil->stencilBack.compare), 0,
            0xFFFFFFFF));
        GL_CALL(glStencilMaskSeparate(GL_FRONT,
                                      desc_.depthStencil->stencilWriteMask));
        GL_CALL(glStencilMaskSeparate(GL_BACK,
                                      desc_.depthStencil->stencilWriteMask));
    } else {
        GL_CALL(glDisable(GL_DEPTH_TEST));
        GL_CALL(glDisable(GL_STENCIL_TEST));
    }

    // multisample apply
    if (desc_.multisample.alphaToCoverageEnabled) {
        GL_CALL(glEnable(GL_SAMPLE_ALPHA_TO_COVERAGE));
    } else {
        GL_CALL(glDisable(GL_SAMPLE_ALPHA_TO_COVERAGE));
    }

    // color blend
    GL_CALL(glEnable(GL_BLEND));
    // GL_CALL(glEnable(GL_COLOR_LOGIC_OP));

#ifdef NICKEL_HAS_GL
    if (desc_.primitive.unclippedDepth) {
        GL_CALL(glEnable(GL_DEPTH_CLAMP));
    } else {
        GL_CALL(glDisable(GL_DEPTH_CLAMP));
    }

    for (int i = 0; i < desc_.fragment.targets.size(); i++) {
        auto& target = desc_.fragment.targets[i];
        auto& blend = target.blend;

        GL_CALL(glEnablei(GL_BLEND, i));
        GL_CALL(glBlendEquationSeparatei(i, BlendOp2GL(blend.color.operation),
                                         BlendOp2GL(blend.alpha.operation)));
        GL_CALL(glBlendFuncSeparatei(i, BlendFactor2GL(blend.color.srcFactor),
                                     BlendFactor2GL(blend.color.dstFactor),
                                     BlendFactor2GL(blend.alpha.srcFactor),
                                     BlendFactor2GL(blend.alpha.dstFactor)));

        GL_CALL(glColorMaski(i, target.writeMask & ColorWriteMask::Red,
                             target.writeMask & ColorWriteMask::Green,
                             target.writeMask & ColorWriteMask::Blue,
                             target.writeMask & ColorWriteMask::Alpha));
    }
#endif

    // push constant apply
    auto& ranges = static_cast<const PipelineLayoutImpl*>(GetLayout().Impl())
                       ->Descriptor()
                       .pushConstRanges;
    if (!ranges.empty()) {
        GL_CALL(glBindBufferBase(GL_UNIFORM_BUFFER,
                                 _NICKEL_PUSHCONSTANT_BIND_SLOT,
                                 dev_.pushConstantBuf));
    }
}

}  // namespace nickel::rhi::gl