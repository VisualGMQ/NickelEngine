#include "anim/anim.hpp"
#include "mirrow/drefl/value_kind.hpp"
#include "misc/asset_manager.hpp"

namespace nickel {

Animation Animation::Null = Animation{};

/*
bool AnimationTrack::changeApplyTarget(const mirrow::drefl::type* typeInfo) {
    if (propLink_.empty()) {
        applyTypeInfo_ = typeInfo;
        return true;
    }

    Assert(typeInfo->is_class(),
           "must apply class target when has property link");

    int i = 0;
    const mirrow::drefl::type* type = typeInfo;
    while (i < propLink_.size() - 1) {
        if (!type->is_class()) {
            LOGW(log_tag::Asset, type->name(), " isn't class");
            return false;
        }

        auto clazz = type->as_class();
        for (auto& prop : clazz->properties()) {
            if (prop->name() == propLink_[i] && prop->type_info()->is_class()) {
                type = prop->type_info()->as_class();
                i++;
                break;
            }

            LOGW(log_tag::Asset, "type ", typeInfo->name(),
                 " don't has property link ", propLink_);
            return false;
        }
    }

    if (type != valueType_) {
        LOGW(log_tag::Asset, "can't apply ", type->name(), " to ",
             valueType_->name());
        return false;
    }

    return true;
}
*/

AnimationPlayer::AnimationPlayer()
    : mgr_{&gWorld->res_mut<nickel::AssetManager>()->AnimationMgr()} {}

Animation::Animation(const toml::table& tbl) {
    if (auto node = tbl.get("tracks"); node && node->is_array_of_tables()) {
        auto& arr = *node->as_array();

        for (auto& elem : arr) {
            auto& elemTbl = *elem.as_table();

            if (auto valueNode = elemTbl.get("value_type");
                valueNode->is_string()) {
                auto f = AnimTrackLoadMethods::Instance().Find(
                    mirrow::drefl::typeinfo(valueNode->as_string()->get()));
                if (f) {
                    tracks_.emplace_back(f(elemTbl));
                }
            }
        }
    }
}

Animation::Animation(const std::filesystem::path& filename) : Asset(filename) {
    auto result = toml::parse_file(filename.string());
    if (!result) {
        LOGW(log_tag::Asset, "load animation from ", filename,
             " failed: ", result.error());
    } else {
        *this = Animation(result.table());
    }
}

void Animation::Save(const std::filesystem::path& path) const {
    toml::table tbl;
    toml::array arr;
    for (auto& track : tracks_) {
        arr.push_back(track->Save2Toml());
    }
    tbl.emplace("tracks", arr);

    std::ofstream file(path);
    if (file) {
        file << tbl;
    }
}

template <>
std::unique_ptr<Animation> LoadAssetFromMeta(const toml::table& tbl) {
    if (auto path = tbl.get("path"); path && path->is_string()) {
        return std::make_unique<Animation>(path->as_string()->get());
    }
    return nullptr;
}

std::shared_ptr<Animation> AnimationManager::CreateSolitaryFromTracks(
    typename Animation::container_type&& tracks) {
    return std::make_shared<Animation>(std::move(tracks));
}

AnimationHandle AnimationManager::Load(const std::filesystem::path& filename) {
    if (Has(filename)) {
        return GetHandle(filename);
    }

    AnimationHandle handle = AnimationHandle::Create();
    auto anim = std::make_unique<Animation>(filename);
    if (anim && *anim) {
        storeNewItem(handle, std::move(anim));
        return handle;
    } else {
        return AnimationHandle::Null();
    }
}

bool AnimTrackLoadMethods::Contain(type_info_type type) {
    return methods_.count(type) != 0;
}

AnimTrackLoadMethods::deserialize_fn_type AnimTrackLoadMethods::Find(
    type_info_type type) {
    if (auto it = methods_.find(type); it != methods_.end()) {
        return it->second;
    }
    return nullptr;
}

void AnimationPlayer::Sync(gecs::entity entity, gecs::registry reg) {
    if (!mgr_->Has(handle_)) {
        return;
    }

    auto& anim = mgr_->Get(handle_);
    for (auto& track : anim.Tracks()) {
        if (reg.has(entity, track->GetApplyTarget())) {
            auto data = reg.get_mut(entity, track->GetApplyTarget());
            auto type = data.type_info();
            for (auto& prop : track->PropertyLink()) {
                for (auto& classProp :
                     track->GetApplyTarget()->as_class()->properties()) {
                    if (classProp->name() == prop) {
                        data = classProp->call(data);
                    }
                }
            }

            data.steal_assign(track->GetValueAt(curTime_));
        }
    }

    auto timer = reg.res<nickel::Time>();
    curTime_ += timer->Elapse();

    if (curTime_ >= Duration()) {
        Stop();
    }
}

}  // namespace nickel