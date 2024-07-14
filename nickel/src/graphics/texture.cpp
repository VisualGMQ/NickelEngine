#include "graphics/texture.hpp"
#include "graphics/context.hpp"
#include "stb_image.h"

namespace nickel {

Texture Texture::Null = Texture{};

Texture::Texture(void* pixels, int w, int h,
                 rhi::TextureFormat gpuFmt,
                 rhi::Flags<rhi::TextureUsage> usage)
    : w_(w), h_(h) {
    texture_ = createTexture(RenderContext::Instance().device, pixels, w_, h_,
                             gpuFmt, usage);
    if (!texture_) {
        LOGE(log_tag::RHI, "create texture from pixels failed");
    } else {
        view_ = texture_.CreateView();
    }
}

uint8_t GetTextureFormatSize(rhi::TextureFormat fmt) {
    switch (fmt) {
        case rhi::TextureFormat::Undefined:
        case rhi::TextureFormat::Presentation:
            Assert(false, "invalid texture format");
            return 0;
        case rhi::TextureFormat::R8_UNORM:
        case rhi::TextureFormat::R8_SNORM:
        case rhi::TextureFormat::R8_UINT:
        case rhi::TextureFormat::R8_SINT:
            return 1;
        case rhi::TextureFormat::R16_UINT:
        case rhi::TextureFormat::R16_SINT:
        case rhi::TextureFormat::R16_FLOAT:
        case rhi::TextureFormat::RG8_UNORM:
        case rhi::TextureFormat::RG8_SNORM:
        case rhi::TextureFormat::RG8_UINT:
        case rhi::TextureFormat::RG8_SINT:
            return 2;
        case rhi::TextureFormat::R32_UINT:
        case rhi::TextureFormat::R32_SINT:
        case rhi::TextureFormat::R32_FLOAT:
        case rhi::TextureFormat::RG16_UINT:
        case rhi::TextureFormat::RG16_SINT:
        case rhi::TextureFormat::RG16_FLOAT:
        case rhi::TextureFormat::RGBA8_UNORM:
        case rhi::TextureFormat::RGBA8_UNORM_SRGB:
        case rhi::TextureFormat::RGBA8_SNORM:
        case rhi::TextureFormat::RGBA8_UINT:
        case rhi::TextureFormat::RGBA8_SINT:
        case rhi::TextureFormat::BGRA8_UNORM:
        case rhi::TextureFormat::BGRA8_UNORM_SRGB:
        case rhi::TextureFormat::RGB9E5_UFLOAT:
        case rhi::TextureFormat::RGB10A2_UINT:
        case rhi::TextureFormat::RGB10A2_UNORM:
        case rhi::TextureFormat::RG11B10_UFLOAT:
            return 4;
        case rhi::TextureFormat::RG32_UINT:
        case rhi::TextureFormat::RG32_SINT:
        case rhi::TextureFormat::RG32_FLOAT:
        case rhi::TextureFormat::RGBA16_UINT:
        case rhi::TextureFormat::RGBA16_SINT:
        case rhi::TextureFormat::RGBA16_FLOAT:
            return 8;
        case rhi::TextureFormat::RGBA32_UINT:
        case rhi::TextureFormat::RGBA32_SINT:
        case rhi::TextureFormat::RGBA32_FLOAT:
            return 16;
        case rhi::TextureFormat::STENCIL8:
            return 1;
        case rhi::TextureFormat::DEPTH16_UNORM:
            return 2;
        case rhi::TextureFormat::DEPTH24_PLUS:
        case rhi::TextureFormat::DEPTH24_PLUS_STENCIL8:
        case rhi::TextureFormat::DEPTH32_FLOAT:
            return 4;
        case rhi::TextureFormat::DEPTH32_FLOAT_STENCIL8:
            return 5;
        case rhi::TextureFormat::BC1_RGBA_UNORM:
        case rhi::TextureFormat::BC1_RGBA_UNORM_SRGB:
        case rhi::TextureFormat::BC2_RGBA_UNORM:
        case rhi::TextureFormat::BC2_RGBA_UNORM_SRGB:
        case rhi::TextureFormat::BC3_RGBA_UNORM:
        case rhi::TextureFormat::BC3_RGBA_UNORM_SRGB:
        case rhi::TextureFormat::BC4_R_UNORM:
        case rhi::TextureFormat::BC4_R_SNORM:
        case rhi::TextureFormat::BC5_RG_UNORM:
        case rhi::TextureFormat::BC5_RG_SNORM:
        case rhi::TextureFormat::BC6H_RGB_UFLOAT:
        case rhi::TextureFormat::BC6H_RGB_FLOAT:
        case rhi::TextureFormat::BC7_RGBA_UNORM:
        case rhi::TextureFormat::BC7_RGBA_UNORM_SRGB:
        case rhi::TextureFormat::ETC2_RGB8_UNORM:
        case rhi::TextureFormat::ETC2_RGB8_UNORM_SRGB:
        case rhi::TextureFormat::ETC2_RGB8A1_UNORM:
        case rhi::TextureFormat::ETC2_RGB8A1_UNORM_SRGB:
        case rhi::TextureFormat::ETC2_RGBA8_UNORM:
        case rhi::TextureFormat::ETC2_RGBA8_UNORM_SRGB:
        case rhi::TextureFormat::EAC_R11_UNORM:
        case rhi::TextureFormat::EAC_R11_SNORM:
        case rhi::TextureFormat::EAC_RG11_UNORM:
        case rhi::TextureFormat::EAC_RG11_SNORM:
        case rhi::TextureFormat::ASTC_4X4_UNORM:
        case rhi::TextureFormat::ASTC_4X4_UNORM_SRGB:
        case rhi::TextureFormat::ASTC_5X4_UNORM:
        case rhi::TextureFormat::ASTC_5X4_UNORM_SRGB:
        case rhi::TextureFormat::ASTC_5X5_UNORM:
        case rhi::TextureFormat::ASTC_5X5_UNORM_SRGB:
        case rhi::TextureFormat::ASTC_6X5_UNORM:
        case rhi::TextureFormat::ASTC_6X5_UNORM_SRGB:
        case rhi::TextureFormat::ASTC_6X6_UNORM:
        case rhi::TextureFormat::ASTC_6X6_UNORM_SRGB:
        case rhi::TextureFormat::ASTC_8X5_UNORM:
        case rhi::TextureFormat::ASTC_8X5_UNORM_SRGB:
        case rhi::TextureFormat::ASTC_8X6_UNORM:
        case rhi::TextureFormat::ASTC_8X6_UNORM_SRGB:
        case rhi::TextureFormat::ASTC_8X8_UNORM:
        case rhi::TextureFormat::ASTC_8X8_UNORM_SRGB:
        case rhi::TextureFormat::ASTC_10X5_UNORM:
        case rhi::TextureFormat::ASTC_10X5_UNORM_SRGB:
        case rhi::TextureFormat::ASTC_10X6_UNORM:
        case rhi::TextureFormat::ASTC_10X6_UNORM_SRGB:
        case rhi::TextureFormat::ASTC_10X8_UNORM:
        case rhi::TextureFormat::ASTC_10X8_UNORM_SRGB:
        case rhi::TextureFormat::ASTC_10X10_UNORM:
        case rhi::TextureFormat::ASTC_10X10_UNORM_SRGB:
        case rhi::TextureFormat::ASTC_12X10_UNORM:
        case rhi::TextureFormat::ASTC_12X10_UNORM_SRGB:
        case rhi::TextureFormat::ASTC_12X12_UNORM:
        case rhi::TextureFormat::ASTC_12X12_UNORM_SRGB:
            Assert(false, "not implement");
            return 0;
    }
}

rhi::Texture Texture::createTexture(rhi::Device dev, void* data, uint32_t w,
                                    uint32_t h,
                                    rhi::TextureFormat gpuFmt,
                                    rhi::Flags<rhi::TextureUsage> usage) {
    rhi::Texture::Descriptor desc;
    desc.format = gpuFmt;
    desc.size.width = w;
    desc.size.height = h;
    desc.size.depthOrArrayLayers = 1;
    desc.usage = usage;
    auto texture = dev.CreateTexture(desc);

    if (data) {
        rhi::Buffer::Descriptor bufferDesc;
        bufferDesc.mappedAtCreation = true;
        bufferDesc.usage = rhi::BufferUsage::CopySrc;
        bufferDesc.size = GetTextureFormatSize(gpuFmt) * w * h;
        rhi::Buffer copyBuffer = dev.CreateBuffer(bufferDesc);

        void* bufData = copyBuffer.GetMappedRange();
        memcpy(bufData, data, bufferDesc.size);
        copyBuffer.Unmap();

        auto encoder = dev.CreateCommandEncoder();
        rhi::CommandEncoder::BufTexCopySrc src;
        src.buffer = copyBuffer;
        src.offset = 0;
        src.rowLength = w;
        src.rowsPerImage = h;
        rhi::CommandEncoder::BufTexCopyDst dst;
        dst.texture = texture;
        dst.aspect = rhi::TextureAspect::ColorOnly;
        dst.miplevel = 0;
        encoder.CopyBufferToTexture(src, dst,
                                    rhi::Extent3D{(uint32_t)w, (uint32_t)h, 1});
        auto buf = encoder.Finish();
        dev.GetQueue().Submit({buf});
        dev.WaitIdle();
        encoder.Destroy();

        copyBuffer.Destroy();
    }

    return texture;
}

bool Texture::Load(const std::filesystem::path& relativePath) {
    stbi_uc* pixels = stbi_load((relativePath).string().c_str(), &w_, &h_, nullptr,
                                STBI_rgb_alpha);
    bool success = false;
    if (pixels) {
        auto newTexture = createTexture(RenderContext::Instance().device, pixels, w_,
                                 h_, rhi::TextureFormat::RGBA8_UNORM,
                                 rhi::Flags(rhi::TextureUsage::TextureBinding) |
                                     rhi::TextureUsage::CopyDst);
        if (!newTexture) {
            LOGE(log_tag::RHI, "create texture from", relativePath, "failed");
            success = false;
        } else {
            auto newView = newTexture.CreateView();
            if (newView) {
                view_.Destroy();
                texture_.Destroy();
                view_ = newView;
                texture_ = newTexture;
                success = true;
            } else {
                success = false;
            }
        }
    } else {
        LOGE(log_tag::Asset, "load texture from ", relativePath, " failed");
        success = false;
    }

    stbi_image_free(pixels);

    ChangeRelativePath(relativePath);
    return success;
}

bool Texture::Load(const toml::table& tbl) {
    if (auto path = tbl.get("path"); path && path->is_string()) {
        auto& filename = path->as_string()->get();
        ChangeRelativePath(filename);

        Texture newTexture;
        if (newTexture.Load(filename)) {
            *this = std::move(newTexture);
            return true;
        }
        return false;
    } else {
        LOGW(log_tag::Asset,
             "deserialize texture failed! `path` field not string");
        return false;
    }
    return true;
}

bool Texture::Save(toml::table& tbl) const {
    NICKEL_TOML_EMPLACE_NODE(tbl, "path", GetRelativePath().string());
    return true;
}

Texture::~Texture() {
    view_.Destroy();
    texture_.Destroy();
}

}  // namespace nickel