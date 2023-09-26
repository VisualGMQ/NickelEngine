#pragma once

#include "core/gogl.hpp"
#include "pch.hpp"

namespace mirrow::serd::srefl {

using ::nickel::gogl::Sampler;

namespace impl {

template <typename T>
struct has_serialize_method<
    T, std::enable_if_t<std::is_same_v<T, Sampler>>> {
    static constexpr bool value = true;
};

}

template <typename T>
std::enable_if_t<std::is_same_v<T, Sampler>> serialize(
    const T& sampler, serialize_destination_type_t<T>& tbl) {
    tbl.emplace("mipmap", sampler.mipmap);

    toml::table filterTbl;
    filterTbl.emplace(
        "min", ::nickel::gogl::GetTextureFilterTypeName(sampler.filter.min));
    filterTbl.emplace(
        "mag", ::nickel::gogl::GetTextureFilterTypeName(sampler.filter.mag));

    tbl.emplace("filter", filterTbl);

    toml::table wrapperTbl;

    wrapperTbl.emplace(
        "r", ::nickel::gogl::GetTextureWrapperTypeName(sampler.wrapper.r));
    wrapperTbl.emplace(
        "s", ::nickel::gogl::GetTextureWrapperTypeName(sampler.wrapper.s));
    wrapperTbl.emplace(
        "t", ::nickel::gogl::GetTextureWrapperTypeName(sampler.wrapper.t));

    if (sampler.wrapper.NeedBorderColor()) {
        toml::array borderColor;
        borderColor.push_back(sampler.wrapper.borderColor[0]);
        borderColor.push_back(sampler.wrapper.borderColor[1]);
        borderColor.push_back(sampler.wrapper.borderColor[2]);
        borderColor.push_back(sampler.wrapper.borderColor[3]);
        wrapperTbl.emplace("border_color", borderColor);
    }

    tbl.emplace("wrapper", wrapperTbl);
}

template <typename T>
std::enable_if_t<std::is_same_v<T, Sampler>> deserialize(
    const toml::node& node, T& sampler) {
    Assert(node.is_table(), "node must be table when deserialize KeyFrame");

    auto& tbl = *node.as_table();

    if (auto node = tbl["mipmap"]; node.is_boolean()) {
        sampler.mipmap = node.as_boolean()->get();
    }

    if (auto node = tbl["filter"]; node.is_table()) {
        auto& filterTbl = *node.as_table();

        if (auto filterMin = filterTbl["min"]; filterMin.is_string()) {
            if (auto filter = ::nickel::gogl::GetTextureFilterTypeByName(
                    filterMin.as_string()->get());
                filter) {
                sampler.filter.min = filter.value();
            }
        }

        if (auto filterMag = filterTbl["mag"]; filterMag.is_string()) {
            if (auto filter = ::nickel::gogl::GetTextureFilterTypeByName(
                    filterMag.as_string()->get());
                filter) {
                sampler.filter.mag = filter.value();
            }
        }
    }

    if (auto node = tbl["wrapper"]; node.is_table()) {
        auto& wrapperTbl = *node.as_table();

        if (auto s = wrapperTbl["s"]; s.is_string()) {
            if (auto wrapper = ::nickel::gogl::GetTextureWrapperTypeFromName(
                    s.as_string()->get());
                wrapper) {
                sampler.wrapper.s = wrapper.value();
            }
        }
        if (auto r = wrapperTbl["r"]; r.is_string()) {
            if (auto wrapper = ::nickel::gogl::GetTextureWrapperTypeFromName(
                    r.as_string()->get())) {
                sampler.wrapper.r = wrapper.value();
            }
        }
        if (auto t = wrapperTbl["t"]; t.is_string()) {
            if (auto wrapper = ::nickel::gogl::GetTextureWrapperTypeFromName(
                    t.as_string()->get());
                wrapper) {
                sampler.wrapper.t = wrapper.value();
            }
        }
        if (auto color = wrapperTbl["border_color"]; color.is_array()) {
            auto& arr = *color.as_array();
            sampler.wrapper.borderColor[0] =
                static_cast<float>(arr[0].as_floating_point()->get());
            sampler.wrapper.borderColor[1] =
                static_cast<float>(arr[1].as_floating_point()->get());
            sampler.wrapper.borderColor[2] =
                static_cast<float>(arr[2].as_floating_point()->get());
            sampler.wrapper.borderColor[3] =
                static_cast<float>(arr[3].as_floating_point()->get());
        }
    }
}

}  // namespace mirrow::serd::srefl::impl