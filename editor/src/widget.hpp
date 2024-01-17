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
    virtual void Show() { show_ = true; }

    virtual void Hide() { show_ = false; }

    bool IsVisible() const { return show_; }

    virtual void SetVisible(bool visible) { show_ = visible; }

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

class PopupMenu: public Window {
public:
    PopupMenu(const std::string& title) {
        SetTitle(title);
        show_ = false;
    }

    void Show() override {
        Window::Show();
        showed_ = true;
    }

    void Hide() override {
        Window::Hide();
        ImGui::CloseCurrentPopup();
    }
    void SetVisible(bool v) override {
        Window::SetVisible(v);
        if (!v) {
            ImGui::CloseCurrentPopup();
        }
    }

    virtual ~PopupMenu() = default;

    void Update() override final {
        if (showed_) {
            ImGui::OpenPopup(GetTitle().c_str());
            showed_ = false;
        }
        update();
        show_ = ImGui::IsPopupOpen(GetTitle().c_str());
    }

protected:
    virtual void update() = 0;

    bool showed_ = false;
};