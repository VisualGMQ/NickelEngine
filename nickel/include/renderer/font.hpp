#pragma once

#include "pch.hpp"
#include "core/handle.hpp"
#include "core/manager.hpp"
#include "renderer/texture.hpp"
#include "core/asset.hpp"

namespace nickel {

class Font;

using FontHandle = Handle<Font>;

class Font final : public Asset {
public:
    friend class FontManager;

    static Font Null;

    ~Font();

    Font(const Font&) = delete;
    Font& operator=(const Font&) = delete;

    FT_GlyphSlot GetGlyph(uint64_t c, int size) const;
    const FT_Face& GetFace() const { return face_; }

    explicit operator bool() const {
        return face_ != nullptr;
    }

private:
    FT_Face face_ = nullptr;
    std::filesystem::path filename_;

    Font(const std::filesystem::path& rootPath,
         const std::filesystem::path& filename);
    Font() = default;
};

class FontManager final : public Manager<Font> {
public:
    FontHandle Load(const std::filesystem::path& filename);

    toml::table Save2Toml() const override;
    void LoadFromToml(toml::table&) override;
};

struct Character {
    const cgmath::Vec2 size;
    const cgmath::Vec2 bearing;
    const cgmath::Vec2 advance;
    std::unique_ptr<Texture> texture;

    Character(const FT_GlyphSlot&);
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