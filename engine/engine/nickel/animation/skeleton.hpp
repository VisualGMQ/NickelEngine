#pragma once
#include "nickel/animation/bone.hpp"
#include "nickel/common/impl_wrapper.hpp"
#include "nickel/common/memory/memory.hpp"

namespace nickel {

class Skeleton {
public:
    explicit Skeleton(BlockMemoryAllocator<Bone>& allocator);
    
private:
    Bone* m_root_bone{};
    BlockMemoryAllocator<Bone>& m_bone_allocator;
};

class SkeletonManagerImpl;

class SkeletonManager: public ImplWrapper<SkeletonManagerImpl> {
public:
    ~SkeletonManager();

private:
    std::unique_ptr<SkeletonManagerImpl> m_impl;
};

}