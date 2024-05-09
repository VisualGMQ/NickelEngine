#pragma once

#include "image_view_canva.hpp"
#include "imgui_plugin.hpp"
#include "nickel.hpp"
#include "widget.hpp"

class TexturePropertyWidget: public Widget {
public:
    TexturePropertyWidget() = default;

    void ChangeTexture(nickel::TextureHandle handle) {
        handle_ = handle;
        imageViewer_.ChangeTexture(handle);
        auto assetMgr = nickel::ECS::Instance().World().res<nickel::AssetManager>();
        if (assetMgr->Has(handle_)) {
            // TODO: not finish
        }
    }

    void Update() override;

private:
    nickel::TextureHandle handle_;
    ImageViewCanva imageViewer_;
};

class SoundPropertyWidget : public Widget {
public:
    SoundPropertyWidget() = default;

    void ChangeAudio(nickel::SoundHandle handle) {
        handle_ = handle;
        if (handle_) {
            player_.ChangeSound(handle);
            player_.Stop();
            player_.Rewind();
        }
    }

    void Update() override;

private:
    nickel::SoundHandle handle_;
    nickel::SoundPlayer player_;
};

class FontPropertyWidget: public Widget {
public:
    FontPropertyWidget() = default;

    void ChangeFont(nickel::FontHandle handle) { handle_ = handle; }

    void Update() override;

private:
    nickel::FontHandle handle_;
};

class Material2DPropertyWidget: public Widget {
public:
    Material2DPropertyWidget() = default;

    void ChangeMaterial(nickel::Material2DHandle handle) { handle_ = handle; }

    void Update() override;

private:
    nickel::Material2DHandle handle_;
};
