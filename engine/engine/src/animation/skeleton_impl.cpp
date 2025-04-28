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
    if (config.ShouldLoadSkeleton()) {
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

    for (auto& skin : load_data.m_skins) {
        NICKEL_CONTINUE_IF_FALSE(using_skeleton_set->contains(skin.m_name));
        SkeletonImpl* skeleton = loadOneSkeleton(load_data, skin);
        m_skeletons.emplace(skeleton->m_name, skeleton);
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
        SkeletonImpl* skeleton = loadOneSkeleton(load_data, *result);
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
    const GLTFImportData& load_data, const Skin& skin) {
    std::vector<Bone> bones;
    bones.resize(skin.m_bone_indices.size());

    // mapping load_data indices to store indices
    std::unordered_map<uint32_t, uint32_t> bone_idx_mapping;

    for (auto& bone_idx : skin.m_bone_indices) {
        auto& node = load_data.m_nodes[bone_idx];
        Bone bone;
        bone.m_origin_trans = node.m_local_transform;
        bone.m_transform = node.m_local_transform;
        bone.m_global_transform = node.m_global_transform;
        bone_idx_mapping.emplace(bone_idx, bones.size());
        bones.emplace_back(std::move(bone));
    }

    std::vector<SkeletonImpl::BoneChildren> children_list;
    children_list.resize(skin.m_bone_indices.size());
    for (auto& bone_idx : skin.m_bone_indices) {
        uint32_t bone_store_idx = bone_idx_mapping[bone_idx];
        auto& children = children_list[bone_store_idx];
        auto& node = load_data.m_nodes[bone_idx];

        for (uint32_t child_idx : node.m_children) {
            uint32_t child_store_idx = bone_idx_mapping[child_idx];
            children.push_back(child_store_idx);
        }
    }

    SkeletonImpl* skeleton = m_skeleton_allocator.Allocate(this);
    std::string name = load_data.m_asset_name + "." + skin.m_name;
    skeleton->m_name = name;
    skeleton->m_bones = std::move(bones);
    skeleton->m_hierarchy = std::move(children_list);
    skeleton->m_root_bone_idx = bone_idx_mapping[skin.m_root.value()];
    return skeleton;
}

}  // namespace nickel