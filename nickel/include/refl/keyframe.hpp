#pragma once

#include "anim/keyframe.hpp"
#include "pch.hpp"
#include "refl/cgmath.hpp"

namespace mirrow::serd::srefl {

using ::nickel::KeyFrame;

namespace impl {

template <typename T>
struct has_serialize_method<T, std::enable_if_t<std::is_same_v<
                                   T, KeyFrame<typename T::value_type>>>> {
    static constexpr bool value = true;
};

}

template <typename T>
std::enable_if_t<std::is_same_v<T, KeyFrame<typename T::value_type>>>
serialize(const T& frame, serialize_destination_type_t<T>& tbl) {
    mirrow::serd::srefl::serialize_destination_type_t<typename T::value_type> frameTbl;
    serialize(frame.value, frameTbl);
    tbl.emplace("value", frameTbl);
    tbl.emplace("time", static_cast<toml::int64_t>(frame.timePoint));
}

template <typename T>
std::enable_if_t<std::is_same_v<T, KeyFrame<typename T::value_type>>>
deserialize(const toml::node& node, T& frame) {
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
