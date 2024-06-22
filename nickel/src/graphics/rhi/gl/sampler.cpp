#include "graphics/rhi/gl/sampler.hpp"
#include "graphics/rhi/gl/glcall.hpp"

namespace nickel::rhi::gl {

SamplerImpl::SamplerImpl(const Sampler::Descriptor& desc) {
    GL_CALL(glGenSamplers(1, &id));
    GL_CALL(glSamplerParameteri(id, GL_TEXTURE_WRAP_S, SamplerAddressMode2GL(desc.u)));
    GL_CALL(glSamplerParameteri(id, GL_TEXTURE_WRAP_T, SamplerAddressMode2GL(desc.v)));
    GL_CALL(glSamplerParameteri(id, GL_TEXTURE_WRAP_R, SamplerAddressMode2GL(desc.w)));
    GL_CALL(glSamplerParameteri(id, GL_TEXTURE_MIN_FILTER, Filter2GL(desc.min)));
    GL_CALL(glSamplerParameteri(id, GL_TEXTURE_MAG_FILTER, Filter2GL(desc.mag)));
    GL_CALL(glSamplerParameteri(id, GL_TEXTURE_MIN_LOD, desc.lodMinClamp));
    GL_CALL(glSamplerParameteri(id, GL_TEXTURE_MAX_LOD, desc.lodMinClamp));
    if (desc.compare) {
        GL_CALL(glSamplerParameteri(id, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE));
        GL_CALL(glSamplerParameteri(id, GL_TEXTURE_COMPARE_FUNC, CompareOp2GL(desc.compare.value())));
    }
}

SamplerImpl::~SamplerImpl() {
    GL_CALL(glDeleteSamplers(1, &id));
}


}