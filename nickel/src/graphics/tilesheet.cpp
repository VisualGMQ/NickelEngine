#include "graphics/tilesheet.hpp"

#define RAPIDXML_NO_EXCEPTIONS
#include "rapidxml.hpp"
#include "rapidxml_print.hpp"
#include "rapidxml_utils.hpp"

namespace nickel {

Tilesheet Tilesheet::Null;

Tilesheet::Tilesheet(const TextureManager& manager, TextureHandle handle,
                     uint32_t col, uint32_t row, const Margin& margin,
                     const Spacing& spacing)
    : handle_(handle),
      margin_(margin),
      spacing_(spacing),
      row_(row),
      col_(col) {
    if (manager.Has(handle)) {
        auto& image = manager.Get(handle);
        recalcTile(image.Size());
    } else {
        handle_ = TextureHandle::Null();
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

Tilesheet::Tilesheet(const toml::table& tbl) {
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

    // auto assetMgr = gWorld->res_mut<AssetManager>();
    // if (auto node = tbl.get("texture"); node && node->is_string()) {
    //     std::filesystem::path texturePath = node->as_string()->get();
    //     if (assetMgr->Has(texturePath)) {
    //         handle_ = assetMgr->TextureMgr().GetHandle(texturePath);
    //         AssociateFile(texturePath);
    //     }
    // }

    // recalcTile(assetMgr->Get(handle_).Size());
}

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

toml::table Tilesheet::Save2Toml() const {
    toml::table tbl;

    auto ref = mirrow::drefl::any_make_constref(margin_);
    mirrow::serd::drefl::serialize(tbl, ref, ref.type_info()->name());
    ref = mirrow::drefl::any_make_constref(spacing_);
    mirrow::serd::drefl::serialize(tbl, ref, ref.type_info()->name());

    tbl.emplace("row", row_);
    tbl.emplace("col", col_);

    // auto assetMgr = gWorld->res<AssetManager>();
    // if (assetMgr->Has(handle_)) {
    //     tbl.emplace("texture", assetMgr->Get(handle_).RelativePath().string());
    // }
    return tbl;
}

template <>
std::unique_ptr<Tilesheet> LoadAssetFromMeta(const toml::table& tbl) {
    return std::make_unique<Tilesheet>(tbl);
}

TilesheetHandle TilesheetManager::Create(TextureHandle handle, uint32_t col,
                                         uint32_t row, const Margin& margin,
                                         const Spacing& spacing) {
    // auto elem =
    //     std::make_unique<Tilesheet>(gWorld->res<AssetManager>()->TextureMgr(),
    //                                 handle, col, row, margin, spacing);
    // if (elem && *elem) {
    //     auto handle = TilesheetHandle::Create();
    //     storeNewItem(handle, std::move(elem));
    //     return handle;
    // }
    return TilesheetHandle::Null();
}

TilesheetHandle TilesheetManager::Load(const std::filesystem::path& filename) {
    if (Has(filename)) {
        return GetHandle(filename);
    }

    auto handle = TilesheetHandle::Create();
    auto elem = std::make_unique<Tilesheet>(filename);
    if (elem && *elem) {
        storeNewItem(handle, std::move(elem));
        return handle;
    } else {
        return {};
    }
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
    auto assetMgr = ECS::Instance().World().res_mut<TextureManager>();
    if (auto imageNode = node->first_node("image"); imageNode) {
        if (auto n = imageNode->first_attribute("source"); n) {
            std::filesystem::path path = filename.parent_path() / n->value();
            if (assetMgr->Has(path)) {
                texture = assetMgr->GetHandle(path);
            }
        }
    } else {
        return {};
    }

    if (texture) {
        auto mgr = ECS::Instance().World().res_mut<TilesheetManager>();
        auto handle = mgr->Create(
            texture, columns, tileCount / columns,
            Margin{margin, margin, margin, margin}, Spacing{spacing, spacing});
        mgr->AssociateFile(
            handle, filename.parent_path() / (name + ".tilesheet"));
        return handle;
    }

    return {};
}

}  // namespace nickel