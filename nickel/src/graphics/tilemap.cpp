#include "graphics/tilemap.hpp"
#include "common/util.hpp"

#define RAPIDXML_NO_EXCEPTIONS
#include "rapidxml.hpp"
#include "rapidxml_print.hpp"
#include "rapidxml_utils.hpp"

namespace nickel {

#define GET_NODE(var, node, node_name)                             \
    (var) = (node)->first_node(node_name);                         \
    if (!(var)) {                                                  \
        LOGW(log_tag::Asset, "`", node_name, "` node not exists"); \
        return {};                                                 \
    }


std::unique_ptr<TileLayer> parseLayer(const rapidxml::xml_node<char>* node) {
    cgmath::Vec<uint32_t, 2> size;
    if (auto n = node->first_attribute("width"); n) {
        size.w = std::atoi(n->value());
    }
    if (auto n = node->first_attribute("height"); n) {
        size.h = std::atoi(n->value());
    }
    std::string name;
    if (auto n = node->first_attribute("name"); n) {
        name = n->value();
    }

    rapidxml::xml_node<char>* dataNode = node->first_node("data");
    if (!dataNode) {
        return nullptr;
    }

    if (auto attr = dataNode->first_attribute("encoding"); attr) {
        if (attr->value() != std::string_view("csv")) {
            LOGW(log_tag::Asset, "TMX layer data must encoding in CSV");
            return nullptr;
        }
    }

    CSVIterator<std::string_view> it(
        std::string_view{dataNode->value(), dataNode->value_size()});
    CSVIterator<std::string_view> end;
    cgmath::DynMat<uint32_t> datas{size.w, size.h};
    for (int x = 0; x < size.w; x++) {
        for (int y = 0; y < size.h; y++) {
            auto value = *it;
            int ivalue = 0;
            std::from_chars(value.data(), value.data() + value.size(), ivalue);
            datas.Set(x, y, ivalue);
            it++;
        }
    }

    auto layer = std::make_unique<TileLayer>();
    layer->SetData(std::move(datas));
    layer->SetName(name);
    return layer;
}

std::unique_ptr<TileMap> LoadTileMapFromTMX(
    const std::filesystem::path& filename) {
    auto content = ReadWholeFile(filename);
    if (!content) {
        return {};
    }

    rapidxml::xml_document<> doc;
    doc.parse<0>(content->data());
    rapidxml::xml_node<char>* node = {};

    // parse `map` node
    GET_NODE(node, &doc, "map");

    TileMap::TileSize tileSize;
    if (auto widthAttr = node->first_attribute("tilewidth"); widthAttr) {
        tileSize.w = std::atoi(widthAttr->value());
    }
    if (auto heightAttr = node->first_attribute("tileheight"); heightAttr) {
        tileSize.h = std::atoi(heightAttr->value());
    }

    // parse tileset
    std::vector<TilesheetHandle> tilesheets;
    rapidxml::xml_node<char>* tilesetNode;
    GET_NODE(tilesetNode, node, "tileset");
    while (tilesetNode) {
        if (auto tsxFilename = tilesetNode->first_attribute("source");
            tsxFilename) {
            auto tsxPath = filename.parent_path() / tsxFilename->value();
            auto content = ReadWholeFile(tsxPath);
            rapidxml::xml_document<> doc;
            doc.parse<0>(content->data());
            rapidxml::xml_node<char>* node = {};
            if (auto node = doc.first_node("tileset"); node) {
                tilesheets.emplace_back(LoadTilesheetFromTMX(node, tsxPath));
            }
        } else {
            tilesheets.emplace_back(LoadTilesheetFromTMX(tilesetNode, filename));
        }

        tilesetNode = tilesetNode->next_sibling("tileset");
    }

    rapidxml::xml_node<char>* layerNode;
    GET_NODE(layerNode, node, "layer");
    std::vector<std::unique_ptr<TileLayer>> layers;
    while (layerNode) {
        auto layer = parseLayer(layerNode);
        if (layer) {
            layers.emplace_back(std::move(layer));
        }
        layerNode = layerNode->next_sibling("layer");
    }

    auto tilemap = std::make_unique<TileMap>();
    for (auto& layer : layers) {
        tilemap->AddLayer(std::move(layer));
    }

    for (auto tilesheet : tilesheets) {
        tilemap->AddTilesheet(tilesheet);
    }

    return tilemap;
}

}  // namespace nickel