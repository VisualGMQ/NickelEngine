#include "graphics/texture.hpp"
#include "graphics/context.hpp"
#include "lunasvg.h"
#include "stb_image.h"

namespace nickel {

Texture Texture::Null = Texture{};

Texture::Texture(rhi::Device device, const std::filesystem::path& filename,
                 rhi::TextureFormat gpuFmt, rhi::Flags<rhi::TextureUsage> usage)
    : Asset(filename) {
    stbi_uc* pixels = stbi_load((filename).string().c_str(), &w_, &h_, nullptr,
                                STBI_rgb_alpha);
    if (pixels) {
        texture_ = createTexture(device, pixels, w_, h_, gpuFmt, usage);
        if (!texture_) {
            LOGE(log_tag::RHI, "create texture from", filename, "failed");
        } else {
            view_ = texture_.CreateView();
        }
    } else {
        LOGE(log_tag::Asset, "load texture from ", filename, " failed");
    }

    stbi_image_free(pixels);
}

Texture::Texture(rhi::Device device, const toml::table& tbl) {
    std::filesystem::path filename;

    do {
        if (auto path = tbl.get("path"); !path || !path->is_string()) {
            LOGW(log_tag::Asset,
                 "deserialize texture failed! `path` field not string");
            break;
        } else {
            filename = path->as_string()->get();
        }

        auto newTexture = Texture{device, filename};
        newTexture.AssociateFile(filename);
        *this = std::move(newTexture);
    } while (0);
}

Texture::Texture(rhi::Device device, void* pixels, int w, int h,
                 rhi::TextureFormat gpuFmt,
                 rhi::Flags<rhi::TextureUsage> usage)
    : w_(w), h_(h) {
    texture_ =
        createTexture(device, pixels, w_, h_, gpuFmt, usage);
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

Texture::~Texture() {
    view_.Destroy();
    texture_.Destroy();
}

template <>
std::unique_ptr<Texture> LoadAssetFromMetaTable(const toml::table& tbl) {
    return std::make_unique<Texture>(
        ECS::Instance().World().res<rhi::Device>().get(), tbl);
}

toml::table Texture::Save2Toml() const {
    toml::table tbl;
    tbl.emplace("path", RelativePath().string());
    return tbl;
}

TextureHandle TextureManager::Load(const std::filesystem::path& filename,
                                   rhi::TextureFormat gpuFmt,
                                   rhi::Flags<rhi::TextureUsage> usage) {
    auto [handle, _] = LoadAndGet(filename, gpuFmt, usage);
    return handle;
}

std::tuple<TextureHandle, Texture&> TextureManager::LoadAndGet(
    const std::filesystem::path& filename, rhi::TextureFormat gpuFmt,
    rhi::Flags<rhi::TextureUsage> usage) {
    if (Has(filename)) {
        auto handle = GetHandle(filename);
        auto& texture = Get(handle);
        return {handle, texture};
    }

    auto texture = std::make_unique<Texture>(
        ECS::Instance().World().res<rhi::Device>().get(), filename, gpuFmt,
        usage);
    if (texture && *texture) {
        TextureHandle handle = TextureHandle::Create();
        return {handle, storeNewItem(handle, std::move(texture))};
    } else {
        return {TextureHandle::Null(), Texture::Null};
    }
}

TextureHandle TextureManager::Create(const std::filesystem::path& name,
                                     void* data, uint32_t w, uint32_t h,
                                     rhi::TextureFormat gpuFmt,
                                     rhi::Flags<rhi::TextureUsage> usage) {
    if (Has(name)) {
        return GetHandle(name);
    }

    auto texture = std::make_unique<Texture>(
        ECS::Instance().World().res<rhi::Device>().get(), data, w, h,
        gpuFmt, usage);
    texture->AssociateFile(name);
    if (texture && *texture) {
        TextureHandle handle = TextureHandle::Create();
        storeNewItem(handle, std::move(texture));
        return handle;
    } else {
        return TextureHandle::Null();
    }
}

bool TextureManager::Replace(TextureHandle handle,
                             const std::filesystem::path& filename,
                             rhi::TextureFormat gpuFmt,
                             rhi::Flags<rhi::TextureUsage> usage) {
    if (!Has(handle)) {
        return false;
    }

    Texture texture{ECS::Instance().World().res<rhi::Device>().get(), filename,
                    gpuFmt, usage};
    if (texture) {
        Get(handle) = std::move(texture);
        return true;
    } else {
        return false;
    }
}

std::unique_ptr<Texture> TextureManager::CreateSolitary(
    void* data, int w, int h, rhi::TextureFormat gpuFmt,
    rhi::Flags<rhi::TextureUsage> usage) {
    auto texture = std::unique_ptr<Texture>(
        new Texture{ECS::Instance().World().res<rhi::Device>().get(), data, w,
                    h, gpuFmt, usage});
    if (texture && *texture) {
        return std::move(texture);
    } else {
        return nullptr;
    }
}

TextureHandle TextureManager::LoadSVG(const std::filesystem::path& filename,
                                      std::optional<cgmath::Vec2> size) {
    if (Has(filename)) {
        return GetHandle(filename);
    }

    auto doc = lunasvg::Document::loadFromFile(filename.string());
    if (!doc) {
        LOGW(log_tag::Asset, "load svg file from ", filename, " failed");
        return TextureHandle::Null();
    }
    auto bitmap = doc->renderToBitmap(size ? size->w : 0, size ? size->h : 0);
    bitmap.convertToRGBA();

    if (!bitmap.valid()) {
        return TextureHandle::Null();
    }

    auto handle = TextureHandle::Create();

    auto texture = std::make_unique<Texture>(
        ECS::Instance().World().res<rhi::Device>().get(), (void*)bitmap.data(),
        bitmap.width(), bitmap.height());
    texture->AssociateFile(filename);
    if (texture) {
        storeNewItem(handle, std::move(texture));
        return handle;
    }

    return TextureHandle::Null();
}

}  // namespace nickel