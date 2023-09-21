#include "anim/anim.hpp"

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

}