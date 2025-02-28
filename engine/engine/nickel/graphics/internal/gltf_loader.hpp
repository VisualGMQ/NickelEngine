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
void ConvertRangeData(const SrcT* src, DstT* dst, size_t blockCount,
                      size_t elemCount, size_t stride) {
    static_assert(std::is_convertible_v<SrcT, DstT>);
    size_t eCount = 0;
    while (blockCount > 0) {
        if (eCount < elemCount) {
            *(dst++) = *(src++);
        } else if (eCount < stride) {
            ++src;
        }
        eCount++;

        if (eCount >= stride) {
            blockCount--;
            eCount = 0;
        }
    }
}

template <typename RequireT>
BufferView CopyBufferFromGLTF(std::vector<unsigned char>& dst, int type,
                              const tinygltf::Accessor& accessor,
                              const tinygltf::Model& model) {
    auto& view = model.bufferViews[accessor.bufferView];
    auto& buffer = model.buffers[view.buffer];
    auto offset = accessor.byteOffset + view.byteOffset;
    auto size = accessor.count * sizeof(RequireT) *
                tinygltf::GetNumComponentsInType(type);
    BufferView bufView;
    bufView.m_offset = dst.size();
    bufView.m_size = size;
    bufView.m_count = accessor.count;

    dst.resize(dst.size() + size);
    auto copySrc = buffer.data.data() + offset;
    auto dstSrc = dst.data() + dst.size() - size;
    auto srcComponentNum = tinygltf::GetNumComponentsInType(accessor.type);
    auto dstComponentNum = tinygltf::GetNumComponentsInType(type);
    switch (accessor.componentType) {
        case TINYGLTF_COMPONENT_TYPE_FLOAT:
            ConvertRangeData((const float*)copySrc, (RequireT*)dstSrc,
                             accessor.count, dstComponentNum, srcComponentNum);
            break;
        case TINYGLTF_COMPONENT_TYPE_DOUBLE:
            ConvertRangeData((const double*)copySrc, (RequireT*)dstSrc,
                             accessor.count, dstComponentNum, srcComponentNum);
            break;
        case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:
            ConvertRangeData((const unsigned int*)copySrc, (RequireT*)dstSrc,
                             accessor.count, dstComponentNum, srcComponentNum);
            break;
        case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
            ConvertRangeData((const unsigned short*)copySrc, (RequireT*)dstSrc,
                             accessor.count, dstComponentNum, srcComponentNum);
            break;
        case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:
            ConvertRangeData((const unsigned char*)copySrc, (RequireT*)dstSrc,
                             accessor.count, dstComponentNum, srcComponentNum);
            break;
        case TINYGLTF_COMPONENT_TYPE_INT:
            ConvertRangeData((const int*)copySrc, (RequireT*)dstSrc,
                             accessor.count, dstComponentNum, srcComponentNum);
            break;
        case TINYGLTF_COMPONENT_TYPE_SHORT:
            ConvertRangeData((const short*)copySrc, (RequireT*)dstSrc,
                             accessor.count, dstComponentNum, srcComponentNum);
            break;
        case TINYGLTF_COMPONENT_TYPE_BYTE:
            ConvertRangeData((const char*)copySrc, (RequireT*)dstSrc,
                             accessor.count, dstComponentNum, srcComponentNum);
            break;
        default:
            NICKEL_CANT_REACH();
    }

    return bufView;
}

template <typename RequireT>
BufferView RecordBufferView(const tinygltf::Model& model,
                            const tinygltf::Accessor& accessor,
                            std::vector<unsigned char>& buffer,
                            uint32_t buffer_idx, int type) {
    BufferView view;
    view = CopyBufferFromGLTF<RequireT>(buffer, type, accessor, model);
    view.m_buffer = buffer_idx;
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

inline Mat44 CalcNodeTransform(const tinygltf::Node& node) {
    auto cvtMat = [](const std::vector<double>& datas) {
        Mat44 mat;
        for (int i = 0; i < datas.size(); i++) {
            mat.Ptr()[i] = datas[i];
        }
        return mat;
    };

    auto m = Mat44::Identity();
    if (!node.matrix.empty()) {
        m = cvtMat(node.matrix);
    } else if (!node.scale.empty() || !node.translation.empty() ||
               !node.rotation.empty()) {
        m = Mat44::Identity();
        if (!node.scale.empty()) {
            m = CreateScale(Vec3(node.scale[0], node.scale[1], node.scale[2])) *
                m;
        }
        if (!node.rotation.empty()) {
            m = Quat(node.rotation[0], node.rotation[1], node.rotation[2],
                     node.rotation[3])
                    .ToMat() *
                m;
        }
        if (!node.translation.empty()) {
            m = CreateTranslation(Vec3(node.translation[0], node.translation[1],
                                       node.translation[2])) *
                m;
        }
    }

    return m;
}

struct GLTFLoadData {
    GLTFModelResource m_resource;
    std::vector<Mesh> m_meshes;
};

class GLTFLoader {
public:
    explicit GLTFLoader(const tinygltf::Model& model);
    GLTFLoadData Load(const Path& filename, const Adapter& adapter,
                      GLTFManagerImpl& mgr);

private:
    const tinygltf::Model& m_gltf_model;

    GLTFLoadData loadGLTF(const Path& filename, const Adapter& adapter,
                          GLTFManagerImpl& mgr,
                          GLTFRenderPass& render_pass,
                          TextureManager& texture_mgr);

    Material3DImpl::TextureInfo parseTextureInfo(int idx,
                                                 GLTFModelResourceImpl& model);

    template <typename T>
    Buffer copyBuffer2GPU(Device device, std::span<T> src,
                          Flags<BufferUsage> usage) {
        Buffer::Descriptor desc;
        desc.m_usage = usage;
        desc.m_memory_type = MemoryType::GPULocal;
        desc.m_size = src.size();
        auto buffer = device.CreateBuffer(desc);
        buffer.BuffData((void*)src.data(), desc.m_size, 0);
        return buffer;
    }

    Sampler createSampler(Device device, const tinygltf::Sampler& gltfSampler);

    Mesh createMesh(Device device, const tinygltf::Mesh& node,
                    GLTFManagerImpl* mgr, GLTFModelResourceImpl& model);

    Primitive recordPrimInfo(std::vector<unsigned char>& data_buffer,
                             uint32_t buffer_idx,
                             const tinygltf::Primitive& prim);
};

}  // namespace nickel::graphics