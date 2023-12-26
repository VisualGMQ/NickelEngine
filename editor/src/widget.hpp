#pragma once

#include "imgui_plugin.hpp"
#include "nickel.hpp"

class Widget {
public:
    virtual void Update() = 0;
    virtual ~Widget() = default;
};

class Window : public Widget {
public:
    void Show() { show_ = true; }

    void Hide() { show_ = false; }

    bool IsVisible() const { return show_; }

    void SetVisible(bool visible) { show_ = visible; }

    void SetTitle(const std::string& title) { title_ = title; }

    auto& GetTitle() const { return title_; }

protected:
    bool show_ = true;

private:
    std::string title_;
};

class PopupWindow : public Window {
public:
    PopupWindow(const std::string& title) {
        SetTitle(title);
        Hide();
    }

    virtual ~PopupWindow() = default;

    void Update() override final {
        if (IsVisible()) {
            ImGui::OpenPopup(GetTitle().c_str());
            update();
        }
    }

protected:
    virtual void update() = 0;
};