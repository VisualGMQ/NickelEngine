#include "graphics/gogl.hpp"

namespace nickel::gogl {

const std::unordered_map<TextureWrapperType, const char*> gTextureWrapperNameMap = {
    {TextureWrapperType::ClampToBorder, "ClampToBorder"},
    {TextureWrapperType::ClampToEdge, "ClampToEdge"},
    {TextureWrapperType::Repeat, "Repeat"},
    {TextureWrapperType::MirroredRepeat, "MirroredRepeat"},
};

std::string_view GetTextureWrapperTypeName(TextureWrapperType type) {
    if (auto it = gTextureWrapperNameMap.find(type); it != gTextureWrapperNameMap.end()) {
        return it->second;
    } else {
        return {};
    }
}

std::optional<TextureWrapperType> GetTextureWrapperTypeFromName(std::string_view name) {
    for (auto& [wrapper, wrapperName] : gTextureWrapperNameMap) {
        if (name == wrapperName) {
            return wrapper;
        }
    }

    return std::nullopt;
}


const std::unordered_map<TextureFilterType, std::string_view> gTextureFilterTypeNameMap = {
    {TextureFilterType::Linear, "Linear"},
    {TextureFilterType::Nearest, "Nearest"},
    {TextureFilterType::LinearMipmapLinear, "LinearMipmapLinear"},
    {TextureFilterType::LinearMipmapNearest, "LinearMipmapNearest"},
    {TextureFilterType::NearestMipmapLinear, "NearestMipmapLinear"},
    {TextureFilterType::NearestMipmapNearest, "NearestMipmapNearest"},
};

std::string_view GetTextureFilterTypeName(TextureFilterType type) {
    if (auto it = gTextureFilterTypeNameMap.find(type); it != gTextureFilterTypeNameMap.end()) {
        return it->second;
    }
    return {};
}

std::optional<TextureFilterType> GetTextureFilterTypeByName(std::string_view) {
    for (auto& [filter, name] : gTextureFilterTypeNameMap) {
        if (name == name) {
            return filter;
        }
    }

    return std::nullopt;
}

}