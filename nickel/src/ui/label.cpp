#include "ui/label.hpp"
#include "renderer/font.hpp"

namespace nickel::ui {

void Label::SetText(const utf8string& t) {
    text_ = t;
    regenerateTextCache();
}

void Label::SetSize(int size) {
    size_ = size;
    regenerateTextCache();
}

void Label::regenerateTextCache() {
    auto& fontMgr = gWorld->cur_registry()->res<gecs::mut<FontManager>>().get();
    if (!fontMgr.Has(font_)) {
        return;
    }

    auto& font = fontMgr.Get(font_);
    cache_.Clear();
    for (auto& c : text_) {
        auto glyph = font.GetGlyph(c.to_uint64(), size_);
        cache_.Push(Character{glyph});
    }
}

}  // namespace nickel::ui