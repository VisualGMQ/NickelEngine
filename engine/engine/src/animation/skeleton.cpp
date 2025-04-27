#include "nickel/animation/skeleton.hpp"
#include "nickel/animation/internal/skeleton_impl.hpp"

namespace nickel {

Skeleton::Skeleton(BlockMemoryAllocator<Bone>& allocator)
    : m_bone_allocator{allocator} {}

SkeletonManager::~SkeletonManager() {}

}  // namespace nickel