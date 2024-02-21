#pragma once
#include "graphics/rhi/gl4/glpch.hpp"
#include "graphics/rhi/gl4/convert.hpp"
#include "graphics/rhi/impl/sampler.hpp"
#include "graphics/rhi/sampler.hpp"


namespace nickel::rhi::gl4 {

class SamplerImpl : public rhi::SamplerImpl {
public:
    explicit SamplerImpl(const Sampler::Descriptor&);
    ~SamplerImpl();

    GLenum id;
};

}  // namespace nickel::rhi::gl4