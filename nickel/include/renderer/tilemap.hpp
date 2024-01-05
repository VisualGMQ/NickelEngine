#pragma once

#include "pch.hpp"
#include "renderer/tilesheet.hpp"

namespace nickel {

class TileLayer final {
public:
    TileLayer() : map_{} {}

    TileLayer(uint32_t w, uint32_t h)
        : map_{w, h} {}

    auto Get(uint32_t x, uint32_t y) const { return map_.Get(x, y); }

    void Set(uint32_t x, uint32_t y, uint32_t value) {
        return map_.Set(x, y, value);
    }

    void Resize(uint32_t w, uint32_t h) { map_.Resize(w, h); }

    void SetName(const std::string& name) { name_ = name;}
    void SetName(std::string&& name) { name_ = std::move(name);}
    auto& GetName() const { return name_; }

    bool IsEmpty() const { return map_.Col() == 0 || map_.Row() == 0; }

    auto Size() const { return map_.Size(); }

    void SetData(const cgmath::DynMat<uint32_t>& d) { map_ = d; }
    void SetData(cgmath::DynMat<uint32_t>&& d) { map_ = std::move(d); }

private:
    std::string name_;
    cgmath::DynMat<uint32_t> map_;
};

class TileMap final {
public:
    using TileSize = cgmath::Vec<uint32_t, 2>;

    enum class Orientation {
        Ortho,
        // NOTE: currently we don't support these
        // Staggered,
        // Hexagonal,
    };

    TileMap() = default;

    void AddLayer(std::unique_ptr<TileLayer>&& layer) { layers_.emplace_back(std::move(layer)); }

    void AddTilesheet(TilesheetHandle tilesheet);

    auto& GetTilesheets() const { return tilesheets_; }

    auto& GetTilesheet(size_t idx) const { return tilesheets_[idx]; }

    auto& GetLayers() const { return layers_; }

    auto& GetLayer(size_t idx) const { return layers_[idx]; }

    void SetOrientation(Orientation o) { orientation_ = o; }
    auto GetOrientation() const { return orientation_; }

    void SetTileSize(const TileSize& size) { tileSize_ = size; }
    auto& GetTileSize() { return tileSize_; }

private:
    std::vector<std::unique_ptr<TileLayer>> layers_;
    std::vector<TilesheetHandle> tilesheets_;
    TileSize tileSize_;
    Orientation orientation_;
};

std::unique_ptr<TileMap> LoadTileMapFromTMX(const std::filesystem::path& filename);

}  // namespace nickel