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

    bool IsFocused() const { return focused_; }

    void Update() {
        if (!show_) {
            return;
        }

        if (ImGui::Begin(GetTitle().c_str(), &show_)) {
            focused_ = ImGui::IsWindowFocused();
            update();
        }
        ImGui::End();
    }

protected:
    bool show_ = true;

    virtual void update() = 0;

private:
    std::string title_;
    bool focused_ = false;
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

            if (ImGui::BeginPopupModal(GetTitle().c_str(), &show_)) {
                update();
                ImGui::EndPopup();
            }
        }
    }
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

        if (ImGui::BeginPopup(GetTitle().c_str())) {
            update();
            ImGui::EndPopup();
        }
        show_ = ImGui::IsPopupOpen(GetTitle().c_str());
    }

private:
    bool showed_ = false;
};