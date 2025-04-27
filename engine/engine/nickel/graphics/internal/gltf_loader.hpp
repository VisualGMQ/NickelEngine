#pragma once
#include "nickel/graphics/gltf.hpp"
#include "nickel/graphics/internal/material3d_impl.hpp"
#include "nickel/graphics/mesh.hpp"
#include "nickel/graphics/texture_manager.hpp"
#include "nickel/nickel.hpp"
#include "tiny_gltf.h"

namespace nickel::graphics {
class GLTFRenderPass;

template <typename SrcT, typename DstT>
void ConvertRangeData(const unsigned char* src, DstT* dst, size_t blockCount,
                      size_t elemCount, size_t stride) {
    static_assert(std::is_convertible_v<SrcT, DstT>);
    size_t eCount = 0;
    const unsigned char* src_start_ptr = src;
    while (blockCount > 0) {
        if (eCount < elemCount) {
            *(dst++) = *(SrcT*)src;
            src += sizeof(SrcT);
            eCount++;
        } else {
            blockCount--;
            eCount = 0;
            src = src_start_ptr + stride;
            src_start_ptr = src;
        }
    }
}

/**
 * @brief Copy gltf buffer to custom buffer. It will convert data from
 * `accessor.type` to `type`, and for single element in type, copy
 * `accessor.componentType` to RequireT
 *
 * @tparam ComponentType the component type you want
 * @param dst copy destination
 * @param type TINYGLTF_TYPE_XXX, the destination type you want
 * @param accessor
 * @param model model
 */
template <typename ComponentType>
BufferView CopyBufferFromGLTF(std::vector<unsigned char>& dst, int type,
                              const tinygltf::Accessor& accessor,
                              const tinygltf::Model& model) {
    auto& view = model.bufferViews[accessor.bufferView];
    auto& buffer = model.buffers[view.buffer];
    auto src_offset = accessor.byteOffset + view.byteOffset;
    auto size = accessor.count * sizeof(ComponentType) *
                tinygltf::GetNumComponentsInType(type);
    BufferView bufView;
    bufView.m_offset = dst.size();
    bufView.m_size = size;
    bufView.m_count = accessor.count;

    dst.resize(dst.size() + size);
    auto copySrc = buffer.data.data() + src_offset;
    auto dstSrc = dst.data() + dst.size() - size;
    auto srcComponentNum = tinygltf::GetNumComponentsInType(accessor.type);
    size_t stride = view.byteStride == 0
                        ? srcComponentNum * tinygltf::GetComponentSizeInBytes(
                                                accessor.componentType)
                        : view.byteStride;
    auto dstComponentNum = tinygltf::GetNumComponentsInType(type);
    switch (accessor.componentType) {
        case TINYGLTF_COMPONENT_TYPE_FLOAT:
            ConvertRangeData<float>(copySrc, (ComponentType*)dstSrc,
                                    accessor.count, dstComponentNum, stride);
            break;
        case TINYGLTF_COMPONENT_TYPE_DOUBLE:
            ConvertRangeData<double>(copySrc, (ComponentType*)dstSrc,
                                     accessor.count, dstComponentNum, stride);
            break;
        case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:
            ConvertRangeData<unsigned int>(copySrc, (ComponentType*)dstSrc,
                                           accessor.count, dstComponentNum,
                                           stride);
            break;
        case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
            ConvertRangeData<unsigned short>(copySrc, (ComponentType*)dstSrc,
                                             accessor.count, dstComponentNum,
                                             stride);
            break;
        case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:
            ConvertRangeData<unsigned char>(copySrc, (ComponentType*)dstSrc,
                                            accessor.count, dstComponentNum,
                                            stride);
            break;
        case TINYGLTF_COMPONENT_TYPE_INT:
            ConvertRangeData<int>(copySrc, (ComponentType*)dstSrc,
                                  accessor.count, dstComponentNum, stride);
            break;
        case TINYGLTF_COMPONENT_TYPE_SHORT:
            ConvertRangeData<short>(copySrc, (ComponentType*)dstSrc,
                                    accessor.count, dstComponentNum, stride);
            break;
        case TINYGLTF_COMPONENT_TYPE_BYTE:
            ConvertRangeData<char>(copySrc, (ComponentType*)dstSrc,
                                   accessor.count, dstComponentNum, stride);
            break;
        default:
            NICKEL_CANT_REACH();
    }

    return bufView;
}

template <typename RequireT>
BufferView RecordBufferView(const tinygltf::Model& model,
                            const tinygltf::Accessor& accessor,
                            std::vector<unsigned char>& buffer, int type) {
    BufferView view;
    view = CopyBufferFromGLTF<RequireT>(buffer, type, accessor, model);
    return view;
}

inline Filter GLTFFilter2RHI(int type) {
    switch (type) {
        case TINYGLTF_TEXTURE_FILTER_LINEAR:
        // TODO: support mipmap;
        case TINYGLTF_TEXTURE_FILTER_LINEAR_MIPMAP_LINEAR:
        case TINYGLTF_TEXTURE_FILTER_LINEAR_MIPMAP_NEAREST:
            return Filter::Linear;
        case TINYGLTF_TEXTURE_FILTER_NEAREST:
        // TODO: support mipmap;
        case TINYGLTF_TEXTURE_FILTER_NEAREST_MIPMAP_NEAREST:
        case TINYGLTF_TEXTURE_FILTER_NEAREST_MIPMAP_LINEAR:
            return Filter::Nearest;
    }
    return Filter::Linear;
}

inline SamplerAddressMode GLTFWrapper2RHI(int type) {
    switch (type) {
        case TINYGLTF_TEXTURE_WRAP_REPEAT:
            return SamplerAddressMode::Repeat;
        case TINYGLTF_TEXTURE_WRAP_MIRRORED_REPEAT:
            return SamplerAddressMode::MirrorRepeat;
        case TINYGLTF_TEXTURE_WRAP_CLAMP_TO_EDGE:
            return SamplerAddressMode::ClampToEdge;
        default:
            return SamplerAddressMode::ClampToEdge;
    }
}

inline std::string ParseURI2Path(std::string_view str) {
    std::string path;
    path.reserve(str.size());
    int idx = 0;
    while (idx < str.size()) {
        if (str[idx] == '%') {
            if (str[idx + 1] == '2' && str[idx + 2] == '0') {
                path.push_back(' ');
                idx += 3;
            }
            // TODO?: other encode parse
        } else {
            path.push_back(str[idx++]);
        }
    }
    return path;
}

inline Transform CalcNodeTransform(const tinygltf::Node& node) {
    auto cvtMat = [](const std::vector<double>& datas) {
        Mat44 mat;
        for (int i = 0; i < datas.size(); i++) {
            mat.Ptr()[i] = datas[i];
        }
        return mat;
    };

    if (!node.matrix.empty()) {
        auto m = cvtMat(node.matrix);
        return Transform::FromMat(m);
    }

    Transform transform;
    if (!node.scale.empty()) {
        transform.scale = Vec3(node.scale[0], node.scale[1], node.scale[2]);
    }
    if (!node.rotation.empty()) {
        transform.q = Quat(node.rotation[0], node.rotation[1],
                           node.rotation[2], node.rotation[3]);
    }
    if (!node.translation.empty()) {
        transform.p = Vec3(node.translation[0], node.translation[1],
                           node.translation[2]);
    }
    return transform;
}

struct Node {
    enum class Flag {
        None = 0x00,
        HasMesh = 0x01,
        HasBone = 0x02, 
    };
    
    std::string m_name;
    Flags<Flag> m_flags = Flag::None;
    Transform m_local_transform;
    Transform m_global_transform;
    std::vector<uint32_t> m_children;

    std::optional<uint32_t> m_mesh;

    bool HasMesh() const { return m_flags & Flag::HasMesh; }
    bool HasBone() const { return m_flags & Flag::HasBone; }
};

struct Skin {
    std::string m_name;
    uint32_t m_root;
    std::set<uint32_t> m_bone_indices;
};

struct GLTFLoadData {
    Path m_filename;
    
    std::vector<Node> m_nodes;
    std::vector<uint32_t> m_root_nodes;

    GLTFModelResource m_resource;
    std::vector<Mesh> m_meshes;

    std::vector<Skin> m_skins;
};

class GLTFLoader {
public:
    explicit GLTFLoader(const tinygltf::Model& model);
    GLTFLoadData Load(const Path& filename, const Adapter& adapter,
                      GLTFManagerImpl& mgr);

private:
    const tinygltf::Model& m_gltf_model;

    GLTFLoadData loadGLTF(const Path& filename, const Adapter& adapter,
                          GLTFManagerImpl& gltf_manager,
                          GLTFRenderPass& render_pass,
                          TextureManager& texture_mgr,
                          CommonResource& common_res);

    Material3D::TextureInfo parseTextureInfo(int idx,
                                             std::vector<Texture>& textures,
                                             ImageView& default_texture,
                                             std::vector<Sampler>& samplers,
                                             Sampler& default_sampler);

    Sampler createSampler(Device device, const tinygltf::Sampler& gltfSampler);

    Mesh createMesh(const tinygltf::Mesh& gltf_mesh, GLTFManagerImpl* mgr,
                    std::vector<unsigned char>& vertex_buffer,
                    std::vector<unsigned char>& indices_buffer,
                    const std::vector<BufferView>& accessors,
                    std::vector<Material3D>& materials,
                    Material3DImpl& default_material) const;

    Primitive recordPrimInfo(std::vector<unsigned char>& vertex_buffer,
                             std::vector<unsigned char>& indices_buffer,
                             const std::vector<BufferView>& buffer_views,
                             const tinygltf::Primitive& prim,
                             std::vector<Material3D>& materials,
                             Material3DImpl& default_material) const;

    void analyzeAccessorUsage(std::set<uint32_t>& out_vertex_accessors,
                              std::set<uint32_t>& out_index_accessors,
                              size_t& out_vertex_buffer_size,
                              size_t& out_index_buffer_size);
    void analyzeImageUsage(std::set<uint32_t>& out_color_texture,
                           std::set<uint32_t>& out_normal_texture,
                           std::set<uint32_t>& out_occlusion_texture,
                           std::set<uint32_t>& out_metallic_roughness_texture,
                           std::set<uint32_t>& out_emissive_texture) const;

    std::vector<Texture> loadTextures(const Path& root_dir,
                                      TextureManager& texture_mgr,
                                      const std::set<uint32_t>& color_textures);
    std::vector<Sampler> loadSamplers(Device& device);
    std::vector<Material3D> loadMaterials(
        const Adapter& adapter, GLTFManagerImpl& gltf_mgr,
        GLTFRenderPass& render_pass, std::vector<PBRParameters>& pbr_parameters,
        std::vector<unsigned char>& data_buffer, std::vector<Texture>& textures,
        std::vector<Sampler>& samplers, CommonResource& common_res);
    std::vector<BufferView> loadVertexBuffer(
        std::vector<unsigned char>& out_vertex_buffer,
        std::vector<unsigned char>& out_index_buffer,
        const std::set<uint32_t>& vertex_accessor,
        const std::set<uint32_t>& index_accessor) const;

    /*
     * 1. calculate node global transform
     * 2. record wether node/subtree is a mesh node or bone node
     */
    Flags<Node::Flag> recordNodeInfoRecursive(std::vector<Node>& nodes, Node& node,
                                 Transform* parent_transform) const;
};

}  // namespace nickel::graphics