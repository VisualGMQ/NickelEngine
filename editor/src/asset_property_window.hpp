#pragma once

#include "image_view_canva.hpp"
#include "imgui_plugin.hpp"
#include "nickel.hpp"
#include "widget.hpp"


class TexturePropertyPopupWindow : public PopupWindow {
public:
    TexturePropertyPopupWindow(const std::string& title) : PopupWindow(title) {}

    void ChangeTexture(nickel::TextureHandle handle) {
        handle_ = handle;
        imageViewer_.ChangeTexture(handle);
        auto assetMgr = gWorld->res<nickel::AssetManager>();
        if (assetMgr->Has(handle_)) {
            sampler_ = assetMgr->Get(handle_).Sampler();
        }
    }

protected:
    void update() override;

private:
    nickel::gogl::Sampler sampler_ =
        nickel::gogl::Sampler::CreateLinearRepeat();
    nickel::TextureHandle handle_;
    ImageViewCanva imageViewer_;

    void showWrapper(nickel::gogl::Sampler::Wrapper& wrapper,
                     gecs::registry reg);
    void showSampler(nickel::gogl::Sampler& sampler, gecs::registry reg);
};

class SoundPropertyPopupWindow : public PopupWindow {
public:
    SoundPropertyPopupWindow(const std::string& title) : PopupWindow(title) {}

    void ChangeAudio(nickel::SoundHandle handle) { handle_ = handle; }

protected:
    void update() override;

private:
    nickel::SoundHandle handle_;
};

class FontPropertyPopupWindow : public PopupWindow {
public:
    FontPropertyPopupWindow(const std::string& title) : PopupWindow(title) {}

    void ChangeFont(nickel::FontHandle handle) { handle_ = handle; }

protected:
    void update() override;

private:
    nickel::FontHandle handle_;
};
