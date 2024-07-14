#include "anim/anim.hpp"
#include "common/ecs.hpp"

namespace nickel {

Animation Animation::Null = Animation{};

bool Animation::Load(const toml::table& tbl) {
    if (auto node = tbl.get("tracks"); node && node->is_array_of_tables()) {
        auto& arr = *node->as_array();

        for (auto& elem : arr) {
            auto& elemTbl = *elem.as_table();

            if (auto valueNode = elemTbl.get("value_type");
                valueNode->is_string()) {
                auto [create, serialize] =
                    AnimTrackLoadMethods::Instance().Find(
                        mirrow::drefl::typeinfo(valueNode->as_string()->get()));
                if (serialize) {
                    tracks_.emplace_back(serialize(elemTbl));
                }
            }
        }
    } else {
        return false;
    }

    return true;
}

bool Animation::Save(toml::table& tbl) const {
    toml::array arr;
    for (auto& track : tracks_) {
        arr.push_back(track->Save2Toml());
    }
    tbl.emplace("tracks", arr);
    return true;
}

bool AnimTrackLoadMethods::Contain(type_info_type type) {
    return methods_.count(type) != 0;
}

AnimTrackLoadMethods::Method AnimTrackLoadMethods::Find(
    type_info_type type) const {
    if (auto it = methods_.find(type); it != methods_.end()) {
        return it->second;
    }
    return {nullptr, nullptr};
}

void AnimationPlayer::Sync(gecs::entity entity, gecs::registry reg) {
    if (!handle_) {
        return;
    }

    auto& anim = *handle_.GetData();
    for (auto& track : anim.Tracks()) {
        if (reg.has(entity, track->GetApplyTarget())) {
            auto obj = reg.get_mut(entity, track->GetApplyTarget());
            auto type = obj.type_info();

            getTarget(type, track->PropertyLink(), 0, obj);

            obj.steal_assign(track->GetValueAt(curTime_));
        }
    }

    auto timer = reg.res<nickel::Time>();
    curTime_ += timer->Elapse();

    if (curTime_ >= Duration()) {
        Stop();
    }
}

void AnimationPlayer::getTarget(const mirrow::drefl::type* type,
                                const std::vector<std::string>& propertyLink,
                                size_t idx, mirrow::drefl::any& obj) {
    if (idx >= propertyLink.size()) {
        LOGE(nickel::log_tag::Editor,
             "can't find correspond type in animation");
        return;
    }

    if (type->is_class()) {
        auto clazz = type->as_class();

        for (auto& prop : clazz->properties()) {
            auto& property = propertyLink[idx];
            if (prop->name() == property) {
                obj = prop->call(obj);
                getTarget(prop->type_info(), propertyLink, idx + 1, obj);
                return;
            }
        }
    }
}

}  // namespace nickel