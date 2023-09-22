#pragma once

#include "anim/keyframe.hpp"
#include "pch.hpp"
#include "refl/cgmath.hpp"

namespace mirrow::serd::srefl::impl {

using ::nickel::KeyFrame;

template <typename T>
struct has_serialize_method<T, std::void_t<std::enable_if_t<std::is_same_v<
                                   T, KeyFrame<typename T::value_type>>, T>>> {
    static constexpr bool value = true;
};

template <typename T>
std::enable_if_t<std::is_same_v<T, KeyFrame<typename T::value_type>>>
serialize_impl(const T& frame, serialize_destination_type_t<T>& tbl) {
    tbl.emplace("value", serialize<util::remove_cvref_t<decltype(frame.value)>>(frame.value));
    tbl.emplace("time", static_cast<toml::int64_t>(frame.timePoint));
}

template <typename T>
std::enable_if_t<std::is_same_v<T, KeyFrame<typename T::value_type>>>
deserialize_impl(const toml::node& node, T& frame) {
    Assert(node.is_table(), "node must be table when deserialize KeyFrame");

    auto& tbl = *node.as_table();
    if (auto valueNode = tbl["value"]; valueNode) {
        deserialize<util::remove_cvref_t<decltype(frame.value)>>(
            *valueNode.node(), frame.value);
    }
    if (auto timeNode = tbl["time"]; timeNode.is_integer()) {
        frame.timePoint = timeNode.as_integer()->get();
    }
}

}  // namespace mirrow::serd::srefl::impl
