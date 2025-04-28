#pragma once
#include "nickel/animation/bone.hpp"
#include "nickel/common/impl_wrapper.hpp"
#include "nickel/common/memory/memory.hpp"
#include "nickel/importer/gltf_importer_config.hpp"

namespace nickel {

struct GLTFImportData;
class SkeletonImpl;

class Skeleton: public ImplWrapper<SkeletonImpl> {
public:
    using ImplWrapper::ImplWrapper;
    
    void UpdateTransformByRoot(const Transform&);
};

class SkeletonManagerImpl;

class SkeletonManager {
public:
    SkeletonManager();
    ~SkeletonManager();

    bool Load(const GLTFImportData&, const GLTFLoadConfig& = {});
    Skeleton Find(const std::string& name);
    void GC();
    void Clear();

private:
    std::unique_ptr<SkeletonManagerImpl> m_impl;
};

}