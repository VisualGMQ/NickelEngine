#include "nickel/animation/internal/skeleton_impl.hpp"

#include "nickel/common/macro.hpp"
#include "nickel/importer/internal/gltf_importer_impl.hpp"

#include <unordered_set>

namespace nickel {

SkeletonImpl::SkeletonImpl(SkeletonManagerImpl* mgr) : m_mgr{mgr} {}

void SkeletonImpl::DecRefcount() {
    RefCountable::DecRefcount();

    if (Refcount() == 0) {
        m_mgr->m_skeleton_allocator.MarkAsGarbage(this);
        m_mgr->Remove(*this);
    }
}

void SkeletonImpl::UpdateTransformByRoot(const Transform& transform) {
    updateTransformRecursive(m_root_bone_idx, &transform);
}

void SkeletonImpl::updateTransformRecursive(uint32_t bone_idx,
                                            const Transform* parent) {
    Bone& bone = m_bones[bone_idx];
    if (parent) {
        bone.m_global_transform = *parent * bone.m_transform;
    } else {
        bone.m_transform = *parent;
        bone.m_global_transform = *parent;
    }

    auto& children = m_hierarchy[bone_idx];
    for (uint32_t child : children) {
        updateTransformRecursive(child, &bone.m_global_transform);
    }
}

bool SkeletonManagerImpl::Load(const GLTFImportData& load_data,
                               const GLTFLoadConfig& config) {
    if (!config.ShouldLoadSkeleton()) {
        return true;
    }

    std::unordered_set<std::string> skeleton_names;
    const std::unordered_set<std::string>* using_skeleton_set =
        &config.m_skeletons;

    if (config.ShouldLoadAllSkeleton()) {
        for (auto& skin : load_data.m_skins) {
            skeleton_names.insert(skin.m_name);
        }
        using_skeleton_set = &skeleton_names;
    }

    uint32_t name_idx = 0;
    for (auto& skin : load_data.m_skins) {
        NICKEL_CONTINUE_IF_FALSE(using_skeleton_set->contains(skin.m_name));
        std::string skin_name = skin.m_name;
        if (load_data.m_skins.size() == 1) {
            skin_name.clear();
        } else if (skin_name.empty()) {
            auto it = load_data.m_skins.end();
            do {
                skin_name = "skeleton_" + std::to_string(name_idx++);
                it = std::ranges::find_if(
                    load_data.m_skins,
                    [&](const Skin& s) { return s.m_name == skin_name; });
            } while (it != load_data.m_skins.end());
        }
        SkeletonImpl* skeleton = loadOneSkeleton(load_data, skin, skin_name);
        m_skeletons.emplace(skeleton->m_name, std::move(skeleton));
    }
    return true;
}

bool SkeletonManagerImpl::Load(const GLTFImportData& load_data,
                               const std::string& load_skeleton_name) {
    auto skins = std::span{load_data.m_skins};
    auto result = std::ranges::find_if(skins, [&](const Skin& skin) {
        return skin.m_name == load_skeleton_name;
    });
    if (result != skins.end()) {
        SkeletonImpl* skeleton =
            loadOneSkeleton(load_data, *result, load_skeleton_name);
        m_skeletons.emplace(skeleton->m_name, skeleton);
        return true;
    }
    LOGW("Failed to load skeleton {} from {}: not found", load_skeleton_name,
         load_data.m_filename);
    return false;
}

Skeleton SkeletonManagerImpl::Find(const std::string& name) {
    if (auto it = m_skeletons.find(name); it != m_skeletons.end()) {
        return it->second;
    }
    return {};
}

void SkeletonManagerImpl::Remove(SkeletonImpl& impl) {
    if (auto it = m_skeletons.find(impl.m_name); it != m_skeletons.end()) {
        m_skeletons.erase(it);
    }
}

void SkeletonManagerImpl::GC() {
    m_skeleton_allocator.GC();
}

void SkeletonManagerImpl::Clear() {
    m_skeletons.clear();
    m_skeleton_allocator.GC();
}

SkeletonImpl* SkeletonManagerImpl::loadOneSkeleton(
    const GLTFImportData& load_data, const Skin& skin,
    const std::string& skin_name) {
    std::vector<Bone> bones;
    bones.resize(skin.m_bone_indices.size());

    for (auto& bone_idx : skin.m_bone_indices) {
        auto& node = load_data.m_nodes[bone_idx];
        Bone bone;
        bone.m_origin_trans = node.m_local_transform;
        bone.m_transform = node.m_local_transform;
        bone.m_global_transform = node.m_global_transform;
        bones[skin.m_mapped_bone_indices.at(bone_idx)] = std::move(bone);
    }

    std::vector<SkeletonImpl::BoneChildren> children_list;
    children_list.resize(skin.m_bone_indices.size());
    for (auto& bone_idx : skin.m_bone_indices) {
        uint32_t bone_store_idx = skin.m_mapped_bone_indices.at(bone_idx);
        auto& children = children_list[bone_store_idx];
        auto& node = load_data.m_nodes[bone_idx];

        for (uint32_t child_idx : node.m_children) {
            uint32_t child_store_idx = skin.m_mapped_bone_indices.at(child_idx);
            children.push_back(child_store_idx);
        }
    }

    SkeletonImpl* skeleton = m_skeleton_allocator.Allocate(this);
    std::string name = load_data.m_asset_name;
    if (!skin_name.empty()) {
        name += "." + skin_name;
    }
    skeleton->m_name = name;
    skeleton->m_bones = std::move(bones);
    skeleton->m_hierarchy = std::move(children_list);
    skeleton->m_root_bone_idx =
        skin.m_mapped_bone_indices.at(skin.m_root.value());
    skeleton->m_inverse_bind_matrices = skin.m_inverse_bind_matrices_buffer;
    auto& root_bone = skeleton->m_bones[skeleton->m_root_bone_idx];
    root_bone.m_origin_trans = root_bone.m_global_transform;
    root_bone.m_transform = root_bone.m_global_transform;
    return skeleton;
}

}  // namespace nickel