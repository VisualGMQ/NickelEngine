#pragma once

#include "graphics/rhi/common.hpp"
#include <optional>

namespace nickel::rhi {

class DeviceImpl;
class TextureViewImpl;
class TextureImpl;

class TextureView {
public:
    struct Descriptor final {
        std::optional<TextureViewType> dimension;
        std::optional<uint32_t> arrayLayerCount;
        TextureAspect aspect = TextureAspect::All;
        uint32_t baseArrayLayer = 0;
        uint32_t baseMipLevel = 0;
        std::optional<TextureFormat> format;
        std::optional<uint32_t> mipLevelCount;
    };

    TextureView() = default;
    TextureView(APIPreference api, DeviceImpl&, TextureImpl&, const TextureView::Descriptor&);
    explicit TextureView(TextureViewImpl*);
    enum TextureFormat Format() const;
    class Texture Texture() const;

    bool operator==(const TextureView& o) const {
        return o.impl_ == impl_;
    }

    bool operator!=(const TextureView& o) const {
        return !(*this == o);
    }

    operator bool() const {
        return impl_;
    }

    void Destroy();

    auto Impl() const { return impl_; }

private:
    TextureViewImpl* impl_{};
};

}