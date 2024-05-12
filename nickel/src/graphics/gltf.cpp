#include "graphics/gltf.hpp"

#include "graphics/rhi/rhi.hpp"
#include "stb_image.h"
#define TINYGLTF_IMPLEMENTATION
#include "tiny_gltf.h"

namespace nickel {

template <typename SrcT, typename DstT>
void ConvertRangeData(const SrcT* src, DstT* dst, size_t blockCount,
                      size_t elemCount, size_t stride) {
    static_assert(std::is_convertible_v<SrcT, DstT>);
    size_t eCount = 0;
    while (blockCount > 0) {
        if (eCount < elemCount) {
            *(dst++) = *(src++);
        } else if (eCount < stride) {
            src++;
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
    bufView.offset = dst.size();
    bufView.size = size;
    bufView.count = accessor.count;

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
    }

    return bufView;
}

inline rhi::Filter GLTFFilter2RHI(int type) {
    switch (type) {
        case TINYGLTF_TEXTURE_FILTER_LINEAR:
        // TODO: support mipmap;
        case TINYGLTF_TEXTURE_FILTER_LINEAR_MIPMAP_LINEAR:
        case TINYGLTF_TEXTURE_FILTER_LINEAR_MIPMAP_NEAREST:
            return rhi::Filter::Linear;
        case TINYGLTF_TEXTURE_FILTER_NEAREST:
        // TODO: support mipmap;
        case TINYGLTF_TEXTURE_FILTER_NEAREST_MIPMAP_NEAREST:
        case TINYGLTF_TEXTURE_FILTER_NEAREST_MIPMAP_LINEAR:
            return rhi::Filter::Nearest;
    }
    return rhi::Filter::Linear;
}

inline rhi::SamplerAddressMode GLTFWrapper2RHI(int type) {
    switch (type) {
        case TINYGLTF_TEXTURE_WRAP_REPEAT:
            return rhi::SamplerAddressMode::Repeat;
        case TINYGLTF_TEXTURE_WRAP_MIRRORED_REPEAT:
            return rhi::SamplerAddressMode::MirrorRepeat;
        case TINYGLTF_TEXTURE_WRAP_CLAMP_TO_EDGE:
            return rhi::SamplerAddressMode::ClampToEdge;
    }
    return rhi::SamplerAddressMode::ClampToEdge;
}

inline std::filesystem::path ParseURI2Path(std::string_view str) {
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

struct GLTFLoader {
    GLTFModel Load(const std::filesystem::path& filename, rhi::Adapter adapter,
                   rhi::Device device, RenderContext& ctx) {
        auto textureMgr = ECS::Instance().World().res_mut<TextureManager>();
        return loadGLTF(filename, adapter, device, textureMgr.get(), ctx);
    }

private:
    struct MeshData {
        std::vector<unsigned char> positions;
        std::vector<unsigned char> normals;
        std::vector<unsigned char> uvs;
        std::vector<unsigned char> indices;
        std::vector<unsigned char> tangents;
    };

    GLTFModel loadGLTF(const std::filesystem::path& filename,
                       rhi::Adapter adapter, rhi::Device device,
                       TextureManager& mgr, RenderContext& ctx) {
        tinygltf::TinyGLTF loader;
        std::string err, warn;
        if (!loader.LoadASCIIFromFile(&model_, &err, &warn,
                                      filename.string())) {
            LOGW(nickel::log_tag::Asset, "load model from ", filename,
                 " failed:\n\terr:", err, "\n\twarn:", warn);
            return {};
        }

        auto rootDir = filename.parent_path();

        std::vector<TextureHandle> imageHandles(model_.images.size());
        std::vector<std::unique_ptr<Material3D>> materials;
        rhi::Buffer pbrParamsBuffer;

        for (int i = 0; i < model_.images.size(); i++) {
            auto& image = model_.images[i];
            imageHandles[i] = mgr.Load(rootDir / ParseURI2Path(image.uri));
        }

        bool supportSeparateSampler = adapter.Limits().supportSeparateSampler;

        std::vector<rhi::Sampler> samplers;
        if (supportSeparateSampler) {
            for (auto& sampler : model_.samplers) {
                samplers.emplace_back(createSampler(device, sampler));
            }
        }

        std::vector<unsigned char> pbrParams;
        for (auto& mtl : model_.materials) {
            Material3D material;
            auto align = adapter.Limits().minUniformBufferOffsetAlignment;
            material.pbrParameters.count = 6;
            material.pbrParameters.offset =
                std::ceil(pbrParams.size() / (float)align) * align;
            material.pbrParameters.size = sizeof(PBRParameters);
            pbrParams.resize(material.pbrParameters.offset +
                             material.pbrParameters.size);

            PBRParameters pbrParam;

            auto& colorFactor = mtl.pbrMetallicRoughness.baseColorFactor;
            pbrParam.baseColor.Set(colorFactor[0], colorFactor[1],
                                   colorFactor[2], colorFactor[3]);

            pbrParam.metalness = mtl.pbrMetallicRoughness.metallicFactor;
            pbrParam.roughness = mtl.pbrMetallicRoughness.roughnessFactor;

            memcpy(pbrParams.data() + material.pbrParameters.offset, &pbrParam,
                   sizeof(PBRParameters));

            uint32_t baseColorTextureIdx =
                mtl.pbrMetallicRoughness.baseColorTexture.index;
            if (baseColorTextureIdx != -1) {
                Material3D::TextureInfo baseTextureInfo;
                auto& info = model_.textures[baseColorTextureIdx];
                if (info.source != -1) {
                    baseTextureInfo.texture = imageHandles[info.source];
                    if (info.sampler != -1) {
                        baseTextureInfo.sampler = info.sampler;
                    }
                }
                material.basicTexture = baseTextureInfo;
            }

            if (auto idx =
                    mtl.pbrMetallicRoughness.metallicRoughnessTexture.index;
                idx != -1) {
                material.metalicRoughnessTexture =
                    parseTextureInfo(device, idx, imageHandles, samplers,
                                     supportSeparateSampler);
            }

            if (auto idx = mtl.normalTexture.index; idx != -1) {
                material.normalTexture =
                    parseTextureInfo(device, idx, imageHandles, samplers,
                                     supportSeparateSampler);
            }

            if (auto idx = mtl.occlusionTexture.index; idx != -1) {
                material.occlusionTexture =
                    parseTextureInfo(device, idx, imageHandles, samplers,
                                     supportSeparateSampler);
            }

            materials.emplace_back(
                std::make_unique<Material3D>(std::move(material)));
        }

        pbrParamsBuffer =
            copyBuffer2GPU(device, pbrParams, rhi::BufferUsage::Uniform);

        for (auto& material : materials) {
            material->bindGroup =
                createBindGroup(device, ctx, pbrParamsBuffer, mgr,
                                supportSeparateSampler, samplers, *material);
        }

        std::vector<Scene> scenes;
        for (auto& scene : model_.scenes) {
            for (auto& n : scene.nodes) {
                auto node = std::make_unique<Node>();
                preorderNodes(adapter, device, ctx, model_.nodes[n], model_,
                              *node);
                Scene scene{std::move(node)};
                scenes.emplace_back(std::move(scene));
            }
        }

        return {std::move(scenes), std::move(samplers), std::move(materials),
                std::move(pbrParamsBuffer)};
    }

    Material3D::TextureInfo parseTextureInfo(
        rhi::Device device, int idx,
        const std::vector<TextureHandle>& imageHandles,
        std::vector<rhi::Sampler>& samplers, bool supportSeparateSampler) {
        Material3D::TextureInfo textureInfo;
        auto& info = model_.textures[idx];
        if (info.source != -1) {
            textureInfo.texture = imageHandles[info.source];
            if (supportSeparateSampler) {
                if (info.sampler != -1) {
                    textureInfo.sampler = info.sampler;
                }
            } else {
                if (info.sampler != -1) {
                    samplers.emplace_back(
                        createSampler(device, model_.samplers[info.sampler]));
                } else {
                    samplers.emplace_back(device.CreateSampler({}));
                }
                textureInfo.sampler = samplers.size() - 1;
            }
        }
        return textureInfo;
    }

    rhi::Buffer copyBuffer2GPU(rhi::Device device,
                               const std::vector<unsigned char>& src,
                               rhi::BufferUsage usage) {
        rhi::Buffer::Descriptor desc;
        desc.usage = usage;
        desc.mappedAtCreation = true;
        desc.size = src.size();
        auto buffer = device.CreateBuffer(desc);
        memcpy(buffer.GetMappedRange(), src.data(), desc.size);
        buffer.Unmap();
        return buffer;
    }

    rhi::Buffer copyBuffer2GPU(rhi::Device device, const void* data,
                               size_t size, rhi::BufferUsage usage) {
        rhi::Buffer::Descriptor desc;
        desc.usage = usage;
        desc.mappedAtCreation = true;
        desc.size = size;
        auto buffer = device.CreateBuffer(desc);
        memcpy(buffer.GetMappedRange(), data, desc.size);
        buffer.Unmap();
        return buffer;
    }

    rhi::Sampler createSampler(rhi::Device device,
                               const tinygltf::Sampler& gltfSampler) {
        rhi::Sampler::Descriptor desc;
        desc.min = GLTFFilter2RHI(gltfSampler.minFilter);
        desc.mag = GLTFFilter2RHI(gltfSampler.magFilter);
        desc.u = GLTFWrapper2RHI(gltfSampler.wrapS);
        desc.v = GLTFWrapper2RHI(gltfSampler.wrapT);
        return device.CreateSampler(desc);
    }

    rhi::BindGroup createBindGroup(rhi::Device device, RenderContext& ctx,
                                   rhi::Buffer pbrParamsBuffer,
                                   TextureManager& mgr,
                                   bool supportSeparateSampler,
                                   const std::vector<rhi::Sampler> samplers,
                                   const Material3D& material) {
        rhi::BindGroup::Descriptor desc;
        desc.layout = ctx.ctx3D->bindGroupLayout;

        // pbr parameters uniform buffer
        {
            rhi::BindingPoint bufferBinding;
            rhi::BufferBinding entry;
            entry.hasDynamicOffset = true;
            entry.type = rhi::BufferType::Uniform;
            entry.minBindingSize = sizeof(PBRParameters);
            entry.buffer = pbrParamsBuffer;
            bufferBinding.binding = 1;
            bufferBinding.entry = entry;
            desc.entries.push_back(bufferBinding);
        }

        // color texture
        if (material.basicTexture) {
            if (supportSeparateSampler) {
                pushTextureBindingPoint(desc, mgr,
                                        material.basicTexture.value(), 2,
                                        "baseColorTexture");
            }
            if (material.basicTexture->sampler) {
                pushSamplerBindingPoint(desc, supportSeparateSampler, samplers,
                                        mgr, material.basicTexture.value(), 6,
                                        "baseColorSampler");
            }
        }

        // normal texture
        if (material.normalTexture) {
            if (supportSeparateSampler) {
                pushTextureBindingPoint(desc, mgr,
                                        material.normalTexture.value(), 3,
                                        "normalMapTexture");
            }
            if (material.normalTexture->sampler) {
                pushSamplerBindingPoint(desc, supportSeparateSampler, samplers,
                                        mgr, material.normalTexture.value(), 7,
                                        "normalMapSampler");
            }
        }

        // metalicRoughtness texture
        if (material.metalicRoughnessTexture) {
            if (supportSeparateSampler) {
                pushTextureBindingPoint(
                    desc, mgr, material.metalicRoughnessTexture.value(), 4,
                    "metalicRoughnessTexture");
            }
            if (material.metalicRoughnessTexture->sampler) {
                pushSamplerBindingPoint(
                    desc, supportSeparateSampler, samplers, mgr,
                    material.metalicRoughnessTexture.value(), 8,
                    "metalicRoughnessSampler");
            }
        }

        // occlusion texture
        if (material.occlusionTexture) {
            if (supportSeparateSampler) {
                pushTextureBindingPoint(desc, mgr,
                                        material.occlusionTexture.value(), 5,
                                        "occlusionTexture");
            }
            if (material.occlusionTexture->sampler) {
                pushSamplerBindingPoint(desc, supportSeparateSampler, samplers,
                                        mgr, material.occlusionTexture.value(),
                                        9, "occlusionSampler");
            }
        }

        return device.CreateBindGroup(desc);
    }

    void pushTextureBindingPoint(rhi::BindGroup::Descriptor& desc,
                                 const TextureManager& mgr,
                                 const Material3D::TextureInfo& info,
                                 uint32_t slot, const std::string& name) {
        rhi::BindingPoint bindingPoint;
        bindingPoint.binding = slot;
        rhi::TextureBinding binding;
        binding.name = name;
        binding.view = mgr.Get(info.texture).View();
        bindingPoint.entry = binding;
        desc.entries.push_back(bindingPoint);
    }

    void pushSamplerBindingPoint(rhi::BindGroup::Descriptor& desc,
                                 bool supportSeparateSampler,
                                 const std::vector<rhi::Sampler>& samplers,
                                 const TextureManager& mgr,
                                 const Material3D::TextureInfo& info,
                                 uint32_t slot, const std::string& name) {
        rhi::BindingPoint bindingPoint;
        bindingPoint.binding = slot;
        rhi::SamplerBinding binding;
        binding.name = name;
        binding.sampler = samplers[info.sampler.value()];
        if (!supportSeparateSampler) {
            binding.view = mgr.Get(info.texture).View();
        }
        bindingPoint.entry = binding;
        desc.entries.push_back(bindingPoint);
    }

    void preorderNodes(rhi::Adapter adapter, rhi::Device device,
                       RenderContext& ctx, const tinygltf::Node& node,
                       const tinygltf::Model& model, Node& parent) {
        std::unique_ptr<Node> newNode = std::make_unique<Node>();
        newNode->localModelMat = calcNodeTransform(node);

        GPUMesh mesh;
        MeshData data;

        if (node.mesh != -1) {
            auto& gltfMesh = model_.meshes[node.mesh];
            for (uint32_t i = 0; i < gltfMesh.primitives.size(); i++) {
                auto prim = recordPrimInfo(adapter, device, ctx, model,
                                           gltfMesh.primitives[i], data);
                mesh.primitives.emplace_back(prim);
            }
        }

        if (!data.positions.empty()) {
            mesh.posBuf = copyBuffer2GPU(device, data.positions,
                                         rhi::BufferUsage::Vertex);
            if (!data.indices.empty()) {
                mesh.indicesBuf = copyBuffer2GPU(device, data.indices,
                                                 rhi::BufferUsage::Index);
            }
            mesh.uvBuf =
                copyBuffer2GPU(device, data.uvs, rhi::BufferUsage::Vertex);
            mesh.normBuf =
                copyBuffer2GPU(device, data.normals, rhi::BufferUsage::Vertex);
            mesh.tanBuf =
                copyBuffer2GPU(device, data.tangents, rhi::BufferUsage::Vertex);
        }

        newNode->mesh = std::move(mesh);
        parent.children.emplace_back(std::move(newNode));

        for (auto child : node.children) {
            preorderNodes(adapter, device, ctx, model.nodes[child], model,
                          *parent.children.back());
        }
    }

    Primitive recordPrimInfo(rhi::Adapter adapter, rhi::Device dev,
                             RenderContext& ctx, const tinygltf::Model& model,
                             const tinygltf::Primitive& prim, MeshData& data) {
        Primitive primitive;

        auto& attrs = prim.attributes;
        uint32_t positionCount = 0;
        if (auto it = attrs.find("POSITION"); it != attrs.end()) {
            auto& accessor = model_.accessors[it->second];
            positionCount = accessor.count;
            primitive.posBufView = CopyBufferFromGLTF<float>(
                data.positions, TINYGLTF_TYPE_VEC3, accessor, model_);
        }

        std::optional<uint32_t> indicesCount;
        if (prim.indices != -1) {
            auto& accessor = model_.accessors[prim.indices];
            primitive.indicesBufView = CopyBufferFromGLTF<uint32_t>(
                data.indices, TINYGLTF_TYPE_SCALAR, accessor, model_);
            indicesCount = accessor.count;
        }

        if (auto it = attrs.find("TEXCOORD_0"); it != attrs.end()) {
            auto& accessor = model_.accessors[it->second];

            primitive.uvBufView = CopyBufferFromGLTF<float>(
                data.uvs, TINYGLTF_TYPE_VEC2, accessor, model_);
        } else {
            BufferView view;
            view.count = indicesCount ? indicesCount.value() : positionCount;
            view.size = view.count * 4 * 2;
            view.offset = data.uvs.size();
            data.uvs.resize(data.uvs.size() + view.size, 0);
            primitive.uvBufView = view;
        }

        if (auto it = attrs.find("NORMAL"); it != attrs.end()) {
            auto& accessor = model_.accessors[it->second];

            primitive.normBufView = CopyBufferFromGLTF<float>(
                data.normals, TINYGLTF_TYPE_VEC3, accessor, model_);
        } else {
            BufferView view;
            view.count = indicesCount.value_or(primitive.posBufView.count);
            view.size = view.count * 4 * 3;
            view.offset = data.normals.size();
            primitive.normBufView = view;

            nickel::cgmath::Vec3 v;
            data.normals.resize(data.normals.size() + view.size, 0);
            auto normPtr =
                (nickel::cgmath::Vec3*)(data.normals.data() +
                                        primitive.normBufView.offset);
            auto posPtr =
                (const nickel::cgmath::Vec3*)(data.positions.data() +
                                              primitive.posBufView.offset);
            if (indicesCount) {
                auto indicesPtr =
                    (const uint32_t*)(data.indices.data() +
                                      primitive.indicesBufView.offset);
                for (int i = 0; i < primitive.indicesBufView.count / 3; i++) {
                    auto idx1 = indicesPtr[i * 3];
                    auto idx2 = indicesPtr[i * 3 + 1];
                    auto idx3 = indicesPtr[i * 3 + 2];

                    auto pos1 = posPtr[idx1];
                    auto pos2 = posPtr[idx2];
                    auto pos3 = posPtr[idx3];
                    auto normal = nickel::cgmath::Normalize(
                        (pos2 - pos1).Cross(pos3 - pos1));
                    normPtr[idx1] = normal;
                    normPtr[idx2] = normal;
                    normPtr[idx3] = normal;
                }
            } else {
                for (int i = 0; i < primitive.posBufView.count / 3; i++) {
                    auto pos1 = posPtr[i * 3];
                    auto pos2 = posPtr[i * 3 + 1];
                    auto pos3 = posPtr[i * 3 + 2];
                    auto normal = nickel::cgmath::Normalize(
                        (pos2 - pos1).Cross(pos3 - pos1));
                    normPtr[i * 3] = normal;
                    normPtr[i * 3 + 1] = normal;
                    normPtr[i * 3 + 2] = normal;
                }
            }
        }

        if (auto it = attrs.find("TANGENT"); it != attrs.end()) {
            auto& accessor = model_.accessors[it->second];

            primitive.tanBufView = CopyBufferFromGLTF<float>(
                data.tangents, TINYGLTF_TYPE_VEC4, accessor, model_);
        } else {
            BufferView view;
            view.count = indicesCount.value_or(primitive.posBufView.count);
            view.size = view.count * 4 * 4;
            view.offset = data.tangents.size();
            data.tangents.resize(data.tangents.size() + view.size);
            primitive.tanBufView = view;

            auto posPtr =
                (const nickel::cgmath::Vec3*)(data.positions.data() +
                                              primitive.posBufView.offset);
            auto uvPtr =
                (const nickel::cgmath::Vec2*)(data.uvs.data() +
                                              primitive.uvBufView.offset);
            auto tanPtr = (nickel::cgmath::Vec4*)(data.tangents.data() +
                                                  primitive.tanBufView.offset);
            if (indicesCount) {
                auto indicesPtr =
                    (const uint32_t*)(data.indices.data() +
                                      primitive.indicesBufView.offset);
                Assert(primitive.indicesBufView.count % 3 == 0,
                       "indices can't be triangle list");
                for (int i = 0; i < primitive.indicesBufView.count / 3; i++) {
                    uint32_t idx1 = *(indicesPtr + i * 3);
                    uint32_t idx2 = *(indicesPtr + i * 3 + 1);
                    uint32_t idx3 = *(indicesPtr + i * 3 + 2);

                    auto uv1 = *(uvPtr + idx1);
                    auto uv2 = *(uvPtr + idx2);
                    auto uv3 = *(uvPtr + idx3);

                    auto pos1 = *(posPtr + idx1);
                    auto pos2 = *(posPtr + idx2);
                    auto pos3 = *(posPtr + idx3);

                    nickel::cgmath::Vec4 tangent;

                    if (uv1 == nickel::cgmath::Vec2{} &&
                        uv2 == nickel::cgmath::Vec2{} &&
                        uv3 == nickel::cgmath::Vec2{}) {
                        tangent.Set(nickel::cgmath::Normalize(pos2 - pos1));
                        tangent.w = 1;
                    } else {
                        auto [tan, _] = nickel::cgmath::GetNormalMapTB(
                            pos1, pos2, pos3, uv1, uv2, uv3);

                        tangent = nickel::cgmath::Vec4{tan.x, tan.y, tan.z, 1};
                    }
                    *(tanPtr + idx1) = tangent;
                    *(tanPtr + idx2) = tangent;
                    *(tanPtr + idx3) = tangent;
                }
            } else {
                // TODO:
            }
        }

        primitive.material = prim.material;

        return primitive;
    }

    nickel::cgmath::Mat44 calcNodeTransform(const tinygltf::Node& node) {
        auto cvtMat = [](const std::vector<double>& datas) {
            nickel::cgmath::Mat44 mat;
            for (int i = 0; i < datas.size(); i++) {
                mat.data[i] = datas[i];
            }
            return mat;
        };

        auto m = nickel::cgmath::Mat44::Identity();
        if (!node.matrix.empty()) {
            m = cvtMat(node.matrix);
        } else if (!node.scale.empty() || !node.translation.empty() ||
                   !node.rotation.empty()) {
            m = nickel::cgmath::Mat44::Identity();
            if (!node.scale.empty()) {
                m = nickel::cgmath::CreateScale(nickel::cgmath::Vec3(
                        node.scale[0], node.scale[1], node.scale[2])) *
                    m;
            }
            if (!node.rotation.empty()) {
                m = nickel::cgmath::Quat(node.rotation[0], node.rotation[1],
                                         node.rotation[2], node.rotation[3])
                        .ToMat() *
                    m;
            }
            if (!node.translation.empty()) {
                m = nickel::cgmath::CreateTranslation(nickel::cgmath::Vec3(
                        node.translation[0], node.translation[1],
                        node.translation[2])) *
                    m;
            }
        }

        return m;
    }

    tinygltf::Model model_;
};

GLTFModel::GLTFModel(std::vector<Scene>&& scenes,
                     std::vector<rhi::Sampler>&& samplers,
                     std::vector<std::unique_ptr<Material3D>>&& materials,
                     rhi::Buffer buffer)
    : scenes{std::move(scenes)},
      samplers{std::move(samplers)},
      materials{std::move(materials)},
      pbrParamBuffer{std::move(buffer)},
      valid_{true} {}

GLTFModel::~GLTFModel() {
    for (auto sampler : samplers) {
        sampler.Destroy();
    }
    pbrParamBuffer.Destroy();
}

GLTFModel GLTFModel::Null;

toml::table GLTFModel::Save2Toml() const {
    toml::table tbl;
    tbl.emplace("path", RelativePath().string());
    return tbl;
}

GLTFModel::operator bool() const {
    return valid_;
}

template <>
std::unique_ptr<GLTFModel> LoadAssetFromMetaTable(const toml::table& tbl) {
    if (auto node = tbl.get("path"); node && node->is_string()) {
        GLTFLoader loader;
        auto& world = ECS::Instance().World();
        auto model = loader.Load(node->as_string()->get(),
                                 world.res<rhi::Adapter>().get(),
                                 world.res<rhi::Device>().get(),
                                 world.res_mut<RenderContext>().get());
        return std::make_unique<GLTFModel>(std::move(model));
    } else {
        return {};
    }
}

GLTFHandle GLTFManager::Load(const std::filesystem::path& filename) {
    GLTFLoader loader;
    auto& world = ECS::Instance().World();
    auto node = loader.Load(filename, world.res<rhi::Adapter>().get(),
                            world.res<rhi::Device>().get(),
                            world.res_mut<RenderContext>().get());
    auto handle = GLTFHandle::Create();
    storeNewItem(handle, std::make_unique<GLTFModel>(std::move(node)));
    return handle;
}

}  // namespace nickel