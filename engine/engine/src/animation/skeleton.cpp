#include "nickel/animation/skeleton.hpp"
#include "nickel/animation/internal/skeleton_impl.hpp"

namespace nickel {

void Skeleton::UpdateTransformByRoot(const Transform& transform) {
    m_impl->UpdateTransformByRoot(transform);
}

SkeletonManager::SkeletonManager()
    : m_impl{std::make_unique<SkeletonManagerImpl>()} {}

SkeletonManager::~SkeletonManager() {}

bool SkeletonManager::Load(const GLTFImportData& load_data, const GLTFLoadConfig& config) {
    return m_impl->Load(load_data, config);
}

Skeleton SkeletonManager::Find(const std::string& name) {
    return m_impl->Find(name);
}

void SkeletonManager::GC() {
    m_impl->GC();
}

void SkeletonManager::Clear() {
    m_impl->Clear();
}

}  // namespace nickel