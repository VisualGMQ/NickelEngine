#pragma once
#include "graphics/rhi/gl/glpch.hpp"
#include "graphics/rhi/gl/convert.hpp"
#include "graphics/rhi/impl/sampler.hpp"
#include "graphics/rhi/sampler.hpp"


namespace nickel::rhi::gl {

class SamplerImpl : public rhi::SamplerImpl {
public:
    explicit SamplerImpl(const Sampler::Descriptor&);
    ~SamplerImpl();

    GLenum id;
};

}  // namespace nickel::rhi::gl