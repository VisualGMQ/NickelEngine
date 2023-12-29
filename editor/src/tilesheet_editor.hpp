#pragma once

#include "image_view_canva.hpp"
#include "nickel.hpp"
#include "widget.hpp"

class TilesheetEditor;

class TilesheetViewCanva : public ImageViewCanva {
public:
    using SelectCallbackFn = std::function<void(const nickel::Tile&)>;

    explicit TilesheetViewCanva(TilesheetEditor* owner) : ImageViewCanva{ImGuiMouseButton_Middle}, owner_(owner) {}

    void ChangeTilesheet(nickel::Tilesheet& tilesheet) {
        tilesheet_ = &tilesheet;
        ChangeTexture(tilesheet.Handle());
        Resize(gWorld->res<nickel::AssetManager>()
                   ->Get(tilesheet.Handle())
                   .Size());
    }

    void SetSelectCallback(SelectCallbackFn fn) { fn_ = fn; }

protected:
    void additionalDraw(ImDrawList* drawList, const nickel::Texture& texture,
                        const nickel::cgmath::Vec2&) override;

private:
    nickel::Tilesheet* tilesheet_ = nullptr;
    SelectCallbackFn fn_;
    TilesheetEditor* owner_;
};

class TilesheetEditor : public PopupWindow {
public:
    using SelectCallbackFn = typename TilesheetViewCanva::SelectCallbackFn;

    TilesheetEditor(const std::string& title)
        : PopupWindow(title), viewCanva_(this) {}

    void ChangeTilesheet(nickel::TilesheetHandle handle) {
        handle_ = handle;
        auto assetMgr = gWorld->res_mut<nickel::AssetManager>();
        auto& tilesheet = assetMgr->Get(handle);
        viewCanva_.ChangeTilesheet(tilesheet);
    }

    void SetSelectCallback(SelectCallbackFn fn) {
        viewCanva_.SetSelectCallback(fn);
    }

protected:
    static constexpr float scaleIncStep = 0.1;
    static constexpr float minScaleFactor = 0.0001;

    nickel::TilesheetHandle handle_;
    nickel::cgmath::Vec2 offset_;
    float scale_ = 1.0;
    TilesheetViewCanva viewCanva_;

    void update() override;
};