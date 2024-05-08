#include "graphics/context.hpp"

namespace nickel {

GPUMesh2D::~GPUMesh2D() {
    verticesBuffer.Destroy();
    indicesBuffer.Destroy();
}

Render2DContext::Render2DContext(rhi::APIPreference api, rhi::Device device,
                                 const cgmath::Rect& viewport,
                                 RenderContext& ctx)
    : device_{device} {
    initUsableVertexSlots();
    initBuffers();
    initPipelineShader(api);
    bindGroupLayout = createBindGroupLayout(ctx);
    defaultBindGroup = createDefaultBindGroup();
    pipelineLayout = createPipelineLayout();
    pipeline = createPipeline(api, ctx);
}

void Render2DContext::RecreatePipeline(rhi::APIPreference api,
                                       RenderContext& ctx) {
    pipeline.Destroy();
    pipeline = createPipeline(api, ctx);
}

uint32_t Render2DContext::GenVertexSlot() {
    uint32_t slot = usableVertexSlots_.top();
    usableVertexSlots_.pop();
    return slot;
}

void Render2DContext::ReuseVertexSlot(uint32_t slot) {
    usableVertexSlots_.push(slot);
}

rhi::PipelineLayout Render2DContext::createPipelineLayout() {
    rhi::PipelineLayout::Descriptor layoutDesc;
    layoutDesc.layouts.emplace_back(bindGroupLayout);
    rhi::PushConstantRange range;
    range.offset = 0;
    range.size = sizeof(nickel::cgmath::Mat44);
    range.stage = rhi::ShaderStage::Vertex;
    layoutDesc.pushConstRanges.emplace_back(range);
    return device_.CreatePipelineLayout(layoutDesc);
}

rhi::BindGroupLayout Render2DContext::createBindGroupLayout(
    RenderContext& ctx) {
    rhi::BindGroupLayout::Descriptor desc;

    // mvp buffer
    {
        rhi::BufferBinding bufferBinding;
        bufferBinding.buffer = ctx.mvpBuffer;
        bufferBinding.hasDynamicOffset = false;
        bufferBinding.type = rhi::BufferType::Uniform;

        rhi::Entry entry;
        entry.arraySize = 1;
        entry.binding.binding = 0;
        entry.binding.entry = bufferBinding;
        entry.visibility = rhi::ShaderStage::Vertex;
        desc.entries.emplace_back(entry);
    }

    // sampler2D
    {
        rhi::SamplerBinding binding;
        binding.sampler = GetSampler(rhi::SamplerAddressMode::ClampToEdge,
                                     rhi::SamplerAddressMode::ClampToEdge,
                                     rhi::Filter::Linear, rhi::Filter::Linear);
        binding.name = "mySampler";

        rhi::Entry entry;
        entry.arraySize = 1;
        entry.binding.binding = 1;
        entry.binding.entry = binding;
        entry.visibility = rhi::ShaderStage::Fragment;
        desc.entries.emplace_back(entry);
    }

    // texture2D
    {
        rhi::TextureBinding binding;
        binding.view = ctx.whiteTextureView;
        binding.viewDimension = rhi::TextureViewType::Dim2;
        binding.sampleType = rhi::TextureBinding::SampleType::Float;
        binding.name = "myTexture";

        rhi::Entry entry;
        entry.arraySize = 1;
        entry.binding.binding = 2;
        entry.binding.entry = binding;
        entry.visibility = rhi::ShaderStage::Fragment;
        desc.entries.emplace_back(entry);
    }

    return device_.CreateBindGroupLayout(desc);
}

rhi::Sampler Render2DContext::GetSampler(rhi::SamplerAddressMode u,
                                         rhi::SamplerAddressMode v,
                                         rhi::Filter min, rhi::Filter mag) {
    auto key = static_cast<uint8_t>(u) | (static_cast<uint8_t>(v) << 8) |
               (static_cast<uint8_t>(min) << 16) |
               (static_cast<uint8_t>(mag) << 24);
    if (auto it = samplers_.find(key); it != samplers_.end()) {
        return it->second;
    }

    rhi::Sampler::Descriptor desc;
    desc.u = u;
    desc.v = v;
    desc.min = min;
    desc.mag = mag;
    auto sampler = device_.CreateSampler(desc);
    samplers_.emplace(key, sampler);
    return sampler;
}

void Render2DContext::initBuffers() {
    // vertex buffer
    {
        rhi::Buffer::Descriptor desc;
        desc.mappedAtCreation = true;
        desc.size = VertexBufferSize;
        desc.usage =
            rhi::Flags(rhi::BufferUsage::Vertex) | rhi::BufferUsage::MapWrite;
        vertexBuffer = device_.CreateBuffer(desc);
    }

    // indices buffer
    {
        std::array<uint32_t, 6> oneIndices = {
            0, 1, 2, 2, 1, 3,
        };

        rhi::Buffer::Descriptor desc;
        desc.mappedAtCreation = true;
        desc.size = sizeof(oneIndices);
        desc.usage =
            rhi::Flags(rhi::BufferUsage::Index) | rhi::BufferUsage::MapWrite;
        indexBuffer = device_.CreateBuffer(desc);

        auto ptr = indexBuffer.GetMappedRange();

        memcpy(ptr, oneIndices.data(), sizeof(oneIndices));

        indexBuffer.Unmap();
    }
}

void Render2DContext::initUsableVertexSlots() {
    for (int i = 0; i < MaxRectCount; i++) {
        usableVertexSlots_.push(i);
    }
}

rhi::BindGroup Render2DContext::createDefaultBindGroup() {
    rhi::BindGroup::Descriptor desc;
    desc.layout = bindGroupLayout;
    return device_.CreateBindGroup(desc);
}

Render2DContext::~Render2DContext() {
    vertexBuffer.Destroy();
    indexBuffer.Destroy();
    vertexShader.Destroy();
    fragmentShader.Destroy();
    for (auto& [_, sampler] : samplers_) {
        sampler.Destroy();
    }
    pipeline.Destroy();
    pipelineLayout.Destroy();
    defaultBindGroup.Destroy();
    bindGroupLayout.Destroy();
}

rhi::Texture RenderContext::createSingleValueTexture(rhi::Device dev,
                                                     uint32_t color) {
    rhi::Texture::Descriptor desc;
    desc.dimension = rhi::TextureType::Dim2;
    desc.format = rhi::TextureFormat::RGBA8_UNORM;
    desc.usage =
        rhi::Flags<rhi::TextureUsage>(rhi::TextureUsage::TextureBinding) |
        rhi::TextureUsage::CopyDst;
    desc.size.width = 1;
    desc.size.height = 1;
    desc.size.depthOrArrayLayers = 1;
    auto texture = dev.CreateTexture(desc);

    rhi::Buffer::Descriptor bufDesc;
    bufDesc.size = 4;
    bufDesc.usage = rhi::BufferUsage::CopySrc;
    bufDesc.mappedAtCreation = true;
    auto buf = dev.CreateBuffer(bufDesc);
    memcpy(buf.GetMappedRange(), &color, sizeof(uint32_t));
    buf.Unmap();

    auto encoder = dev.CreateCommandEncoder();
    rhi::CommandEncoder::BufTexCopySrc src;
    src.buffer = buf;
    src.offset = 0;
    src.rowLength = 4;
    src.rowsPerImage = 1;
    rhi::CommandEncoder::BufTexCopyDst dst;
    dst.texture = texture;
    encoder.CopyBufferToTexture(src, dst, {1, 1, 1});
    dev.GetQueue().Submit({encoder.Finish()});
    encoder.Destroy();
    buf.Destroy();

    return texture;
}

rhi::RenderPipeline Render2DContext::createPipeline(rhi::APIPreference api,
                                                    RenderContext& ctx) {
    rhi::RenderPipeline::Descriptor desc;

    desc.vertex.module = vertexShader;
    desc.fragment.module = fragmentShader;

    auto& bufferState = Vertex2D::Layout();
    desc.vertex.buffers.emplace_back(bufferState);

    rhi::RenderPipeline::FragmentTarget target;
    target.format = rhi::TextureFormat::Presentation;
    target.blend.color.srcFactor = rhi::BlendFactor::SrcAlpha;
    target.blend.color.dstFactor = rhi::BlendFactor::OneMinusSrcAlpha;
    target.blend.color.operation = rhi::BlendOp::Add;
    target.blend.alpha.srcFactor = rhi::BlendFactor::One;
    target.blend.alpha.dstFactor = rhi::BlendFactor::Zero;
    desc.layout = pipelineLayout;
    desc.fragment.targets.emplace_back(target);

    return device_.CreateRenderPipeline(desc);
}

void Render2DContext::initPipelineShader(rhi::APIPreference api) {
    rhi::ShaderModule::Descriptor shaderDesc;

    if (api == rhi::APIPreference::GL) {
        shaderDesc.code =
            nickel::ReadWholeFile<std::vector<char>>("shader/gl/shader.vert")
                .value();
        vertexShader = device_.CreateShaderModule(shaderDesc);

        shaderDesc.code =
            nickel::ReadWholeFile<std::vector<char>>("shader/gl/shader.frag")
                .value();
        fragmentShader = device_.CreateShaderModule(shaderDesc);
    } else if (api == rhi::APIPreference::Vulkan) {
        shaderDesc.code = nickel::ReadWholeFile<std::vector<char>>(
                              "nickel/shader/vk/vert2d.spv", std::ios::binary)
                              .value();
        vertexShader = device_.CreateShaderModule(shaderDesc);

        shaderDesc.code = nickel::ReadWholeFile<std::vector<char>>(
                              "nickel/shader/vk/frag2d.spv", std::ios::binary)
                              .value();
        fragmentShader = device_.CreateShaderModule(shaderDesc);
    }
}

Render3DContext::Render3DContext(rhi::APIPreference api, rhi::Device device,
                                 RenderContext& ctx)
    : device_{device} {
    initPipelineShader(api);
    bindGroupLayout = createBindGroupLayout(ctx);
    pipelineLayout = createPipelineLayout();
    pipeline = createPipeline(api, ctx);
}

Render3DContext::~Render3DContext() {
    vertexShader.Destroy();
    fragmentShader.Destroy();
    pipeline.Destroy();
    pipelineLayout.Destroy();
    bindGroupLayout.Destroy();
}

void Render3DContext::RecreatePipeline(rhi::APIPreference api,
                                       RenderContext& ctx) {
    pipeline.Destroy();
    pipeline = createPipeline(api, ctx);
}

rhi::PipelineLayout Render3DContext::createPipelineLayout() {
    rhi::PipelineLayout::Descriptor layoutDesc;
    layoutDesc.layouts.emplace_back(bindGroupLayout);
    rhi::PushConstantRange range;
    range.offset = 0;
    range.size = sizeof(nickel::cgmath::Mat44);
    range.stage = rhi::ShaderStage::Vertex;
    layoutDesc.pushConstRanges.emplace_back(range);
    return device_.CreatePipelineLayout(layoutDesc);
}

void Render3DContext::initPipelineShader(rhi::APIPreference api) {
    rhi::ShaderModule::Descriptor shaderDesc;

    if (api == rhi::APIPreference::GL) {
        shaderDesc.code =
            nickel::ReadWholeFile<std::vector<char>>("shader/gl/pbr.vert")
                .value();
        vertexShader = device_.CreateShaderModule(shaderDesc);

        shaderDesc.code =
            nickel::ReadWholeFile<std::vector<char>>("shader/gl/pbr.frag")
                .value();
        fragmentShader = device_.CreateShaderModule(shaderDesc);
    } else if (api == rhi::APIPreference::Vulkan) {
        shaderDesc.code = nickel::ReadWholeFile<std::vector<char>>(
                              "nickel/shader/vk/pbr_vert.spv", std::ios::binary)
                              .value();
        vertexShader = device_.CreateShaderModule(shaderDesc);

        shaderDesc.code = nickel::ReadWholeFile<std::vector<char>>(
                              "nickel/shader/vk/pbr_frag.spv", std::ios::binary)
                              .value();
        fragmentShader = device_.CreateShaderModule(shaderDesc);
    }
}

rhi::BindGroupLayout Render3DContext::createBindGroupLayout(
    RenderContext& ctx) {
    rhi::BindGroupLayout::Descriptor desc;

    // MVP uniform buffer
    {
        rhi::BufferBinding bufferBinding1;
        bufferBinding1.buffer = ctx.mvpBuffer;
        bufferBinding1.hasDynamicOffset = false;
        bufferBinding1.type = rhi::BufferType::Uniform;

        rhi::Entry entry;
        entry.arraySize = 1;
        entry.binding.binding = 0;
        entry.binding.entry = bufferBinding1;
        entry.visibility = rhi::ShaderStage::Vertex;
        desc.entries.emplace_back(entry);
    }

    // material uniform buffer
    {
        rhi::BufferBinding bufferBinding2;
        bufferBinding2.hasDynamicOffset = true;
        bufferBinding2.type = rhi::BufferType::Uniform;
        bufferBinding2.minBindingSize = sizeof(PBRParameters);

        rhi::Entry entry;
        entry.arraySize = 1;
        entry.binding.binding = 1;
        entry.binding.entry = bufferBinding2;
        entry.visibility = rhi::ShaderStage::Fragment;
        desc.entries.emplace_back(entry);
    }

    // base color texture
    {
        rhi::TextureBinding colorTextureBinding;
        colorTextureBinding.name = "baseColorTexture";
        colorTextureBinding.view = ctx.whiteTextureView;

        rhi::Entry entry;
        entry.arraySize = 1;
        entry.binding.binding = 2;
        entry.binding.entry = colorTextureBinding;
        entry.visibility = rhi::ShaderStage::Fragment;
        desc.entries.emplace_back(entry);
    }

    // normal map texture
    {
        rhi::TextureBinding normalTextureBinding;
        normalTextureBinding.name = "normalMapTexture";
        normalTextureBinding.view = ctx.whiteTextureView;

        rhi::Entry entry;
        entry.arraySize = 1;
        entry.binding.binding = 3;
        entry.binding.entry = normalTextureBinding;
        entry.visibility = rhi::ShaderStage::Fragment;
        desc.entries.emplace_back(entry);
    }

    // metalic roughness texture
    {
        rhi::TextureBinding binding;
        binding.name = "metalroughnessTexture";
        binding.view = ctx.whiteTextureView;

        rhi::Entry entry;
        entry.arraySize = 1;
        entry.binding.binding = 4;
        entry.binding.entry = binding;
        entry.visibility = rhi::ShaderStage::Fragment;
        desc.entries.emplace_back(entry);
    }

    // occlusion texture
    {
        rhi::TextureBinding binding;
        binding.view = ctx.whiteTextureView;
        binding.name = "occlusionTexture";

        rhi::Entry entry;
        entry.arraySize = 1;
        entry.binding.binding = 5;
        entry.binding.entry = binding;
        entry.visibility = rhi::ShaderStage::Fragment;
        desc.entries.emplace_back(entry);
    }

    // base color sampler
    {
        rhi::SamplerBinding colorTextureBinding;
        colorTextureBinding.type = rhi::SamplerBinding::SamplerType::Filtering;
        colorTextureBinding.name = "baseColorSampler";
        colorTextureBinding.sampler = ctx.defaultSampler;

        rhi::Entry entry;
        entry.arraySize = 1;
        entry.binding.binding = 6;
        entry.binding.entry = colorTextureBinding;
        entry.visibility = rhi::ShaderStage::Fragment;
        desc.entries.emplace_back(entry);
    }

    // normal map sampler
    {
        rhi::SamplerBinding normalTextureBinding;
        normalTextureBinding.name = "normalMapSampelr";
        normalTextureBinding.sampler = ctx.defaultSampler;

        rhi::Entry entry;
        entry.arraySize = 1;
        entry.binding.binding = 7;
        entry.binding.entry = normalTextureBinding;
        entry.visibility = rhi::ShaderStage::Fragment;
        desc.entries.emplace_back(entry);
    }

    // metalic roughness sampler
    {
        rhi::SamplerBinding binding;
        binding.name = "metalroughnessSampler";
        binding.sampler = ctx.defaultSampler;

        rhi::Entry entry;
        entry.arraySize = 1;
        entry.binding.binding = 8;
        entry.binding.entry = binding;
        entry.visibility = rhi::ShaderStage::Fragment;
        desc.entries.emplace_back(entry);
    }

    // occlusion sampler
    {
        rhi::SamplerBinding binding;
        binding.sampler = ctx.defaultSampler;
        binding.name = "occlusionSampler";

        rhi::Entry entry;
        entry.arraySize = 1;
        entry.binding.binding = 9;
        entry.binding.entry = binding;
        entry.visibility = rhi::ShaderStage::Fragment;
        desc.entries.emplace_back(entry);
    }

    // camera uniform buffer
    {
        rhi::BufferBinding bufferBinding;
        bufferBinding.hasDynamicOffset = false;
        bufferBinding.type = rhi::BufferType::Uniform;
        bufferBinding.minBindingSize = sizeof(nickel::cgmath::Vec3);
        bufferBinding.buffer = ctx.cameraBuffer;

        rhi::Entry entry;
        entry.arraySize = 1;
        entry.binding.binding = 10;
        entry.binding.entry = bufferBinding;
        entry.visibility = rhi::ShaderStage::Fragment;
        desc.entries.emplace_back(entry);
    }

    return device_.CreateBindGroupLayout(desc);
}

rhi::RenderPipeline Render3DContext::createPipeline(rhi::APIPreference api,
                                                    RenderContext& ctx) {
    rhi::RenderPipeline::Descriptor desc;

    desc.vertex.module = vertexShader;
    desc.fragment.module = fragmentShader;

    // position buffer
    {
        rhi::RenderPipeline::BufferState state;
        state.stepMode = rhi::RenderPipeline::BufferState::StepMode::Vertex;
        state.arrayStride = 12;
        rhi::RenderPipeline::BufferState::Attribute attr;
        attr.format = rhi::VertexFormat::Float32x3;
        attr.shaderLocation = 0;
        attr.offset = 0;
        state.attributes.push_back(attr);
        desc.vertex.buffers.emplace_back(std::move(state));
    }

    // uv buffer
    {
        rhi::RenderPipeline::BufferState state;
        rhi::RenderPipeline::BufferState::Attribute attr;
        state.arrayStride = 8;
        attr.format = rhi::VertexFormat::Float32x2;
        attr.shaderLocation = 1;
        attr.offset = 0;
        state.attributes.push_back(attr);
        desc.vertex.buffers.emplace_back(std::move(state));
    }

    // normal buffer
    {
        rhi::RenderPipeline::BufferState state;
        rhi::RenderPipeline::BufferState::Attribute attr;
        state.arrayStride = 12;
        attr.format = rhi::VertexFormat::Float32x3;
        attr.shaderLocation = 2;
        attr.offset = 0;
        state.attributes.push_back(attr);
        desc.vertex.buffers.emplace_back(std::move(state));
    }

    // tangent buffer
    {
        rhi::RenderPipeline::BufferState state;
        rhi::RenderPipeline::BufferState::Attribute attr;
        state.arrayStride = 16;
        attr.format = rhi::VertexFormat::Float32x4;
        attr.shaderLocation = 3;
        attr.offset = 0;
        state.attributes.push_back(attr);
        desc.vertex.buffers.emplace_back(std::move(state));
    }

    rhi::RenderPipeline::FragmentTarget target;
    target.blend.color.srcFactor = rhi::BlendFactor::SrcAlpha;
    target.blend.color.dstFactor = rhi::BlendFactor::OneMinusSrcAlpha;
    target.blend.color.operation = rhi::BlendOp::Add;
    target.blend.alpha.srcFactor = rhi::BlendFactor::One;
    target.blend.alpha.dstFactor = rhi::BlendFactor::Zero;
    target.format = rhi::TextureFormat::Presentation;
    desc.layout = pipelineLayout;
    desc.fragment.targets.emplace_back(target);

    rhi::RenderPipeline::DepthStencilState depthStencilState;
    depthStencilState.depthFormat = ctx.depthTexture.Format();
    depthStencilState.depthWriteEnabled = true;
    depthStencilState.depthCompare = rhi::CompareOp::Greater;
    desc.depthStencil = depthStencilState;
    desc.primitive.cullMode = rhi::CullMode::Back;

    return device_.CreateRenderPipeline(desc);
}

RenderContext::RenderContext(rhi::APIPreference api, rhi::Device device,
                             const cgmath::Vec2& windowSize) {
    initMVPBuffer(device);
    initDepthTexture(device, windowSize);
    cameraBuffer = createCameraBuffer(device);
    whiteTexture = createSingleValueTexture(device, 0xFFFFFFFF);
    whiteTextureView = whiteTexture.CreateView();
    blackTexture = createSingleValueTexture(device, 0x000000FF);
    blackTextureView = blackTexture.CreateView();
    defaultNormalTexture = createSingleValueTexture(device, 0xFFFF8080);
    defaultNormalTextureView = defaultNormalTexture.CreateView();
    defaultSampler = createDefaultSampler(device);
    ctx2D = std::make_unique<Render2DContext>(
        api, device, cgmath::Rect{0, 0, windowSize.w, windowSize.h}, *this);
    ctx3D = std::make_unique<Render3DContext>(api, device, *this);
}

RenderContext::~RenderContext() {
    defaultSampler.Destroy();
    defaultNormalTextureView.Destroy();
    defaultNormalTexture.Destroy();
    blackTextureView.Destroy();
    blackTexture.Destroy();
    whiteTextureView.Destroy();
    whiteTexture.Destroy();
    depthTextureView.Destroy();
    depthTexture.Destroy();
    cameraBuffer.Destroy();
    mvpBuffer.Destroy();
    ctx2D.reset();
    ctx3D.reset();
}

void RenderContext::RecreatePipeline(const cgmath::Vec2& size) {
    auto api =
        ECS::Instance().World().res<rhi::Adapter>()->RequestAdapterInfo().api;
    ctx2D->RecreatePipeline(api, *this);
    ctx3D->RecreatePipeline(api, *this);

    depthTextureView.Destroy();
    depthTexture.Destroy();

    auto device = ECS::Instance().World().res<rhi::Device>().get();
    initDepthTexture(device, size);
}

void RenderContext::OnWindowResize(
    const WindowResizeEvent& event,
    gecs::resource<gecs::mut<RenderContext>> ctx) {
    // ctx->RecreatePipeline(event.size);
}

void RenderContext::initDepthTexture(rhi::Device device,
                                     const nickel::cgmath::Vec2& size) {
    rhi::Texture::Descriptor desc;
    desc.format = rhi::TextureFormat::DEPTH24_PLUS_STENCIL8;
    desc.size.width = 1536;
    desc.size.height = 864;
    desc.size.depthOrArrayLayers = 1;
    desc.usage = rhi::TextureUsage::RenderAttachment;
    depthTexture = device.CreateTexture(desc);
    depthTextureView = depthTexture.CreateView();
}

void RenderContext::initMVPBuffer(rhi::Device device) {
    rhi::Buffer::Descriptor bufferDesc;
    bufferDesc.usage = rhi::BufferUsage::Uniform;
    bufferDesc.mappedAtCreation = true;
    bufferDesc.size = sizeof(nickel::cgmath::Mat44) * 2;
    mvpBuffer = device.CreateBuffer(bufferDesc);
}

rhi::Buffer RenderContext::createCameraBuffer(rhi::Device device) {
    rhi::Buffer::Descriptor bufferDesc;
    bufferDesc.usage = rhi::BufferUsage::Uniform;
    bufferDesc.mappedAtCreation = true;
    bufferDesc.size = sizeof(nickel::cgmath::Vec3);
    return device.CreateBuffer(bufferDesc);
}

rhi::Sampler RenderContext::createDefaultSampler(rhi::Device device) {
    rhi::Sampler::Descriptor desc;
    desc.u = rhi::SamplerAddressMode::Repeat;
    desc.v = rhi::SamplerAddressMode::Repeat;
    desc.w = rhi::SamplerAddressMode::Repeat;
    desc.min = rhi::Filter::Linear;
    desc.mag = rhi::Filter::Linear;
    return device.CreateSampler(desc);
}

}  // namespace nickel