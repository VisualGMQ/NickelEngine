#pragma once
#include "nickel/animation/bone.hpp"
#include "nickel/animation/skeleton.hpp"
#include "nickel/common/memory/memory.hpp"

namespace nickel {

class SkeletonManagerImpl {
public:
    BlockMemoryAllocator<Bone> m_bone_allocator;
    BlockMemoryAllocator<Skeleton> m_skeleton_allocator;
};

}