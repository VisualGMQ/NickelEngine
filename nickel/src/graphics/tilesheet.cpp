#include "graphics/tilesheet.hpp"
#include "common/asset_manager.hpp"

#define RAPIDXML_NO_EXCEPTIONS
#include "rapidxml.hpp"
#include "rapidxml_print.hpp"
#include "rapidxml_utils.hpp"

namespace nickel {

Tilesheet Tilesheet::Null;

Tilesheet::Tilesheet(TextureHandle handle,
                     uint32_t col, uint32_t row, const Margin& margin,
                     const Spacing& spacing)
    : margin_(margin),
      spacing_(spacing),
      row_(row),
      col_(col) {
    if (handle) {
        auto& image = *handle.GetDataConst();
        recalcTile(image.Size());
        handle_ = handle;
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

Tilesheet::Tilesheet(const toml::table& tbl) {}

Tilesheet::Tilesheet(const std::filesystem::path& filename) {
    auto parse = toml::parse_file(filename.string());
    if (!parse) {
        LOGW(log_tag::Asset, "load tilesheet from ", filename,
             " failed:", parse.error());
    } else {
        *this = Tilesheet{parse.table()};
    }
}

void Tilesheet::parseFromToml(const toml::table& tbl) {}

bool Tilesheet::Load(const toml::table& tbl) {
    auto ref = mirrow::drefl::any_make_ref(margin_);
    if (auto node = tbl.get(ref.type_info()->name());
        node && node->is_table()) {
        mirrow::serd::drefl::deserialize(ref, *node);
    }

    ref = mirrow::drefl::any_make_ref(spacing_);
    if (auto node = tbl.get(ref.type_info()->name());
        node && node->is_table()) {
        mirrow::serd::drefl::deserialize(ref, *node);
    }

    if (auto node = tbl.get("row"); node && node->is_integer()) {
        row_ = node->as_integer()->get();
    }

    if (auto node = tbl.get("col"); node && node->is_integer()) {
        col_ = node->as_integer()->get();
    }

    auto& mgr = AssetManager::Instance();
    if (auto node = tbl.get("texture"); node && node->is_string()) {
        std::filesystem::path texturePath = node->as_string()->get();
        if (auto handle = mgr.Find<Texture>(texturePath); handle) {
            handle_ = handle;
        }
    }

    recalcTile(handle_.GetDataConst()->Size());
    return true;
}

bool Tilesheet::Save(toml::table& tbl) const {
    auto ref = mirrow::drefl::any_make_constref(margin_);
    mirrow::serd::drefl::serialize(tbl, ref, ref.type_info()->name());
    ref = mirrow::drefl::any_make_constref(spacing_);
    mirrow::serd::drefl::serialize(tbl, ref, ref.type_info()->name());

    tbl.emplace("row", row_);
    tbl.emplace("col", col_);

    if (handle_) {
        tbl.emplace("texture",
                    handle_.GetDataConst()->GetRelativePath().string());
    }
    return true;
}

TilesheetHandle LoadTilesheetFromTMX(const rapidxml::xml_node<char>* node,
                                     const std::filesystem::path& filename) {
    cgmath::Vec<uint32_t, 2> tileSize;
    if (auto n = node->first_attribute("tilewidth"); n) {
        tileSize.w = std::atoi(n->value());
    } else {
        return {};
    }
    if (auto n = node->first_attribute("tileheight"); n) {
        tileSize.h = std::atoi(n->value());
    } else {
        return {};
    }

    uint32_t tileCount = 0;
    if (auto n = node->first_attribute("tilecount"); n) {
        tileCount = std::atoi(node->value());
    } else {
        return {};
    }

    uint32_t columns = 0;
    if (auto n = node->first_attribute("columns"); n) {
        columns = std::atoi(node->value());
    } else {
        return {};
    }

    std::string name;
    if (auto n = node->first_attribute("name"); n) {
        name = node->value();
    }

    uint32_t spacing = 0;
    if (auto n = node->first_attribute("spacing"); n) {
        spacing = std::atoi(node->value());
    }

    uint32_t margin = 0;
    if (auto n = node->first_attribute("margin"); n) {
        margin = std::atoi(node->value());
    }

    TextureHandle texture;
    if (auto imageNode = node->first_node("image"); imageNode) {
        if (auto n = imageNode->first_attribute("source"); n) {
            std::filesystem::path path = filename.parent_path() / n->value();

            if (auto handle = AssetManager::Instance().Find<Texture>(path);
                handle) {
                texture = handle;
            }
        }
    } else {
        return {};
    }

    if (texture) {
        Handle<Tilesheet> handle = Handle<Tilesheet>::Create(texture, columns, tileCount / columns,
                                  Margin{margin, margin, margin, margin},
                                  Spacing{spacing, spacing});
        return handle;
    }

    return {};
}

}  // namespace nickel