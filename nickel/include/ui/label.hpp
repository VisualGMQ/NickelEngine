#pragma once

#include "core/utf8string.hpp"
#include "renderer/font.hpp"

namespace nickel::ui {

class Label final {
public:
    void SetText(const utf8string&);

    auto& GetText() const { return text_; }

    auto& GetTextCache() const { return cache_; }

    FontHandle GetFont() const { return font_; }

    void SetSize(int size);
    int GetSize() const { return size_; }

    cgmath::Color color = {1, 1, 1, 1};

    /*
     NOTE: I known "TextCache" will forbid compiler to create these functions.
     But std::is_copy_constructible<T> need them to make it become false
     more details:
     https://stackoverflow.com/questions/18404108/false-positive-with-is-copy-constructible-on-vectorunique-ptr
    */
    Label(const Label&) = delete;
    Label& operator=(const Label&) = delete;

    Label() = default;
    Label(FontHandle font): font_(font) {}

private:
    utf8string text_;
    TextCache cache_;
    FontHandle font_;
    int size_ = 16;

    void regenerateTextCache();
};

}  // namespace nickel::ui