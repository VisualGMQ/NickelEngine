#pragma once

#include "common/utf8string.hpp"
#include "graphics/font.hpp"

namespace nickel::ui {

class Label final {
public:
    void SetText(const utf8string&);

    auto& GetText() const { return text_; }

    auto& GetTextCache() const { return cache_; }

    FontHandle GetFont() const { return font_; }
    void ChangeFont(FontHandle);

    void SetPtSize(int size);
    int GetPtSize() const { return size_; }
    auto& GetBoundingBoxSize() const { return textSize_; }

    auto& GetRenderRects() const { return rects_; }

    cgmath::Color color = {1, 1, 1, 1};
    cgmath::Color pressColor = {1, 1, 1, 1};
    cgmath::Color hoverColor = {1, 1, 1, 1};

    /*
     NOTE: I known "TextCache" will forbid compiler to create these functions.
     But std::is_copy_constructible<T> need them to make it become false
     more details:
     https://stackoverflow.com/questions/18404108/false-positive-with-is-copy-constructible-on-vectorunique-ptr
    */
    Label(const Label&) = delete;
    Label& operator=(const Label&) = delete;
    Label(Label&&) = default;
    Label& operator=(Label&&) = default;

    Label() = default;
    Label(FontHandle font): font_(font) {}

private:
    utf8string text_;
    TextCache cache_;
    FontHandle font_;
    int size_ = 16;
    cgmath::Vec2 textSize_;

    std::vector<cgmath::Rect> rects_;   // cached char rects for rendering

    void regenerateTextCache();
};

}  // namespace nickel::ui