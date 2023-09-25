#include "anim/anim.hpp"
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
        if (reg.has(entity, track->GetApplyTarget().type_node())) {
            mirrow::drefl::reference_any data =
                reg.get_mut(entity, track->GetApplyTarget().type_node());
            auto typeInfo = data.type();
            if (!typeInfo.is_class()) {
                LOGI(log_tag::Misc,
                     "currently we don't support non-class in animation");
                continue;
            }

            auto classInfo = typeInfo.as_class();
            for (auto var : classInfo.vars()) {
                auto trackTypeInfo = track->TypeInfo();
                if (var.node()->type->raw_type == trackTypeInfo.type_node() &&
                    var.name() == name) {
                        auto field =
                            mirrow::drefl::invoke_by_any_return_ref(var, &data);
                        field.deep_set(track->GetValueAt(curTime_));
                    }
            }
        }
    }
}

}  // namespace nickel