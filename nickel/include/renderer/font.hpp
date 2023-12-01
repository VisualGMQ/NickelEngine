#pragma once

#include "pch.hpp"
#include "core/handle.hpp"
#include "core/log_tag.hpp"
#include "core/manager.hpp"
#include "core/singlton.hpp"
#include "renderer/texture.hpp"

namespace nickel {

class Font;

using FontHandle = Handle<Font>;

class Font final {
public:
    friend class FontManager;

    static Font Null;

    ~Font();

    Font(const Font&) = delete;
    Font& operator=(const Font&) = delete;

    FT_GlyphSlot GetGlyph(uint64_t c, int size) const;

private:
    FT_Face face_ = nullptr;
    std::string filename_;

    Font(const std::string& filename);
    Font() = default;
};

class FontManager final : public Manager<Font> {
public:
    FontHandle Load(const std::string& filename);
};

struct Character {
    cgmath::Vec2 size;
    cgmath::Vec2 breaing;
    float advance;
    std::unique_ptr<Texture> texture;

    explicit Character(const FT_GlyphSlot&);
};

class TextCache final {
public:
    void Push(Character&& c) { texts_.emplace_back(std::move(c)); }
    auto& Texts() const { return texts_; }
    void Clear() { texts_.clear(); }

private:
    std::vector<Character> texts_;
};

void FontSystemInit();
void FontSystemShutdown();

}  // namespace nickel