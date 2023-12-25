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
    Tilesheet(const TextureManager&, TextureHandle, uint32_t col, uint32_t row,
              const Margin& margin = Margin::Zero(),
              const Spacing& spacing = {0, 0});
    Tilesheet(const std::filesystem::path& root, const std::filesystem::path& relativePath);

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

    /**
     * @brief save tilesheet info to toml file
     */
    void Save(const std::filesystem::path& path) const;

    toml::table Save2Toml() const override;

private:
    TextureHandle handle_;
    Margin margin_;
    Spacing spacing_;
    uint32_t tileWidth_;
    uint32_t tileHeight_;
    uint32_t col_;
    uint32_t row_;
    std::filesystem::path configFilename_;

    Tilesheet() = default;
};

using TilesheetHandle = Handle<Tilesheet>;

template <>
std::unique_ptr<Tilesheet> LoadAssetFromToml(const toml::table&,
                                     const std::filesystem::path&);

class TilesheetManager final : public Manager<Tilesheet> {
public:
    static FileType GetFileType() { return FileType::Tilesheet; }

    TilesheetHandle Create(TextureHandle, uint32_t col, uint32_t row,
                           const Margin& margin = Margin::Zero(),
                           const Spacing& spacing = {0, 0});
    TilesheetHandle Load(const std::filesystem::path&);
};

}  // namespace nickel