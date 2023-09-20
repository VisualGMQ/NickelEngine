#include "renderer/tilesheet.hpp"
#include "refl/tilesheet.hpp"

namespace nickel {

Tilesheet Tilesheet::Null;

Tilesheet::Tilesheet(TextureManager& manager, TextureHandle handle,
                     uint32_t col, uint32_t row, const Margin& margin,
                     const Spacing& spacing)
    : handle_(handle),
      margin_(margin),
      spacing_(spacing),
      row_(row),
      col_(col) {
    if (handle) {
        auto& image = manager.Get(handle);
        tileWidth_ =
            (image.W() - margin.left - margin.right - (col - 1) * spacing.x) /
            col;
        tileHeight_ =
            (image.H() - margin.top - margin.bottom - (row - 1) * spacing.y) /
            row;
    }
}

Tile Tilesheet::Get(uint32_t x, uint32_t y) {
    return Tile{
        cgmath::Rect{
                     static_cast<float>(x * (tileWidth_ + spacing_.x) + margin_.left),
                     static_cast<float>(y * (tileHeight_ + spacing_.y) + margin_.top),
                     static_cast<float>(tileWidth_), static_cast<float>(tileHeight_)},
        handle_
    };
}

Tile Tilesheet::Get(uint32_t index) {
    return Get(index % col_, index / col_);
}

TilesheetManager::TilesheetManager(TextureManager& imageMgr)
    : imageManager_(&imageMgr) {}

Tilesheet& TilesheetManager::CreateFromTexture(TextureHandle handle,
                                               const std::string& name,
                                               uint32_t col, uint32_t row,
                                               const Margin& margin,
                                               const Spacing& spacing) {
    auto it = tilesheets_.insert_or_assign(
        name, Tilesheet(*imageManager_, handle, col, row, margin, spacing));
    return it.first->second;
}

Tilesheet& TilesheetManager::LoadFromConfig(TextureHandle handle,
                                            std::string_view configFilename) {
    auto parseResult = toml::parse_file(configFilename);
    if (!parseResult) {
        LOGE(log_tag::Res, "load tilesheet from config file ", configFilename,
             " failed!\nError: ", parseResult.error());
        return Tilesheet::Null;
    } else {
        TilesheetConfig config;
        mirrow::serd::srefl::deserialize<TilesheetConfig>(parseResult.table(),
                                                          config);
        return CreateFromTexture(handle, config.name, config.col, config.row,
                                 config.margin, config.spacing);
    }
}

Tilesheet& TilesheetManager::Find(std::string_view name) {
    if (auto it = tilesheets_.find(std::string(name));
        it != tilesheets_.end()) {
        return it->second;
    } else {
        LOGW("tilesheet ", name, " not found");
        return Tilesheet::Null;
    }
}

}  // namespace nickel