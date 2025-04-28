#pragma once
#include <unordered_set>

namespace nickel {

struct GLTFLoadConfig {
    bool m_combine_meshes = true;
    bool m_load_mesh = true;
    bool m_load_skeleton = true;
    std::unordered_set<std::string> m_skeletons;
    std::unordered_set<std::string> m_meshes;

    bool ShouldCombineMeshes() const { return m_combine_meshes; }
    bool ShouldLoadAllSkeleton() const { return m_skeletons.empty(); }
    bool ShouldLoadAllMeshes() const { return m_meshes.empty(); }
    bool ShouldLoadMesh() const { return m_load_mesh; }
    bool ShouldLoadSkeleton() const { return m_load_skeleton; }
};

}  // namespace nickel