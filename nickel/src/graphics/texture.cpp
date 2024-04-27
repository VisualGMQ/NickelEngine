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
                 rhi::TextureFormat gpuFmt, rhi::Flags<rhi::TextureUsage> usage)
    : w_(w), h_(h) {
    texture_ = createTexture(device, pixels, w_, h_, gpuFmt, usage);
    if (!texture_) {
        LOGE(log_tag::RHI, "create texture from pixels failed");
    } else {
        view_ = texture_.CreateView();
    }
}

rhi::Texture Texture::createTexture(rhi::Device dev, void* data, uint32_t w,
                                    uint32_t h, rhi::TextureFormat gpuFmt,
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
        bufferDesc.size = 4 * w * h;
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
        dst.aspect = rhi::TextureAspect::All;
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
        ECS::Instance().World().res<rhi::Device>().get(), data, w, h, gpuFmt,
        usage);
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