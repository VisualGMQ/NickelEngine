#include "rhi/gl/image.hpp"
#include "rhi/gl/enum_convert.hpp"
#include "rhi/gl/glcall.hpp"

namespace nickel::rhi::gl {

Image::Image(enum ImageType type, const cgmath::Vec3& extent,
             enum Format format, ImageUsageFlags usage)
    : rhi::Image{type, extent, format, usage}, extent_{extent} {
    GL_CALL(glGenTextures(1, &id_));
}

Image::~Image() {
    GL_CALL(glDeleteTextures(1, &id_));
}

ImageView::ImageView(const Image& image, ImageViewType type, enum Format format)
    : rhi::ImageView{type, format},
      image_{&image},
      glType_{ImageViewType2GL(type)} {}

Sampler::Sampler(Filter min, Filter mag, SamplerAddressMode u,
                 SamplerAddressMode v, SamplerAddressMode w, float mipLodBias,
                 bool anisotropyEnable, float maxAnisotropy, bool compareEnable,
                 CompareOp compareOp, float minLod, float maxLod,
                 BorderColor borderColor, bool unormalizedCoordinates) {
    GL_CALL(glGenSamplers(1, &id_));
    GL_CALL(
        glSamplerParameteri(id_, GL_TEXTURE_WRAP_S, SamplerAddressMode2GL(u)));
    GL_CALL(
        glSamplerParameteri(id_, GL_TEXTURE_WRAP_R, SamplerAddressMode2GL(v)));
    GL_CALL(
        glSamplerParameteri(id_, GL_TEXTURE_WRAP_T, SamplerAddressMode2GL(w)));
    GL_CALL(glSamplerParameterf(id_, GL_TEXTURE_LOD_BIAS, mipLodBias));
    GL_CALL(glSamplerParameterf(id_, GL_TEXTURE_MIN_LOD, minLod));
    GL_CALL(glSamplerParameterf(id_, GL_TEXTURE_MAX_LOD, maxLod));
    GL_CALL(glSamplerParameteri(id_, GL_TEXTURE_MIN_FILTER, Filter2GL(min)));
    GL_CALL(glSamplerParameteri(id_, GL_TEXTURE_MAG_FILTER, Filter2GL(mag)));
    if (compareEnable) {
        GL_CALL(glSamplerParameteri(id_, GL_TEXTURE_COMPARE_MODE,
                                    GL_COMPARE_REF_TO_TEXTURE));
        GL_CALL(glSamplerParameteri(id_, GL_TEXTURE_COMPARE_FUNC,
                                    CompareOp2GL(compareOp)));
    } else {
        GL_CALL(glSamplerParameteri(id_, GL_TEXTURE_COMPARE_MODE, GL_NONE));
    }

    std::array<float, 4> borderColors{0, 0, 0, 1};
    if (borderColor == BorderColor::FloatOpaqueWhite ||
        borderColor == BorderColor::IntOpaqueWhite) {
        borderColors.fill(1);
    }
    GL_CALL(glSamplerParameterfv(id_, GL_TEXTURE_BORDER_COLOR,
                                 borderColors.data()));
    /* anisotropy must use EXT
    if (anisotropyEnable) {
        float aniso = 0.0;
        glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &aniso);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT,
    std::min(aniso, maxAnisotropy)); } else { glTexParameterf(GL_TEXTURE_2D,
    GL_TEXTURE_MAX_ANISOTROPY_EXT, 0);
    }
    */
}

Sampler::~Sampler() {
    GL_CALL(glDeleteSamplers(1, &id_));
}

void Sampler::Bind(GLuint textureIdx) {
    GL_CALL(glBindSampler(textureIdx, id_));
}

}  // namespace nickel::rhi::gl