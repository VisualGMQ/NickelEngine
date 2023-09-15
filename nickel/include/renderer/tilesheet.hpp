#pragma once

#include "pch.hpp"
#include "renderer/texture.hpp"
#include "renderer/sprite.hpp"

namespace nickel {

struct Margin final {
    static Margin Zero() { return {0, 0, 0, 0}; };

    uint32_t left, right, top, bottom;
};

struct Spacing final {
    uint32_t x, y;
};

struct Tile final {
    cgmath::Rect region;
    TextureHandle handle;
};

class Tilesheet final {
public:
    Tilesheet(TextureManager&, TextureHandle, uint32_t col, uint32_t row,
              const Margin& margin = Margin::Zero(),
              const Spacing& spacing = {0, 0});

    static Tilesheet Null;

    operator bool() const {
        return handle_ && tileWidth_ > 0 && tileHeight_ > 0;
    }

    Tile Get(uint32_t x, uint32_t y);
    Tile Get(uint32_t index);

    uint32_t Row() const { return row_; }

    uint32_t Col() const { return col_; }

    TextureHandle Handle() const { return handle_; }

    cgmath::Vec2 TileSize() const {
        return {static_cast<float>(tileWidth_),
                static_cast<float>(tileHeight_)};
    }

private:
    TextureHandle handle_;
    Margin margin_;
    Spacing spacing_;
    uint32_t tileWidth_;
    uint32_t tileHeight_;
    uint32_t col_;
    uint32_t row_;

    Tilesheet() = default;
};

struct TilesheetConfig final {
    std::string name;
    uint32_t row, col;
    Margin margin = Margin::Zero();
    Spacing spacing = {0, 0};
};

class TilesheetManager final {
public:
    explicit TilesheetManager(TextureManager&);
    Tilesheet& CreateFromTexture(TextureHandle, const std::string& name,
                               uint32_t col, uint32_t row,
                               const Margin& margin = Margin::Zero(),
                               const Spacing& spacing = {0, 0});
    Tilesheet& LoadFromConfig(TextureHandle, std::string_view configFilename);
    Tilesheet& Find(std::string_view name);

private:
    TextureManager* imageManager_;
    std::unordered_map<std::string, Tilesheet> tilesheets_;
};

}  // namespace nickel