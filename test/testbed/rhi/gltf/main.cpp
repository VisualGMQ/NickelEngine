#include "graphics/rhi/rhi.hpp"
#include "graphics/rhi/util.hpp"
#include "nickel.hpp"
#define TINYGLTF_IMPLEMENTATION
#include "stb_image.h"
#include "tiny_gltf.h"

using namespace nickel::rhi;

APIPreference API = APIPreference::GL;

struct BufferView {
    uint32_t offset{};
    uint64_t size{};
    uint32_t count{};
};

struct Material final {
    struct TextureInfo {
        uint32_t texture;
        uint32_t sampler;
    };

    BufferView basicColorFactor;
    std::optional<TextureInfo> basicTexture;
    std::optional<TextureInfo> normalTexture;
    BindGroup bindGroup;
};

struct TextureBundle {
    Texture texture;
    TextureView view;
};

struct Context final {
    PipelineLayout layout;
    RenderPipeline pipeline;
    Buffer uniformBuffer;
    Buffer colorBuffer;
    BindGroupLayout bindGroupLayout;
    Texture depth;
    TextureView depthView;
    std::vector<TextureBundle> images;
    std::vector<Sampler> samplers;

    // gltf related
    TextureBundle whiteTexture;
    Sampler whiteTextureSampler;

    TextureBundle specularTexture;
    Sampler normalTextureSampler;

    std::vector<Material> materials;

    ~Context() {
        layout.Destroy();
        pipeline.Destroy();
        uniformBuffer.Destroy();
        depth.Destroy();
        depthView.Destroy();
        for (auto elem : images) {
            elem.view.Destroy();
            elem.texture.Destroy();
        }
        for (auto elem : samplers) {
            elem.Destroy();
        }
        whiteTexture.view.Destroy();
        whiteTexture.texture.Destroy();
        whiteTextureSampler.Destroy();
        bindGroupLayout.Destroy();
    }
};

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
    }
    return SamplerAddressMode::ClampToEdge;
}

struct Primitive final {
    BufferView posBufView;
    BufferView normBufView;
    BufferView tanBufView;
    BufferView uvBufView;
    BufferView colorBufView;
    BufferView indicesBufView;
    uint32_t material;
};

struct GPUMesh final {
    Buffer posBuf;
    Buffer normBuf;
    Buffer tanBuf;
    Buffer uvBuf;
    Buffer indicesBuf;
    std::vector<Material> materials;

    std::vector<Primitive> primitives;

    operator bool() const {
        // at least we need position information
        return posBuf;
    }
};

struct Node final {
    nickel::cgmath::Mat44 transform = nickel::cgmath::Mat44::Identity();
    GPUMesh mesh;
    std::vector<std::unique_ptr<Node>> children;
};

struct Scene final {
    std::unique_ptr<Node> node;
};

struct GLTFNode final {
    std::vector<Scene> scenes;
};

struct GLTFLoader {
    GLTFNode Load(const std::filesystem::path& filename, Adapter adapter,
                  Device device, Context& ctx) {
        return GLTFNode{loadGLTF(filename, adapter, device, ctx)};
    }

private:
    struct MeshData {
        std::vector<unsigned char> positions;
        std::vector<unsigned char> normals;
        std::vector<unsigned char> uvs;
        std::vector<unsigned char> indices;
        std::vector<unsigned char> tangents;
    };

    std::vector<Scene> loadGLTF(const std::filesystem::path& filename,
                                Adapter adapter, Device device, Context& ctx) {
        tinygltf::TinyGLTF loader;
        std::string err, warn;
        if (!loader.LoadASCIIFromFile(&model_, &err, &warn,
                                      filename.string())) {
            LOGW(nickel::log_tag::Asset, "load model from ", filename,
                 " failed:\n\terr:", err, "\n\twarn:", warn);
        }

        auto rootDir = filename.parent_path();

        for (auto& image : model_.images) {
            auto [texture, view] = loadTexture(device, rootDir / image.uri);
            TextureBundle bundle;
            bundle.texture = texture;
            bundle.view = view;
            BindGroup::Descriptor desc;
            desc.layout = ctx.bindGroupLayout;
            ctx.images.emplace_back(bundle);
        }

        for (auto& sampler : model_.samplers) {
            auto spl = createSampler(device, sampler);
            ctx.samplers.emplace_back(spl);
        }

        std::vector<unsigned char> baseColor;
        for (auto& material : model_.materials) {
            Material mat;
            auto align = adapter.Limits().minUniformBufferOffsetAlignment;
            mat.basicColorFactor.count = 4;
            mat.basicColorFactor.offset =
                std::ceil(baseColor.size() / (float)align) * align;
            mat.basicColorFactor.size = sizeof(nickel::cgmath::Color);
            auto& colorFactor = material.pbrMetallicRoughness.baseColorFactor;
            baseColor.resize(mat.basicColorFactor.offset +
                             mat.basicColorFactor.size);
            nickel::cgmath::Color color(colorFactor[0], colorFactor[1],
                                        colorFactor[2], colorFactor[3]);
            memcpy(baseColor.data() + mat.basicColorFactor.offset, color.data,
                   sizeof(color));

            uint32_t baseColorTextureIdx =
                material.pbrMetallicRoughness.baseColorTexture.index;
            if (baseColorTextureIdx != -1) {
                Material::TextureInfo baseTextureInfo;
                auto& info = model_.textures[baseColorTextureIdx];
                if (info.source != -1) {
                    baseTextureInfo.texture = info.source;
                    if (info.sampler != -1) {
                        baseTextureInfo.sampler = info.sampler;
                    } else {
                        ctx.samplers.emplace_back(device.CreateSampler({}));
                        baseTextureInfo.sampler = ctx.samplers.size() - 1;
                    }
                }
                mat.basicTexture = baseTextureInfo;
            }

            uint32_t normalTextureIdx = material.normalTexture.index;
            if (normalTextureIdx != -1) {
                Material::TextureInfo normalTextureInfo;
                auto& info = model_.textures[normalTextureIdx];
                if (info.source != -1) {
                    normalTextureInfo.texture = info.source;
                    if (info.sampler != -1) {
                        normalTextureInfo.sampler = info.sampler;
                    } else {
                        ctx.samplers.emplace_back(device.CreateSampler({}));
                        normalTextureInfo.sampler = ctx.samplers.size() - 1;
                    }
                }
                mat.normalTexture = normalTextureInfo;
            }

            ctx.materials.emplace_back(mat);
        }
        ctx.colorBuffer =
            copyBuffer2GPU(device, baseColor, BufferUsage::Uniform);

        for (auto& material : ctx.materials) {
            material.bindGroup =
                createBindGroup(device, ctx, ctx.colorBuffer, material);
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

        return scenes;
    }

    Buffer copyBuffer2GPU(Device device, const std::vector<unsigned char>& src,
                          BufferUsage usage) {
        Buffer::Descriptor desc;
        desc.usage = usage;
        desc.mappedAtCreation = true;
        desc.size = src.size();
        auto buffer = device.CreateBuffer(desc);
        memcpy(buffer.GetMappedRange(), src.data(), desc.size);
        buffer.Unmap();
        return buffer;
    }

    Buffer copyBuffer2GPU(Device device, const void* data, size_t size,
                          BufferUsage usage) {
        Buffer::Descriptor desc;
        desc.usage = usage;
        desc.mappedAtCreation = true;
        desc.size = size;
        auto buffer = device.CreateBuffer(desc);
        memcpy(buffer.GetMappedRange(), data, desc.size);
        buffer.Unmap();
        return buffer;
    }

    std::tuple<Texture, TextureView> loadTexture(
        Device device, const std::filesystem::path& filename) {
        int w, h;
        void* data = stbi_load(filename.string().c_str(), &w, &h, nullptr,
                               STBI_rgb_alpha);

        Texture::Descriptor desc;
        desc.format = TextureFormat::RGBA8_UNORM;
        desc.size.width = w;
        desc.size.height = h;
        desc.size.depthOrArrayLayers = 1;
        desc.usage = static_cast<uint32_t>(TextureUsage::TextureBinding) |
                     static_cast<uint32_t>(TextureUsage::CopyDst);
        auto texture = device.CreateTexture(desc);

        Buffer::Descriptor bufferDesc;
        bufferDesc.mappedAtCreation = true;
        bufferDesc.usage = BufferUsage::CopySrc;
        bufferDesc.size = 4 * w * h;
        Buffer copyBuffer = device.CreateBuffer(bufferDesc);

        void* bufData = copyBuffer.GetMappedRange();
        memcpy(bufData, data, bufferDesc.size);
        copyBuffer.Unmap();

        auto encoder = device.CreateCommandEncoder();
        CommandEncoder::BufTexCopySrc src;
        src.buffer = copyBuffer;
        src.offset = 0;
        src.bytesPerRow = w;
        src.rowsPerImage = h;
        CommandEncoder::BufTexCopyDst dst;
        dst.texture = texture;
        dst.aspect = TextureAspect::All;
        dst.miplevel = 0;
        encoder.CopyBufferToTexture(src, dst,
                                    Extent3D{(uint32_t)w, (uint32_t)h, 1});
        auto buf = encoder.Finish();
        device.GetQueue().Submit({buf});
        device.WaitIdle();
        encoder.Destroy();

        auto view = texture.CreateView();

        stbi_image_free(data);
        copyBuffer.Destroy();

        return {texture, view};
    }

    Sampler createSampler(Device device, const tinygltf::Sampler& gltfSampler) {
        Sampler::Descriptor desc;
        desc.min = GLTFFilter2RHI(gltfSampler.minFilter);
        desc.mag = GLTFFilter2RHI(gltfSampler.magFilter);
        desc.u = GLTFWrapper2RHI(gltfSampler.wrapS);
        desc.v = GLTFWrapper2RHI(gltfSampler.wrapT);
        return device.CreateSampler(desc);
    }

    BindGroup createBindGroup(Device device, Context& ctx, Buffer colorBuf,
                              const Material& material) {
        BindGroup::Descriptor desc;
        desc.layout = ctx.bindGroupLayout;

        // color uniform buffer
        BindingPoint bufferBinding;
        BufferBinding entry;
        entry.hasDynamicOffset = true;
        entry.type = BufferType::Uniform;
        entry.minBindingSize = sizeof(nickel::cgmath::Color);
        entry.buffer = colorBuf;
        bufferBinding.binding = 1;
        bufferBinding.entry = entry;
        desc.entries.push_back(bufferBinding);

        // color texture
        {
            BindingPoint samplerBinding;
            samplerBinding.binding = 2;
            SamplerBinding binding;
            binding.sampler = ctx.whiteTextureSampler;
            binding.view = ctx.whiteTexture.view;
            binding.name = "mySampler";
            if (material.basicTexture) {
                binding.sampler = ctx.samplers[material.basicTexture->sampler];
                binding.view = ctx.images[material.basicTexture->texture].view;
            }
            samplerBinding.entry = binding;
            desc.entries.push_back(samplerBinding);
        }

        // normal texture
        {
            BindingPoint samplerBinding;
            samplerBinding.binding = 3;
            SamplerBinding binding;
            binding.sampler = ctx.normalTextureSampler;
            binding.view = ctx.specularTexture.view;
            binding.name = "normalMapSampler";
            if (material.normalTexture) {
                binding.sampler = ctx.samplers[material.normalTexture->sampler];
                binding.view = ctx.images[material.normalTexture->texture].view;
            }
            samplerBinding.entry = binding;
            desc.entries.push_back(samplerBinding);
        }

        return device.CreateBindGroup(desc);
    }

    void preorderNodes(Adapter adapter, Device device, Context& ctx,
                       const tinygltf::Node& node, const tinygltf::Model& model,
                       Node& parent) {
        std::unique_ptr<Node> newNode = std::make_unique<Node>();
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
        newNode->transform = calcNodeTransform(node);

        if (!data.positions.empty()) {
            mesh.posBuf =
                copyBuffer2GPU(device, data.positions, BufferUsage::Vertex);
            if (!data.indices.empty()) {
                mesh.indicesBuf =
                    copyBuffer2GPU(device, data.indices, BufferUsage::Index);
            }
            mesh.uvBuf = copyBuffer2GPU(device, data.uvs, BufferUsage::Vertex);
            mesh.normBuf =
                copyBuffer2GPU(device, data.normals, BufferUsage::Vertex);
            mesh.tanBuf =
                copyBuffer2GPU(device, data.tangents, BufferUsage::Vertex);
        }

        newNode->mesh = std::move(mesh);
        parent.children.emplace_back(std::move(newNode));

        for (auto child : node.children) {
            preorderNodes(adapter, device, ctx, model.nodes[child], model,
                          *parent.children.back());
        }
    }

    Primitive recordPrimInfo(Adapter adapter, Device dev, Context& ctx,
                             const tinygltf::Model& model,
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

void renderNodeRecursive(Node& node, Context& ctx,
                         RenderPassEncoder& renderPass,
                         const nickel::cgmath::Mat44& model) {
    auto newModel = model * node.transform;
    auto& mesh = node.mesh;
    if (mesh) {
        for (auto& prim : node.mesh.primitives) {
            auto& material = ctx.materials[prim.material];

            if (prim.indicesBufView.size > 0) {
                renderPass.SetIndexBuffer(mesh.indicesBuf, IndexType::Uint32,
                                          prim.indicesBufView.offset,
                                          prim.indicesBufView.size);
            }

            renderPass.SetPushConstant(ShaderStage::Vertex, newModel.data, 0,
                                       sizeof(nickel::cgmath::Mat44));
            renderPass.SetVertexBuffer(0, mesh.posBuf, prim.posBufView.offset,
                                       prim.posBufView.size);
            renderPass.SetVertexBuffer(1, mesh.uvBuf, prim.uvBufView.offset,
                                       prim.uvBufView.size);
            renderPass.SetVertexBuffer(2, mesh.normBuf, prim.normBufView.offset,
                                       prim.normBufView.size);
            renderPass.SetVertexBuffer(3, mesh.tanBuf, prim.tanBufView.offset,
                                       prim.tanBufView.size);

            renderPass.SetBindGroup(material.bindGroup,
                                    {material.basicColorFactor.offset});

            if (prim.indicesBufView.size > 0) {
                renderPass.DrawIndexed(prim.indicesBufView.count, 1, 0, 0, 0);
            } else {
                renderPass.Draw(prim.posBufView.count, 1, 0, 0);
            }
        }
    }

    for (auto& child : node.children) {
        renderNodeRecursive(*child, ctx, renderPass, newModel);
    }
}

void RenderScenes(const GLTFNode& node, Context& ctx,
                  RenderPassEncoder& renderPass) {
    for (auto& scene : node.scenes) {
        renderNodeRecursive(*scene.node, ctx, renderPass,
                            nickel::cgmath::Mat44::Identity());
    }
}

struct MVP {
    nickel::cgmath::Mat44 model, view, proj;
} mvp;

void initShaders(APIPreference api, Device device,
                 RenderPipeline::Descriptor& desc) {
    ShaderModule::Descriptor shaderDesc;

    if (api == APIPreference::Vulkan) {
        shaderDesc.code =
            nickel::ReadWholeFile<std::vector<char>>(
                "test/testbed/rhi/gltf/vert.spv", std::ios::binary)
                .value();
        desc.vertex.module = device.CreateShaderModule(shaderDesc);

        shaderDesc.code =
            nickel::ReadWholeFile<std::vector<char>>(
                "test/testbed/rhi/gltf/frag.spv", std::ios::binary)
                .value();
        desc.fragment.module = device.CreateShaderModule(shaderDesc);
    } else if (api == APIPreference::GL) {
        shaderDesc.code = nickel::ReadWholeFile<std::vector<char>>(
                              "test/testbed/rhi/gltf/shader.glsl.vert")
                              .value();
        desc.vertex.module = device.CreateShaderModule(shaderDesc);

        shaderDesc.code = nickel::ReadWholeFile<std::vector<char>>(
                              "test/testbed/rhi/gltf/shader.glsl.frag")
                              .value();
        desc.fragment.module = device.CreateShaderModule(shaderDesc);
    }
}

void initUniformBuffer(Context& ctx, Adapter adapter, Device device,
                       nickel::Window& window) {
    mvp.proj = nickel::cgmath::CreatePersp(
        nickel::cgmath::Deg2Rad(45.0f), window.Size().w / window.Size().h, 0.1,
        10000, adapter.RequestAdapterInfo().api == APIPreference::GL);
    mvp.view =
        nickel::cgmath::CreateTranslation(nickel::cgmath::Vec3{0, 0, -30});
    mvp.model = nickel::cgmath::Mat44::Identity();

    Buffer::Descriptor bufferDesc;
    bufferDesc.usage = BufferUsage::Uniform;
    bufferDesc.mappedAtCreation = true;
    bufferDesc.size = 4 * 4 * 4 * 3;
    ctx.uniformBuffer = device.CreateBuffer(bufferDesc);
    void* data = ctx.uniformBuffer.GetMappedRange();
    memcpy(data, &mvp, sizeof(mvp));
    if (!ctx.uniformBuffer.IsMappingCoherence()) {
        ctx.uniformBuffer.Flush();
    }
}

void initPipelineLayout(Context& ctx, Device& device) {
    PipelineLayout::Descriptor layoutDesc;
    layoutDesc.layouts.emplace_back(ctx.bindGroupLayout);
    PushConstantRange range;
    range.offset = 0;
    range.size = sizeof(nickel::cgmath::Mat44);
    range.stage = ShaderStage::Vertex;
    layoutDesc.pushConstRanges.emplace_back(range);
    ctx.layout = device.CreatePipelineLayout(layoutDesc);
}

void initBindGroupLayout(Context& ctx, Device& device) {
    BindGroupLayout::Descriptor bindGroupLayoutDesc;

    // MVP uniform buffer
    BufferBinding bufferBinding1;
    bufferBinding1.buffer = ctx.uniformBuffer;
    bufferBinding1.hasDynamicOffset = false;
    bufferBinding1.type = BufferType::Uniform;

    Entry entry;
    entry.arraySize = 1;
    entry.binding.binding = 0;
    entry.binding.entry = bufferBinding1;
    entry.visibility = ShaderStage::Vertex;
    bindGroupLayoutDesc.entries.emplace_back(entry);

    // color uniform buffer
    BufferBinding bufferBinding2;
    bufferBinding2.hasDynamicOffset = true;
    bufferBinding2.type = BufferType::Uniform;
    bufferBinding2.minBindingSize = sizeof(nickel::cgmath::Color);

    entry.arraySize = 1;
    entry.binding.binding = 1;
    entry.binding.entry = bufferBinding2;
    entry.visibility = ShaderStage::Fragment;
    bindGroupLayoutDesc.entries.emplace_back(entry);

    // color texture sampler
    SamplerBinding colorTextureBinding;
    colorTextureBinding.type = SamplerBinding::SamplerType::Filtering;
    colorTextureBinding.name = "mySampler";

    entry.arraySize = 1;
    entry.binding.binding = 2;
    entry.binding.entry = colorTextureBinding;
    entry.visibility = ShaderStage::Fragment;
    bindGroupLayoutDesc.entries.emplace_back(entry);

    // normal map sampler
    SamplerBinding normalTextureBinding;
    normalTextureBinding.type = SamplerBinding::SamplerType::Filtering;
    normalTextureBinding.name = "normalMapSampler";

    entry.arraySize = 1;
    entry.binding.binding = 3;
    entry.binding.entry = normalTextureBinding;
    entry.visibility = ShaderStage::Fragment;
    bindGroupLayoutDesc.entries.emplace_back(entry);

    ctx.bindGroupLayout = device.CreateBindGroupLayout(bindGroupLayoutDesc);
}

void initDepthTexture(Context& ctx, Device& dev, nickel::Window& window) {
    Texture::Descriptor desc;
    desc.format = TextureFormat::DEPTH24_PLUS_STENCIL8;
    desc.size.width = window.Size().w;
    desc.size.height = window.Size().h;
    desc.size.depthOrArrayLayers = 1;
    desc.usage = TextureUsage::RenderAttachment;
    ctx.depth = dev.CreateTexture(desc);

    ctx.depthView = ctx.depth.CreateView();
}

std::tuple<TextureBundle, Sampler, BindGroup> initSingleValueTexture(
    std::string_view name, BindGroupLayout layout, Device dev, uint32_t color) {
    Texture::Descriptor desc;
    desc.dimension = TextureType::Dim2;
    desc.format = TextureFormat::RGBA8_UNORM;
    desc.usage = Flags<TextureUsage>(TextureUsage::TextureBinding) |
                 TextureUsage::CopyDst;
    desc.size.width = 1;
    desc.size.height = 1;
    desc.size.depthOrArrayLayers = 1;
    TextureBundle bundle;
    bundle.texture = dev.CreateTexture(desc);
    bundle.view = bundle.texture.CreateView();

    Buffer::Descriptor bufDesc;
    bufDesc.size = 4;
    bufDesc.usage = BufferUsage::CopySrc;
    bufDesc.mappedAtCreation = true;
    auto buf = dev.CreateBuffer(bufDesc);
    memcpy(buf.GetMappedRange(), &color, sizeof(uint32_t));
    buf.Unmap();

    Sampler::Descriptor samplerDesc;
    samplerDesc.u = SamplerAddressMode::Repeat;
    samplerDesc.v = SamplerAddressMode::Repeat;
    samplerDesc.w = SamplerAddressMode::Repeat;

    auto sampler = dev.CreateSampler(samplerDesc);

    auto encoder = dev.CreateCommandEncoder();
    CommandEncoder::BufTexCopySrc src;
    src.buffer = buf;
    src.offset = 0;
    src.bytesPerRow = 4;
    src.rowsPerImage = 1;
    CommandEncoder::BufTexCopyDst dst;
    dst.texture = bundle.texture;
    encoder.CopyBufferToTexture(src, dst, {1, 1, 1});
    dev.GetQueue().Submit({encoder.Finish()});
    encoder.Destroy();
    buf.Destroy();

    BindGroup::Descriptor bindGroupDesc;
    bindGroupDesc.layout = layout;
    BindingPoint entry;
    entry.binding = 2;
    SamplerBinding binding;
    binding.sampler = sampler;
    binding.view = bundle.view;
    binding.name = name;
    entry.entry = binding;
    bindGroupDesc.entries.emplace_back(entry);
    auto bindGroup = dev.CreateBindGroup(bindGroupDesc);

    return {bundle, sampler, bindGroup};
}

void initWhiteTexture(Context& ctx, Device dev) {
    auto [bundle, sampler, bindGroup] = initSingleValueTexture(
        "mySampler", ctx.bindGroupLayout, dev, 0xFFFFFFFF);
    ctx.whiteTextureSampler = sampler;
    ctx.whiteTexture = bundle;
}

void initNormalTexture(Context& ctx, Device dev) {
    auto [bundle, sampler, bindGroup] = initSingleValueTexture(
        "normalMapSampler", ctx.bindGroupLayout, dev, 0xFFFF8080);
    ctx.normalTextureSampler = sampler;
    ctx.specularTexture = bundle;
}

void StartupSystem(gecs::commands cmds,
                   gecs::resource<gecs::mut<nickel::Window>> window) {
    auto& adapter =
        cmds.emplace_resource<Adapter>(window->Raw(), Adapter::Option{API});
    auto& device = cmds.emplace_resource<Device>(adapter.RequestDevice());
    auto& ctx = cmds.emplace_resource<Context>();

    RenderPipeline::Descriptor desc;
    initShaders(adapter.RequestAdapterInfo().api, device, desc);

    initUniformBuffer(ctx, adapter, device, window.get());
    initBindGroupLayout(ctx, device);
    initWhiteTexture(ctx, device);
    initNormalTexture(ctx, device);

    GLTFLoader loader;
    cmds.emplace_resource<GLTFNode>(loader.Load(
        std::filesystem::path{
            // "external/glTF-Sample-Models/2.0/2CylinderEngine/glTF/2CylinderEngine.gltf"},
            "external/glTF-Sample-Models/2.0/NormalTangentTest/glTF/NormalTangentTest.gltf"},
            // "external/glTF-Sample-Models/2.0/Avocado/glTF/Avocado.gltf"},
           // "external/glTF-Sample-Models/2.0/CesiumMilkTruck/glTF/CesiumMilkTruck.gltf"},
            // "external/glTF-Sample-Models/2.0/BoxTextured/glTF/"
            // "BoxTextured.gltf"},
        // "external/glTF-Sample-Models/2.0/Fox/glTF/Fox.gltf"},
        // "external/glTF-Sample-Models/2.0/SheenChair/glTF/SheenChair.gltf"},
        // "external/glTF-Sample-Models/2.0/Triangle/glTF/Triangle.gltf"},
        // "external/glTF-Sample-Models/2.0/TriangleWithoutIndices/glTF/TriangleWithoutIndices.gltf"},
        // "external/glTF-Sample-Models/2.0/TextureCoordinateTest/glTF/TextureCoordinateTest.gltf"},
        // "external/glTF-Sample-Models/2.0/UnlitTest/glTF/UnlitTest.gltf"},
        // "external/glTF-Sample-Models/2.0/CesiumMan/glTF/CesiumMan.gltf"},
        adapter, device, ctx));

    initPipelineLayout(ctx, device);
    initDepthTexture(ctx, device, window.get());

    // position buffer
    {
        RenderPipeline::BufferState state;
        state.stepMode = RenderPipeline::BufferState::StepMode::Vertex;
        state.arrayStride = 12;
        RenderPipeline::BufferState::Attribute attr;
        attr.format = VertexFormat::Float32x3;
        attr.shaderLocation = 0;
        attr.offset = 0;
        state.attributes.push_back(attr);
        desc.vertex.buffers.emplace_back(std::move(state));
    }

    // uv buffer
    {
        RenderPipeline::BufferState state;
        RenderPipeline::BufferState::Attribute attr;
        state.arrayStride = 8;
        attr.format = VertexFormat::Float32x2;
        attr.shaderLocation = 1;
        attr.offset = 0;
        state.attributes.push_back(attr);
        desc.vertex.buffers.emplace_back(std::move(state));
    }

    // normal buffer
    {
        RenderPipeline::BufferState state;
        RenderPipeline::BufferState::Attribute attr;
        state.arrayStride = 12;
        attr.format = VertexFormat::Float32x3;
        attr.shaderLocation = 2;
        attr.offset = 0;
        state.attributes.push_back(attr);
        desc.vertex.buffers.emplace_back(std::move(state));
    }

    // tangent buffer
    {
        RenderPipeline::BufferState state;
        RenderPipeline::BufferState::Attribute attr;
        state.arrayStride = 16;
        attr.format = VertexFormat::Float32x4;
        attr.shaderLocation = 3;
        attr.offset = 0;
        state.attributes.push_back(attr);
        desc.vertex.buffers.emplace_back(std::move(state));
    }

    desc.viewport.viewport.x = 0;
    desc.viewport.viewport.y = 0;
    desc.viewport.viewport.w = window->Size().w;
    desc.viewport.viewport.h = window->Size().h;
    desc.viewport.scissor.offset.x = 0;
    desc.viewport.scissor.offset.y = 0;
    desc.viewport.scissor.extent.width = window->Size().w;
    desc.viewport.scissor.extent.height = window->Size().h;

    RenderPipeline::FragmentTarget target;
    target.format = TextureFormat::Presentation;
    desc.layout = ctx.layout;
    desc.fragment.targets.emplace_back(target);

    RenderPipeline::DepthStencilState depthStencilState;
    depthStencilState.depthFormat = ctx.depth.Format();
    depthStencilState.depthWriteEnabled = true;
    depthStencilState.depthCompare = CompareOp::Greater;
    desc.depthStencil = depthStencilState;

    // desc.primitive.cullMode = CullMode::Back;

    ctx.pipeline = device.CreateRenderPipeline(desc);
}

void HandleEvent(gecs::resource<gecs::mut<Context>> ctx,
                 gecs::resource<nickel::Mouse> mouse,
                 gecs::resource<nickel::Keyboard> keyboard) {
    constexpr float offset = 0.01;
    constexpr float scaleStep = 0.01;
    static float scale = 1;
    static float x = 0, y = 0;
    if (mouse->LeftBtn().IsPress()) {
        y += mouse->Offset().x * offset;
        x += mouse->Offset().y * offset;
    }

    if (auto y = mouse->WheelOffset().y; y != 0) {
        scale += scaleStep * nickel::cgmath::Sign(y) *
                 (keyboard->Key(nickel::Key::Lctrl).IsPress() ? 100 : 1);
    }

    if (scale < 0) {
        scale = 0.001;
    }
    mvp.model = nickel::cgmath::CreateScale({scale, scale, scale}) *
                nickel::cgmath::CreateXYZRotation({x, y, 0});
}

void UpdateSystem(gecs::resource<gecs::mut<nickel::rhi::Device>> device,
                  gecs::resource<gecs::mut<Context>> ctx,
                  gecs::resource<gecs::mut<GLTFNode>> node) {
    RenderPass::Descriptor desc;
    RenderPass::Descriptor::ColorAttachment colorAtt;
    colorAtt.loadOp = AttachmentLoadOp::Clear;
    colorAtt.storeOp = AttachmentStoreOp::Store;
    colorAtt.clearValue = {0.3, 0.3, 0.3, 1};

    Texture::Descriptor textureDesc;
    textureDesc.format = TextureFormat::Presentation;
    auto texture = device->CreateTexture(textureDesc);

    auto view = texture.CreateView();
    colorAtt.view = view;
    desc.colorAttachments.emplace_back(colorAtt);

    desc.depthStencilAttachment =
        RenderPass::Descriptor::DepthStencilAttachment{};
    desc.depthStencilAttachment->view = ctx->depthView;
    desc.depthStencilAttachment->depthLoadOp = AttachmentLoadOp::Clear;
    desc.depthStencilAttachment->depthStoreOp = AttachmentStoreOp::Discard;
    desc.depthStencilAttachment->depthReadOnly = false;
    desc.depthStencilAttachment->depthClearValue = 0.0;

    auto encoder = device->CreateCommandEncoder();
    auto renderPass = encoder.BeginRenderPass(desc);
    renderPass.SetPipeline(ctx->pipeline);
    RenderScenes(node.get(), ctx.get(), renderPass);
    renderPass.End();
    auto cmd = encoder.Finish();

    Queue queue = device->GetQueue();

    queue.Submit({cmd});
    device->SwapContext();

    encoder.Destroy();
    view.Destroy();
    texture.Destroy();
}

void LogicUpdate(gecs::resource<gecs::mut<Context>> ctx) {
    void* data = ctx->uniformBuffer.GetMappedRange();
    memcpy(data, mvp.model.data, sizeof(mvp.model));
    if (!ctx->uniformBuffer.IsMappingCoherence()) {
        ctx->uniformBuffer.Flush();
    }
}

void ShutdownSystem(gecs::commands cmds,
                    gecs::resource<gecs::mut<Context>> ctx) {
    cmds.remove_resource<Context>();
    cmds.remove_resource<Device>();
    cmds.remove_resource<Adapter>();
}

void BootstrapSystem(gecs::world& world,
                     typename gecs::world::registry_type& reg) {
    auto& args = reg.res<nickel::CmdLineArgs>()->Args();
#ifdef NICKEL_HAS_VULKAN
    bool isVulkanBackend =
        args.size() == 1 ? true : (args[1] == "--api=gl" ? false : true);
#else
    bool isVulkanBackend = false;
#endif
    if (isVulkanBackend) {
        API = APIPreference::Vulkan;
    } else {
        API = APIPreference::GL;
    }
    nickel::Window& window = reg.commands().emplace_resource<nickel::Window>(
        "gltf", 1024, 720, API == APIPreference::Vulkan);

    reg
        // startup systems
        .regist_startup_system<nickel::VideoSystemInit>()
        .regist_startup_system<nickel::EventPollerInit>()
        .regist_startup_system<nickel::InputSystemInit>()
        .regist_startup_system<StartupSystem>()
        // shutdown systems
        .regist_shutdown_system<ShutdownSystem>()
        .regist_shutdown_system<nickel::EngineShutdown>()
        // update systems
        .regist_update_system<nickel::VideoSystemUpdate>()
        // other input handle event must put here(after mouse/keyboard
        // update)
        .regist_update_system<nickel::Mouse::Update>()
        .regist_update_system<nickel::Keyboard::Update>()
        .regist_update_system<nickel::HandleInputEvents>()
        .regist_update_system<UpdateSystem>()
        .regist_update_system<HandleEvent>()
        .regist_update_system<LogicUpdate>();
}