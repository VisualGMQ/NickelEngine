#include "anim/anim.hpp"
#include "refl/anim.hpp"

namespace nickel {

Animation Animation::Null = Animation{};

AnimationHandle AnimationManager::CreateFromTracks(typename Animation::container_type&& tracks) {
    auto handle = AnimationHandle::Create();
    storeNewItem(handle, std::make_unique<Animation>(std::move(tracks)));
    return handle;
}

std::shared_ptr<Animation> AnimationManager::CreateSolitaryFromTracks(typename Animation::container_type&& tracks) {
    return std::make_shared<Animation>(std::move(tracks));
}

AnimTrackSerialMethods::serialize_fn_type AnimTrackSerialMethods::GetSerializeMethod(AnimTrackSerialMethods::type_info_type type) {
    if (auto it = methods_.find(type); it != methods_.end()) {
        return it->second.first;
    }
    return nullptr;
}

AnimTrackSerialMethods::deserialize_fn_type
AnimTrackSerialMethods::GetDeserializeMethod(
    AnimTrackSerialMethods::type_info_type type) {
    if (auto it = methods_.find(type); it != methods_.end()) {
        return it->second.second;
    }
    return nullptr;
}

bool AnimTrackSerialMethods::Contain(type_info_type type) {
    return methods_.count(type) != 0;
}

}  // namespace nickel