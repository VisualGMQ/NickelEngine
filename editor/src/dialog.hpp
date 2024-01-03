#pragma once

#include "nickel.hpp"
#include <vector>
#include <string>
#include <filesystem>

std::filesystem::path OpenDirDialog(const std::string& title);
std::vector<std::filesystem::path> OpenFileDialog(
    const std::string& title, const std::vector<std::string>& extensions);
std::filesystem::path SaveFileDialog(
    const std::string& title, const std::vector<std::string>& extensions);

enum class MessageBoxType {
    Error = SDL_MESSAGEBOX_ERROR,
    Warning = SDL_MESSAGEBOX_WARNING,
    Info = SDL_MESSAGEBOX_INFORMATION,
};

void ShowSimpleMessageBox(MessageBoxType type, const std::string& title,
                          const std::string& content);

class MessageBox final {
public:
    enum class ButtonType {
        None = 0,
        ReturnKeyDefault = SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT,
        EscapeKeyDefault = SDL_MESSAGEBOX_BUTTON_ESCAPEKEY_DEFAULT,
    };

    MessageBox(const std::string& title, const std::string& msg,
               MessageBoxType type)
        : title_(title), msg_(msg), type_(type) {}

    MessageBox& AddButton(const std::string& title,
                          ButtonType type = ButtonType::None) {
        buttonTexts_.push_back(title);
        SDL_MessageBoxButtonData btn;
        btn.flags = static_cast<SDL_MessageBoxButtonFlags>(type);
        btn.buttonid = buttonId_++;
        btn.text = buttonTexts_.back().c_str();
        buttons_.emplace_back(btn);
        return *this;
    }

    MessageBox& SetBackgroundColor(const nickel::cgmath::Color& color) {
        setSchemeColor(SDL_MESSAGEBOX_COLOR_BACKGROUND, color);
        return *this;
    }

    MessageBox& SetTextColor(const nickel::cgmath::Color& color) {
        setSchemeColor(SDL_MESSAGEBOX_COLOR_TEXT, color);
        return *this;
    }

    MessageBox& SetButtonBorderColor(const nickel::cgmath::Color& color) {
        setSchemeColor(SDL_MESSAGEBOX_COLOR_BUTTON_BORDER, color);
        return *this;
    }

    MessageBox& SetButtonBackgroundColor(const nickel::cgmath::Color& color) {
        setSchemeColor(SDL_MESSAGEBOX_COLOR_BUTTON_BACKGROUND, color);
        return *this;
    }

    MessageBox& SetButtonSelectedColor(const nickel::cgmath::Color& color) {
        setSchemeColor(SDL_MESSAGEBOX_COLOR_BUTTON_SELECTED, color);
        return *this;
    }

    int Show() const {
        int btnId;
        SDL_MessageBoxData data;
        data.title = title_.c_str();
        data.message = msg_.c_str();
        data.flags = static_cast<SDL_MessageBoxFlags>(type_);
        data.numbuttons = buttons_.size();
        data.buttons = buttons_.data();
        data.colorScheme = scheme_ ? &scheme_.value() : nullptr;
        SDL_ShowMessageBox(&data, &btnId);
        return btnId;
    }

private:
    int buttonId_ = 0;
    MessageBoxType type_;
    std::string title_;
    std::string msg_;
    std::vector<SDL_MessageBoxButtonData> buttons_;
    std::vector<std::string> buttonTexts_;
    std::optional<SDL_MessageBoxColorScheme> scheme_;

    void setSchemeColor(SDL_MessageBoxColorType type,
                        const nickel::cgmath::Color& c) {
        if (!scheme_) {
            scheme_ = SDL_MessageBoxColorScheme{};
        }
        scheme_->colors[type].r = c.r;
        scheme_->colors[type].g = c.g;
        scheme_->colors[type].b = c.b;
    }
};