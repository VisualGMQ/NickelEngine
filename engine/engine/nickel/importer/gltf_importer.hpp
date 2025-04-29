#pragma once
#include "nickel/graphics/gltf.hpp"
#include "nickel/graphics/mesh.hpp"

namespace nickel {

struct Node {
    enum class Flag {
        None = 0x00,
        InMeshTree = 0x01,  // node or child node has mesh
        HasBone = 0x02,     // node has bone
    };

    std::string m_name;
    Flags<Flag> m_flags = Flag::None;
    Transform m_local_transform;
    Transform m_global_transform;
    std::vector<uint32_t> m_children;

    std::optional<uint32_t> m_mesh;

    bool IsInMeshTree() const { return m_flags & Flag::InMeshTree; }

    bool HasBone() const { return m_flags & Flag::HasBone; }
};

struct Skin {
    std::string m_name;
    std::optional<uint32_t> m_root;
    std::vector<uint32_t> m_bone_indices;
    std::unordered_map<uint32_t, uint32_t>
        m_mapped_bone_indices;  // gltf bone indices to packed bone indices

    operator bool() const { return m_root && !m_bone_indices.empty(); }
};

struct GLTFImportData {
    Path m_filename;
    std::string m_asset_name;

    std::vector<Node> m_nodes;
    std::vector<uint32_t> m_root_nodes;

    graphics::GLTFModelResource m_resource;
    std::vector<graphics::Mesh> m_meshes;

    std::vector<Skin> m_skins;
};

GLTFImportData ImportGLTF(const Path& filename);
bool ImportGLTF2Engine(const Path& filename, const GLTFLoadConfig& config = {});

}  // namespace nickel