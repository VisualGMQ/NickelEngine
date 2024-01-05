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

class Tilesheet final: public Asset {
public:
    friend class TilesheetInnerAccessor;

    Tilesheet(const TextureManager&, TextureHandle, uint32_t col, uint32_t row,
              const Margin& margin = Margin::Zero(),
              const Spacing& spacing = {0, 0});

    explicit Tilesheet(const toml::table& tbl);
    explicit Tilesheet(const std::filesystem::path& configFilename);

    Tilesheet(const Tilesheet&) = delete;
    Tilesheet(Tilesheet&&) = default;
    Tilesheet& operator=(const Tilesheet&) = delete;
    Tilesheet& operator=(Tilesheet&&) = default;

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
        return {tileWidth_, tileHeight_};
    }

    toml::table Save2Toml() const override;

private:
    TextureHandle handle_;
    Margin margin_;
    Spacing spacing_;
    float tileWidth_;
    float tileHeight_;
    uint32_t col_;
    uint32_t row_;

    void parseFromToml(const toml::table&);

    void recalcTile(const cgmath::Vec2& textureSize) {
        tileWidth_ = (textureSize.w - margin_.left - margin_.right -
                      (col_ - 1) * spacing_.x) /
                     static_cast<float>(col_);
        tileHeight_ = (textureSize.h - margin_.top - margin_.bottom -
                       (row_ - 1) * spacing_.y) /
                      static_cast<float>(row_);
    }

    Tilesheet() = default;
};

using TilesheetHandle = Handle<Tilesheet>;

class TilesheetInnerAccessor final {
public:
    explicit TilesheetInnerAccessor(Tilesheet& ts) : ts_{ts} {}

    auto& Margin() { return ts_.margin_; }

    auto& Spacing() { return ts_.spacing_; }

    auto& Col() { return ts_.col_; }

    auto& Row() { return ts_.row_; }

    auto& Margin() const { return ts_.margin_; }

    auto& Spacing() const { return ts_.spacing_; }

    auto& Col() const { return ts_.col_; }

    auto& Row() const { return ts_.row_; }

private:
    Tilesheet& ts_;
};

template <>
std::unique_ptr<Tilesheet> LoadAssetFromMeta(const toml::table&);

class TilesheetManager final : public Manager<Tilesheet> {
public:
    static FileType GetFileType() { return FileType::Tilesheet; }

    TilesheetHandle Create(TextureHandle, uint32_t col, uint32_t row,
                           const Margin& margin = Margin::Zero(),
                           const Spacing& spacing = {0, 0});
    TilesheetHandle Load(const std::filesystem::path&);
};

TilesheetHandle LoadTilesheetFromTMX(const rapidxml::xml_node<char>* node,
                                     const std::filesystem::path& filename);

}  // namespace nickel