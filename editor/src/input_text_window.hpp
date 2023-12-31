#pragma once

#include "widget.hpp"

class InputTextWindow: public PopupWindow {
public:
    using CallbackFn = std::function<void(const std::string&)>;

    InputTextWindow(const std::string& title);
    void SetText(const std::string& text) { text_ = text; }
    void SetCallback(CallbackFn fn) {
        callback_ = fn;
    }

protected:
    virtual void update() override;

private:
    static constexpr size_t MaxTextLen = 512;

    std::string text_;
    CallbackFn callback_;
};