#include "graphics/rhi/rhi.hpp"
#include "graphics/rhi/util.hpp"
#include "nickel.hpp"
#define TINYGLTF_IMPLEMENTATION
#include "stb_image.h"
#include "tiny_gltf.h"

using namespace nickel::rhi;

APIPreference API = APIPreference::GL;


struct BufferView {
    uint32_t offset;
    uint64_t size;
    uint32_t count;
};

struct Material final {
    BufferView basicColorFactor;
};

struct TextureBundle {
    Texture texture;
    TextureView view;
    BindGroup bindGroup;
    Sampler sampler;
};

struct Context final {
    PipelineLayout layout;
    RenderPipeline pipeline;
    Buffer uniformBuffer;
    BindGroupLayout bindGroupLayout;
    Texture depth;
    TextureView depthView;
    std::vector<TextureBundle> images;

    // gltf related
    Buffer verticesBuffer;
    Buffer uvBuffer;
    Buffer indicesBuffer;
    Buffer colorBuffer;
    TextureBundle whiteTexture;
    std::vector<Material> materials;
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
                tinygltf::GetComponentSizeInBytes(accessor.componentType) *
                tinygltf::GetNumComponentsInType(accessor.type);
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

struct GPUMesh {
    GPUMesh(const std::filesystem::path& filename, Adapter adapter, Device device,
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
            TextureBundle bundle;
            bundle.texture = texture;
            bundle.view = view;
            bundle.sampler = sampler;
            BindGroup::Descriptor desc;
            desc.layout = ctx.bindGroupLayout;
            BindingPoint entry;
            entry.binding = 2;
            SamplerBinding binding;
            binding.sampler = bundle.sampler;
            binding.view = bundle.view;
            binding.name = "mySampler";
            entry.entry = binding;
            desc.entries.emplace_back(entry);
            bundle.bindGroup = device.CreateBindGroup(desc);
            ctx.images.emplace_back(bundle);
        }

        std::vector<unsigned char> verticesBuffer;
        std::vector<unsigned char> indicesBuffer;
        std::vector<unsigned char> uvBuffer;
        std::vector<unsigned char> colorBuffer;
        colorBuffer.resize(4 * 4);
        float* ptr = (float*)colorBuffer.data();
        *ptr = 1;
        *(ptr + 1) = 1;
        *(ptr + 2) = 1;
        *(ptr + 3) = 1;
        int colorCount = 1;
        for (auto& mesh : model_.meshes) {
            for (int i = 0; i < mesh.primitives.size(); i++) {
                auto& prim = mesh.primitives[i];
                auto& attrs = prim.attributes;
                uint32_t positionCount = 0;
                if (auto it = attrs.find("POSITION"); it != attrs.end()) {
                    auto& accessor = model_.accessors[it->second];
                    positionCount = accessor.count;
                    auto bufferView = CopyBufferFromGLTF<float>(
                        verticesBuffer, TINYGLTF_TYPE_VEC3, accessor, model_);
                    verticesViews_[i] = bufferView;
                }

                std::optional<uint32_t> indicesCount;
                if (prim.indices != -1) {
                    auto& accessor = model_.accessors[prim.indices];
                    auto bufView = CopyBufferFromGLTF<uint32_t>(
                        indicesBuffer, TINYGLTF_TYPE_SCALAR, accessor, model_);
                    indicesViews_[i] = bufView;
                    indicesCount = accessor.count;
                }

                if (auto it = attrs.find("TEXCOORD_0"); it != attrs.end()) {
                    auto& accessor = model_.accessors[it->second];
                    auto bufferView = CopyBufferFromGLTF<float>(
                        uvBuffer, TINYGLTF_TYPE_VEC2, accessor, model_);
                    uvViews_[i] = bufferView;
                } else {
                    BufferView view;
                    view.count =
                        indicesCount ? indicesCount.value() : positionCount;
                    view.size = view.count * 4;
                    view.count = uvBuffer.size();
                    uvBuffer.resize(uvBuffer.size() + view.size, 0);
                    uvViews_[i] = view;
                }

                if (prim.material != -1) {
                    auto& material = model_.materials[prim.material];
                    Material mat;
                    mat.basicColorFactor.count = 4;
                    mat.basicColorFactor.offset = AlignTo(
                        (colorCount++) * sizeof(nickel::cgmath::Color),
                        adapter.Limits().minUniformBufferOffsetAlignment);
                    mat.basicColorFactor.size = sizeof(nickel::cgmath::Color);
                    auto& colorFactor = material.pbrMetallicRoughness.baseColorFactor;
                    int i = 0;
                    colorBuffer.resize(mat.basicColorFactor.offset + mat.basicColorFactor.size);
                    nickel::cgmath::Color color{0, 0, 0, 1};
                    for (int i = 0; i < colorFactor.size(); i++) {
                        color[i] = colorFactor[i];
                    }
                    memcpy(colorBuffer.data() + mat.basicColorFactor.offset, color.data, sizeof(color.data));
                    ctx.materials.emplace_back(std::move(mat));
                } else {
                    Material mat;
                    mat.basicColorFactor.count = 4;
                    mat.basicColorFactor.offset = 0;
                    mat.basicColorFactor.size =  4 * 4;
                    ctx.materials.emplace_back(std::move(mat));
                }
            }
        }

        ctx.verticesBuffer =
            copyBuffer2GPU(device, verticesBuffer, BufferUsage::Vertex);
        ctx.indicesBuffer =
            copyBuffer2GPU(device, indicesBuffer, BufferUsage::Index);
        ctx.uvBuffer = copyBuffer2GPU(device, uvBuffer, BufferUsage::Vertex);
        ctx.colorBuffer =
            copyBuffer2GPU(device, colorBuffer, BufferUsage::Uniform);
    }

    void Render(RenderPassEncoder renderPass, Context& ctx) {
        for (auto& mesh : model_.meshes) {
            for (int i = 0; i < mesh.primitives.size(); i++) {
                BufferView *verticesView{}, *uvView{}, *indicesView{};
                if (auto it = verticesViews_.find(i);
                    it != verticesViews_.end()) {
                    verticesView = &it->second;
                }
                if (auto it = uvViews_.find(i); it != uvViews_.end()) {
                    uvView = &it->second;
                }
                if (auto it = indicesViews_.find(i);
                    it != indicesViews_.end()) {
                    indicesView = &it->second;
                }

                auto& material = ctx.materials[i];

                renderPass.SetVertexBuffer(0, ctx.verticesBuffer,
                                           verticesView->offset,
                                           verticesView->size);
                renderPass.SetVertexBuffer(1, ctx.uvBuffer, uvView->offset,
                                           uvView->size);
                renderPass.SetBindGroup(ctx.whiteTexture.bindGroup, {0, material.basicColorFactor.offset});

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

private:
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

    tinygltf::Model model_;
    std::unordered_map<int, BufferView> verticesViews_;
    std::unordered_map<int, BufferView> uvViews_;
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

void initBindGroupLayout(Context& ctx, Device& device) {
    BindGroupLayout::Descriptor bindGroupLayoutDesc;

    // uniform buffer
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

    // uniform buffer
    BufferBinding bufferBinding2;
    bufferBinding2.buffer = ctx.colorBuffer;
    bufferBinding2.hasDynamicOffset = true;
    bufferBinding2.type = BufferType::Uniform;
    bufferBinding2.minBindingSize = sizeof(nickel::cgmath::Color);

    entry.arraySize = 1;
    entry.binding.binding = 1;
    entry.binding.entry = bufferBinding2;
    entry.visibility = ShaderStage::Fragment;
    bindGroupLayoutDesc.entries.emplace_back(entry);

    // sampler
    SamplerBinding samplerBinding;
    samplerBinding.type = SamplerBinding::SamplerType::Filtering;
    samplerBinding.name = "mySampler";
    samplerBinding.sampler = ctx.whiteTexture.sampler;
    samplerBinding.view = ctx.whiteTexture.view;

    entry.arraySize = 1;
    entry.binding.binding = 2;
    entry.binding.entry = samplerBinding;
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

void initWhiteTexture(Context& ctx, Device dev) {
    Texture::Descriptor desc;
    desc.dimension = TextureType::Dim2;
    desc.format = TextureFormat::RGBA8_UNORM_SRGB;
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
    uint32_t data = 0xFFFF00FF;
    auto buf = dev.CreateBuffer(bufDesc);
    memcpy(buf.GetMappedRange(), &data, sizeof(uint32_t));
    buf.Unmap();

    Sampler::Descriptor samplerDesc;
    samplerDesc.u = SamplerAddressMode::Repeat;
    samplerDesc.v = SamplerAddressMode::Repeat;
    samplerDesc.w = SamplerAddressMode::Repeat;
    bundle.sampler = dev.CreateSampler(samplerDesc);

    auto encoder = dev.CreateCommandEncoder();
    CommandEncoder::BufTexCopySrc src;
    src.buffer = buf;
    src.offset = 0;
    src.bytesPerRow = 4;
    src.rowsPerImage = 1;
    CommandEncoder::BufTexCopyDst dst;
    dst.texture = bundle.texture;
    encoder.CopyBufferToTexture(src, dst, {4, 1, 1});
    dev.GetQueue().Submit({encoder.Finish()});
    encoder.Destroy();
    buf.Destroy();

    BindGroup::Descriptor bindGroupDesc;
    bindGroupDesc.layout = ctx.bindGroupLayout;
    BindingPoint entry;
    entry.binding = 2;
    SamplerBinding binding;
    binding.sampler = bundle.sampler;
    binding.view = bundle.view;
    binding.name = "mySampler";
    entry.entry = binding;
    bindGroupDesc.entries.emplace_back(entry);
    bundle.bindGroup = dev.CreateBindGroup(bindGroupDesc);

    ctx.whiteTexture = std::move(bundle);
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
        adapter, device, ctx);

    RenderPipeline::Descriptor desc;
    initShaders(adapter.RequestAdapterInfo().api, device, desc);
    initUniformBuffer(ctx, device, window.get());
    initBindGroupLayout(ctx, device);
    initWhiteTexture(ctx, device);
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