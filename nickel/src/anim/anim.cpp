#include "anim/anim.hpp"
#include "mirrow/drefl/value_kind.hpp"
#include "refl/anim.hpp"

namespace nickel {

Animation Animation::Null = Animation{};

AnimationHandle AnimationManager::CreateFromTracks(
    typename Animation::container_type&& tracks) {
    auto handle = AnimationHandle::Create();
    storeNewItem(handle, std::make_unique<Animation>(std::move(tracks)));
    return handle;
}

std::shared_ptr<Animation> AnimationManager::CreateSolitaryFromTracks(
    typename Animation::container_type&& tracks) {
    return std::make_shared<Animation>(std::move(tracks));
}

AnimTrackSerialMethods::serialize_fn_type
AnimTrackSerialMethods::GetSerializeMethod(
    AnimTrackSerialMethods::type_info_type type) {
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

void AnimationPlayer::Sync(gecs::entity entity, gecs::registry reg) {
    if (!mgr_->Has(handle_)) {
        return;
    }

    auto& anim = mgr_->Get(handle_);
    for (auto& [name, track] : anim.Tracks()) {
        if (reg.has(entity, track->GetApplyTarget())) {
            auto data = reg.get_mut(entity, track->GetApplyTarget());
            auto typeInfo = data.type_info();
            if (typeInfo->kind() != mirrow::drefl::value_kind::Class) {
                LOGI(log_tag::Misc,
                     "currently we don't support non-class in animation");
                continue;
            }

            auto classInfo = typeInfo->as_class();
            for (auto prop : classInfo->properties()) {
                auto trackTypeInfo = track->TypeInfo();
                if (prop->type_info() == trackTypeInfo &&
                    prop->name() == name) {
                        auto field = prop->call(data);
                        field.steal_assign(track->GetValueAt(curTime_));
                    }
            }
        }
    }
}

}  // namespace nickel