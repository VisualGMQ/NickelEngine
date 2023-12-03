#include "ui/label.hpp"
#include "renderer/font.hpp"

namespace nickel::ui {

void Label::SetText(const utf8string& t) {
    text_ = t;
    regenerateTextCache();
}

void Label::SetPtSize(int size) {
    size_ = size;
    regenerateTextCache();
}

void Label::regenerateTextCache() {
    auto& fontMgr = gWorld->cur_registry()->res<gecs::mut<FontManager>>().get();
    if (!fontMgr.Has(font_)) {
        return;
    }

    auto& font = fontMgr.Get(font_);
    textSize_.Set(0, 0);
    cache_.Clear();
    rects_.clear();

    cgmath::Vec2 baseline;

    for (auto& c : text_) {
        auto glyph = font.GetGlyph(c.to_uint64(), size_);
        auto ch = Character{glyph};

        cgmath::Rect rect;

        if (c.is_white_space()) {
            auto value = c.to_uint64();
            if (value == ' ') {
                baseline.x += size_;
            } else if (value == '\t') {
                baseline.x += size_ * 4;
            } else if (value == '\n') {
                baseline.x = 0;
                baseline.y += size_;
            }
            rect.position.x = baseline.x;
            rect.position.y = baseline.y + size_;
        } else {
            rect.position.x = baseline.x + ch.bearing.x;
            rect.position.y = baseline.y - ch.bearing.y + size_;
            baseline.x += ch.size.w + ch.bearing.x;
        }

        rect.size = ch.size;

        textSize_.w = std::max(textSize_.w, rect.position.x + rect.size.w);
        textSize_.h = std::max(textSize_.h, rect.position.y + rect.size.h);

        cache_.Push(std::move(ch));
        rects_.push_back(rect);
    }
}

}  // namespace nickel::ui