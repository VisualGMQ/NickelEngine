#pragma once
#include "nickel/animation/bone.hpp"
#include "nickel/animation/skeleton.hpp"
#include "nickel/common/memory/memory.hpp"
#include "nickel/graphics/lowlevel/buffer.hpp"

namespace nickel {
struct Skin;
struct GLTFImportData;

class SkeletonImpl : public RefCountable {
public:
    using BoneChildren = std::vector<uint32_t>;

    SkeletonImpl(SkeletonManagerImpl*);
    void DecRefcount() override;

    std::string m_name;
    std::vector<Bone> m_bones;
    uint32_t m_root_bone_idx{};
    std::vector<BoneChildren> m_hierarchy;
    
    graphics::Buffer m_inverse_bind_matrices;
    // graphics::Buffer m_bone_global_pose_matrices;

    void UpdateTransformByRoot(const Transform&);

private:
    SkeletonManagerImpl* m_mgr;

    void updateTransformRecursive(uint32_t bone_idx, const Transform* parent);
};

class SkeletonManagerImpl {
public:
    bool Load(const GLTFImportData&, const GLTFLoadConfig& config);
    bool Load(const GLTFImportData&, const std::string& load_skeleton_name);
    Skeleton Find(const std::string& name);
    void Remove(SkeletonImpl&);
    void GC();
    void Clear();

    BlockMemoryAllocator<SkeletonImpl> m_skeleton_allocator;
    std::unordered_map<std::string, SkeletonImpl*> m_skeletons;

private:
    SkeletonImpl* loadOneSkeleton(const GLTFImportData&, const Skin&, const std::string& skin_name);
};

}  // namespace nickel