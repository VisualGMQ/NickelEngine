#pragma once
#include "graphics/rhi/common.hpp"
#include "glad/glad.h"

namespace nickel::rhi::gl4 {

#define CASE(a, b) case a: return b;

inline GLenum ShaderStage2GL(ShaderStage stage) {
    switch (stage) {
        CASE(ShaderStage::Vertex, GL_VERTEX_SHADER);
        CASE(ShaderStage::Compute, GL_COMPUTE_SHADER);
        CASE(ShaderStage::Fragment, GL_FRAGMENT_SHADER);
    }
}

inline GLenum Filter2GL(Filter filter) {
    switch(filter) {
        CASE(Filter::Nearest, GL_NEAREST);
        CASE(Filter::Linear, GL_LINEAR);
    }
}

inline GLenum SamplerAddressMode2GL(SamplerAddressMode mode) {
    switch(mode) {
        CASE(SamplerAddressMode::ClampToEdge, GL_CLAMP_TO_EDGE);
        CASE(SamplerAddressMode::Repeat, GL_REPEAT);
        CASE(SamplerAddressMode::MirrorRepeat, GL_MIRRORED_REPEAT);
    }
}

inline GLenum CompareOp2GL(CompareOp op) {
    switch (op) {
        CASE(CompareOp::Never, GL_NEVER);
        CASE(CompareOp::Less, GL_LESS);
        CASE(CompareOp::Equal, GL_EQUAL);
        CASE(CompareOp::LessEqual, GL_LEQUAL);
        CASE(CompareOp::Greater, GL_GREATER);
        CASE(CompareOp::NotEqual, GL_NOTEQUAL);
        CASE(CompareOp::GreaterEqual, GL_GEQUAL);
        CASE(CompareOp::Always, GL_ALWAYS);
    }
}

#undef CASE

}  // namespace nickel::rhi::gl4