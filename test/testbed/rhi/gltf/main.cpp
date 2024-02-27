#include "graphics/rhi/rhi.hpp"
#include "nickel.hpp"
#define TINYGLTF_IMPLEMENTATION
#include "stb_image.h"
#include "tiny_gltf.h"

using namespace nickel::rhi;

APIPreference API = APIPreference::GL;

struct Context {
    PipelineLayout layout;
    RenderPipeline pipeline;
    Buffer uniformBuffer;
    BindGroupLayout bindGroupLayout;
    BindGroup bindGroup;
    Texture depth;
    TextureView depthView;
    std::vector<Texture> images;
    std::vector<TextureView> imageViews;
    std::vector<Sampler> samplers;
    Buffer verticesBuffer;
    Buffer uvBuffer;
    Buffer indicesBuffer;
    tinygltf::Model model;
};

struct BufferView {
    uint64_t offset;
    uint64_t size;
    uint32_t count;
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
BufferView CopyBufferFromGLTF(std::vector<unsigned char>& dst,
                              int componentType,
                              const tinygltf::Accessor& accessor,
                              const tinygltf::Model& model) {
    auto& view = model.bufferViews[accessor.bufferView];
    auto& buffer = model.buffers[view.buffer];
    auto offset = accessor.byteOffset + view.byteOffset;
    auto size = accessor.count * sizeof(RequireT) * tinygltf::GetComponentSizeInBytes(accessor.type);
    BufferView bufView;
    bufView.offset = dst.size();
    bufView.size = size;
    bufView.count = accessor.count;

    dst.resize(dst.size() + size);
    auto copySrc = buffer.data.data() + offset;
    auto dstSrc = dst.data() + dst.size() - size;
    auto srcComponentNum = tinygltf::GetNumComponentsInType(accessor.type);
    auto dstComponentNum = tinygltf::GetNumComponentsInType(componentType);
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
}

struct GPUMesh {
    GPUMesh(const std::filesystem::path& filename, Device device,
            Context& ctx) {
        tinygltf::TinyGLTF loader;
        std::string err, warn;
        if (!loader.LoadASCIIFromFile(&model_, &err, &warn,
                                      filename.string())) {
            LOGW(nickel::log_tag::Asset, "load model from ", filename,
                 " failed:\n\terr:", err, "\n\twarn:", warn);
        }

        auto rootDir = filename.parent_path();

        for (auto& image : model_.images) {
            auto [texture, view, sampler] =
                loadTexture(device, rootDir / image.uri);
            ctx.images.emplace_back(texture);
            ctx.imageViews.emplace_back(view);
            ctx.samplers.emplace_back(sampler);
        }

        std::vector<unsigned char> verticesBuffer;
        std::vector<uint32_t> indicesBuffer;
        std::vector<float> uvBuffer;
        for (auto& mesh : model_.meshes) {
            for (int i = 0; i < mesh.primitives.size(); i++) {
                auto& prim = mesh.primitives[i];
                auto& attrs = prim.attributes;
                if (auto it = attrs.find("POSITION"); it != attrs.end()) {
                    auto& accessor = model_.accessors[it->second];
                    auto& view = model_.bufferViews[accessor.bufferView];
                    auto& buffer = model_.buffers[view.buffer];
                    auto offset = accessor.byteOffset + view.byteOffset;
                    auto size = accessor.count * 3 * 4;
                    BufferView bufView;
                    bufView.offset = verticesBuffer.size();
                    bufView.size = size;
                    bufView.count = accessor.count;
                    verticesViews_[i] = bufView;

                    verticesBuffer.resize(verticesBuffer.size() + size);
                    auto copySrc = buffer.data.data() + offset;
                    auto dstSrc =
                        verticesBuffer.data() + verticesBuffer.size() - size;
                    size_t elemCount = 3;
                    if (accessor.type == TINYGLTF_TYPE_VEC4) {
                        elemCount = 4;
                    }
                    switch (accessor.componentType) {
                        case TINYGLTF_COMPONENT_TYPE_FLOAT:
                            memcpy(dstSrc, copySrc, size);
                            break;
                        case TINYGLTF_COMPONENT_TYPE_DOUBLE:
                            ConvertRangeData((const double*)copySrc,
                                             (float*)dstSrc, accessor.count,
                                             elemCount, 3);
                            break;
                        case TINYGLTF_COMPONENT_TYPE_INT:
                            ConvertRangeData((const int*)copySrc,
                                             (float*)dstSrc, accessor.count,
                                             elemCount, 3);
                            break;
                        case TINYGLTF_COMPONENT_TYPE_BYTE:
                            ConvertRangeData((const char*)copySrc,
                                             (float*)dstSrc, accessor.count,
                                             elemCount, 3);
                            break;
                        case TINYGLTF_COMPONENT_TYPE_SHORT:
                            ConvertRangeData((const short*)copySrc,
                                             (float*)dstSrc, accessor.count,
                                             elemCount, 3);
                            break;
                    }
                }

                if (auto it = attrs.find("TEXCOORD_0"); it != attrs.end()) {
                    auto& accessor = model_.accessors[it->second];
                    if (accessor.type != TINYGLTF_TYPE_VEC2 ||
                        accessor.componentType !=
                            TINYGLTF_COMPONENT_TYPE_FLOAT) {
                        TODO("convert uv to vec3");
                    }
                    auto view = model_.bufferViews[accessor.bufferView];
                    auto& buffer = model_.buffers[view.buffer];
                }

                if (prim.indices != -1) {
                    auto& accessor = model_.accessors[prim.indices];
                    auto& view = model_.bufferViews[accessor.bufferView];
                    auto& buffer = model_.buffers[view.buffer];
                    auto offset = accessor.byteOffset + view.byteOffset;

                    if (accessor.type != TINYGLTF_TYPE_SCALAR ||
                        (accessor.componentType !=
                             TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT &&
                         accessor.componentType !=
                             TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT)) {
                        TODO("convert indices to uint32_t");
                    }
                    BufferView bufView;
                    bufView.offset = indicesBuffer.size();
                    bufView.size = sizeof(uint32_t) * accessor.count;
                    bufView.count = accessor.count;
                    indicesViews_[i] = bufView;
                    if (accessor.componentType ==
                        TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT) {
                        indicesBuffer.resize(indicesBuffer.size() +
                                             accessor.count);
                        ConvertRangeData<uint16_t, uint32_t>(
                            (uint16_t*)(buffer.data.data() + offset),
                            indicesBuffer.data() + indicesBuffer.size() -
                                accessor.count,
                            accessor.count, 1, 1);
                    } else if (accessor.componentType ==
                               TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT) {
                        indicesBuffer.resize(indicesBuffer.size() +
                                             accessor.count);
                        memcpy(indicesBuffer.data(), buffer.data.data(),
                               sizeof(uint32_t) * accessor.count);
                    }
                }
            }
        }

        Buffer::Descriptor desc;
        desc.usage = BufferUsage::Vertex;
        desc.mappedAtCreation = true;
        desc.size = verticesBuffer.size();
        ctx.verticesBuffer = device.CreateBuffer(desc);
        memcpy(ctx.verticesBuffer.GetMappedRange(), verticesBuffer.data(),
               verticesBuffer.size());
        ctx.verticesBuffer.Unmap();

        desc.usage = BufferUsage::Index;
        desc.mappedAtCreation = true;
        desc.size = indicesBuffer.size() * sizeof(uint32_t);
        ctx.indicesBuffer = device.CreateBuffer(desc);
        memcpy(ctx.indicesBuffer.GetMappedRange(), indicesBuffer.data(),
               desc.size);
        ctx.indicesBuffer.Unmap();
    }

    std::tuple<Texture, TextureView, Sampler> loadTexture(
        Device device, const std::filesystem::path& filename) {
        int w, h;
        void* data = stbi_load(filename.string().c_str(), &w, &h, nullptr,
                               STBI_rgb_alpha);

        Texture::Descriptor desc;
        desc.format = TextureFormat::RGBA8_UNORM_SRGB;
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

        return {texture, view, device.CreateSampler({})};
    }

    void Render(RenderPassEncoder renderPass, Context& ctx) {
        for (auto& mesh : model_.meshes) {
            for (int i = 0; i < mesh.primitives.size(); i++) {
                BufferView *verticesView{}, *indicesView{};
                if (auto it = verticesViews_.find(i);
                    it != verticesViews_.end()) {
                    verticesView = &it->second;
                }
                if (auto it = indicesViews_.find(i);
                    it != indicesViews_.end()) {
                    indicesView = &it->second;
                }

                renderPass.SetVertexBuffer(0, ctx.verticesBuffer,
                                           verticesView->offset,
                                           verticesView->size);

                if (indicesView) {
                    renderPass.SetIndexBuffer(
                        ctx.indicesBuffer, IndexType::Uint32,
                        indicesView->offset, indicesView->size);
                    renderPass.DrawIndexed(
                        indicesView->count, 1,
                        indicesView->offset / sizeof(uint32_t), 0, 0);
                } else {
                    renderPass.Draw(verticesView->count, 1, 0, 0);
                }
            }
        }
    }

    tinygltf::Model model_;
    std::unordered_map<int, BufferView> verticesViews_;
    std::unordered_map<int, BufferView> indicesViews_;
};

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

void initUniformBuffer(Context& ctx, Device& device, nickel::Window& window) {
    mvp.proj = nickel::cgmath::CreatePersp(nickel::cgmath::Deg2Rad(45.0f),
                                           window.Size().w / window.Size().h,
                                           0.1, 100);
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
    ctx.layout = device.CreatePipelineLayout(layoutDesc);
}

void initBindGroupAndLayout(Context& ctx, Device& device) {
    BindGroupLayout::Descriptor bindGroupLayoutDesc;

    // uniform buffer
    Entry entry;
    entry.arraySize = 1;
    entry.binding = 0;
    entry.visibility = ShaderStage::Vertex;
    BufferBinding bufferBinding;
    bufferBinding.buffer = ctx.uniformBuffer;
    bufferBinding.hasDynamicOffset = false;
    bufferBinding.type = BufferType::Uniform;
    entry.resourceLayout = bufferBinding;
    bindGroupLayoutDesc.entries.emplace_back(entry);

    /*
    // sampler
    entry.arraySize = 1;
    entry.binding = 1;
    entry.visibility = ShaderStage::Fragment;
    SamplerBinding samplerBinding;
    samplerBinding.type = SamplerBinding::SamplerType::Filtering;
    samplerBinding.name = "mySampler";
    // samplerBinding.sampler = ctx.sampler;
    // samplerBinding.view = ctx.imageView;
    entry.resourceLayout = samplerBinding;
    bindGroupLayoutDesc.entries.emplace_back(entry);
    */

    ctx.bindGroupLayout = device.CreateBindGroupLayout(bindGroupLayoutDesc);

    BindGroup::Descriptor bindGroupDesc;
    bindGroupDesc.layout = ctx.bindGroupLayout;
    bindGroupDesc.entries = bindGroupLayoutDesc.entries;
    ctx.bindGroup = device.CreateBindGroup(bindGroupDesc);
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

void StartupSystem(gecs::commands cmds,
                   gecs::resource<gecs::mut<nickel::Window>> window) {
    auto& adapter =
        cmds.emplace_resource<Adapter>(window->Raw(), Adapter::Option{API});
    auto& device = cmds.emplace_resource<Device>(adapter.RequestDevice());
    auto& ctx = cmds.emplace_resource<Context>();
    auto& mesh = cmds.emplace_resource<GPUMesh>(
        std::filesystem::path{
            "external/glTF-Sample-Models/2.0/Box/glTF/Box.gltf"},
        device, ctx);

    RenderPipeline::Descriptor desc;

    RenderPipeline::VertexState vertexState;
    RenderPipeline::BufferState bufferState;

    RenderPipeline::BufferState state;
    state.stepMode = RenderPipeline::BufferState::StepMode::Vertex;
    state.arrayStride = 12;
    RenderPipeline::BufferState::Attribute attr;
    attr.format = VertexFormat::Float32x3;
    attr.shaderLocation = 0;
    attr.offset = 0;
    state.attributes.push_back(attr);
    desc.vertex.buffers.emplace_back(std::move(state));

    desc.viewport.viewport.x = 0;
    desc.viewport.viewport.y = 0;
    desc.viewport.viewport.w = window->Size().w;
    desc.viewport.viewport.h = window->Size().h;
    desc.viewport.scissor.offset.x = 0;
    desc.viewport.scissor.offset.y = 0;
    desc.viewport.scissor.extent.width = window->Size().w;
    desc.viewport.scissor.extent.height = window->Size().h;

    initShaders(adapter.RequestAdapterInfo().api, device, desc);

    initUniformBuffer(ctx, device, window.get());
    initBindGroupAndLayout(ctx, device);
    initPipelineLayout(ctx, device);
    initDepthTexture(ctx, device, window.get());

    RenderPipeline::FragmentTarget target;
    target.format = TextureFormat::Presentation;
    desc.layout = ctx.layout;
    desc.fragment.targets.emplace_back(target);

    RenderPipeline::DepthStencilState depthStencilState;
    depthStencilState.depthFormat = ctx.depth.Format();
    depthStencilState.depthWriteEnabled = true;
    depthStencilState.depthCompare = CompareOp::Greater;
    desc.depthStencil = depthStencilState;

    ctx.pipeline = device.CreateRenderPipeline(desc);
}

void UpdateSystem(gecs::resource<gecs::mut<nickel::rhi::Device>> device,
                  gecs::resource<gecs::mut<Context>> ctx,
                  gecs::resource<gecs::mut<GPUMesh>> mesh) {
    RenderPass::Descriptor desc;
    RenderPass::Descriptor::ColorAttachment colorAtt;
    colorAtt.loadOp = AttachmentLoadOp::Clear;
    colorAtt.storeOp = AttachmentStoreOp::Store;
    colorAtt.clearValue.fill(1);

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
    renderPass.SetBindGroup(ctx->bindGroup);
    mesh->Render(renderPass, ctx.get());
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
    static float x = 0, y = 0;

    void* data = ctx->uniformBuffer.GetMappedRange();
    mvp.model = nickel::cgmath::CreateXYZRotation({x, y, 0});
    memcpy(data, mvp.model.data, sizeof(mvp.model));
    if (!ctx->uniformBuffer.IsMappingCoherence()) {
        ctx->uniformBuffer.Flush();
    }
    x += 0.001;
    y += 0.002;
}

void ShutdownSystem(gecs::commands cmds,
                    gecs::resource<gecs::mut<Context>> ctx) {
    ctx->depthView.Destroy();
    ctx->depth.Destroy();
    ctx->bindGroup.Destroy();
    ctx->bindGroupLayout.Destroy();
    ctx->uniformBuffer.Destroy();
    ctx->layout.Destroy();
    ctx->pipeline.Destroy();
    cmds.remove_resource<Device>();
    cmds.remove_resource<Adapter>();
}

void BootstrapSystem(gecs::world& world,
                     typename gecs::world::registry_type& reg) {
    auto& args = reg.res<nickel::CmdLineArgs>()->Args();
    bool isVulkanBackend =
        args.size() == 1 ? true : (args[1] == "--api=gl" ? false : true);
    if (isVulkanBackend) {
        API = APIPreference::Vulkan;
    } else {
        API = APIPreference::GL;
    }
    API = APIPreference::GL;
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
        // other input handle event must put here(after mouse/keyboard update)
        .regist_update_system<nickel::Mouse::Update>()
        .regist_update_system<nickel::Keyboard::Update>()
        .regist_update_system<nickel::HandleInputEvents>()
        .regist_update_system<UpdateSystem>()
        .regist_update_system<LogicUpdate>();
}