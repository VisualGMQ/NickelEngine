// #pragma once

#include "pch.hpp"
#include "anim/anim.hpp"
#include "refl/cgmath.hpp"
#include "refl/keyframe.hpp"
#include "refl/transform.hpp"

// Animation static serialization
namespace mirrow::serd::srefl {

using ::nickel::Animation;
using ::nickel::AnimationTrack;

namespace impl {

template <typename T>
struct has_serialize_method<
    T, std::enable_if_t<std::is_same_v<T, Animation>>> {
    static constexpr bool value = true;
};

template <typename T>
struct has_serialize_method<
    T, std::enable_if_t<std::is_same_v<T, AnimationTrack>>> {
    static constexpr bool value = true;
};

}

// AnimationTrack

template <typename T>
std::enable_if_t<std::is_same_v<T, AnimationTrack>> serialize(
    const T& track, serialize_destination_type_t<T>& tbl) {
    if (AnimTrackSerialMethods::Instance().Contain(track.TypeInfo())) {
        tbl = AnimTrackSerialMethods::Instance().GetSerializeMethod(
            track.TypeInfo())(track);
    }
}

template <typename T>
std::enable_if_t<std::is_same_v<T, AnimationTrack>> deserialize(
    const toml::node& node, T& track) {
    Assert(node.is_table(), "deserialize AnimationTrack need toml::table node");

    if (AnimTrackSerialMethods::Instance().Contain()) {
        track = std::move(
            *AnimTrackSerialMethods::Instance().GetDeserializeMethod()(
                *node.as_table()));
    }
}

// Animation

template <typename T>
std::enable_if_t<std::is_same_v<T, Animation>> serialize(
    const T& anim, serialize_destination_type_t<T>& tbl) {
    auto& tracks = anim.Tracks();

    for (auto& [name, track] : tracks) {
        toml::table trackTbl;
        serialize<AnimationTrack>(*track, trackTbl);
        tbl.emplace(name, trackTbl);
    }
}

template <typename T>
std::enable_if_t<std::is_same_v<T, Animation>> deserialize(
    const toml::node& node, T& elem) {
    Assert(node.is_table(), "deserialize Animation need toml::table node");

    typename Animation::container_type tracks;

    for (auto& [name, node] : *node.as_table()) {
        Assert(node.is_table(), "");
        auto& tbl = *node.as_table();

        auto& typeName = tbl["type"].as_string()->get();
        auto type_info = mirrow::drefl::typeinfo(typeName);

        if (AnimTrackSerialMethods::Instance().Contain(type_info)) {
            auto&& value = AnimTrackSerialMethods::Instance().GetDeserializeMethod(type_info)(*node.as_table());
            tracks.emplace(std::string(name.str()), std::move(value));
        }
    }

    elem = Animation(std::move(tracks));
}

}  // namespace mirrow::serd::srefl::impl
