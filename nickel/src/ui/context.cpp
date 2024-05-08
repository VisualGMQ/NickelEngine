#include "ui/context.hpp"

namespace nickel::ui {

RenderUIContext::RenderUIContext(rhi::APIPreference api, rhi::Device device,
                                 RenderContext& ctx,
                                 const cgmath::Vec2& windowSize)
    : device_{device},
      camera{Camera::CreateOrtho(0, windowSize.w, -windowSize.h, 0, 1, 0,
                                 {0, 0, windowSize.w, windowSize.h})} {
    bindGroupLayout = createBindGroupLayout(ctx);
    pipelineLayout = createPipelineLayout();
    initPipelineShader(api);
    initPipelines(api);
    vertexBuffer = createVertexBuffer();
    indexBuffer = createIndexBuffer();
    initDefaultBindGroup();
}

RenderUIContext::~RenderUIContext() {
    for (auto&& [_, bindGroup] : bindGroups) {
        bindGroup.Destroy();
    }
    defaultBindGroup.Destroy();

    indexBuffer.Destroy();
    vertexBuffer.Destroy();
    fillPipeline.Destroy();
    linePipeline.Destroy();
    vertexShader.Destroy();
    fragmentShader.Destroy();
    pipelineLayout.Destroy();
    bindGroupLayout.Destroy();
}

void RenderUIContext::AddElement(const cgmath::Rect& rect,
                                 const cgmath::Color& color,
                                 const TextureClip& textureClip,
                                 rhi::RenderPipeline pipeline) {
    auto& pos = rect.position;
    auto& size = rect.size;

    std::array vertices{
        UIVertex{                  {pos.x, pos.y},textureClip.uv0,color                                                                   },
        UIVertex{         {pos.x + size.w, pos.y},
                 {textureClip.uv1.x, textureClip.uv0.y},
                 color                                                   },
        UIVertex{         {pos.x, pos.y + size.h},
                 {textureClip.uv0.x, textureClip.uv1.y},
                 color                                                   },
        UIVertex{{pos.x + size.w, pos.y + size.h}, textureClip.uv1, color},
    };

    if (batchBreakInfos.empty()) {
        BatchBreakInfo breakInfo;
        breakInfo.texture = textureClip.handle;
        breakInfo.pipeline = pipeline;
        breakInfo.start = this->vertices.size();
        breakInfo.count++;

        batchBreakInfos.emplace_back(std::move(breakInfo));
    } else {
        auto& lastBatch = batchBreakInfos.back();
        if (lastBatch.texture != textureClip.handle ||
            lastBatch.pipeline != pipeline) {
            BatchBreakInfo breakInfo;
            breakInfo.texture = textureClip.handle;
            breakInfo.pipeline = pipeline;
            breakInfo.start = this->vertices.size();
            breakInfo.count++;

            batchBreakInfos.emplace_back(std::move(breakInfo));
        }
    }

    for (int i = 0; i < vertices.size(); i++) {
        this->vertices.emplace_back(vertices[i]);
    }
}

void RenderUIContext::RecreatePipeline(rhi::APIPreference api) {
    fillPipeline.Destroy();
    linePipeline.Destroy();
    initPipelines(api);
}

rhi::PipelineLayout RenderUIContext::createPipelineLayout() {
    rhi::PipelineLayout::Descriptor layoutDesc;
    layoutDesc.layouts.emplace_back(bindGroupLayout);
    rhi::PushConstantRange range;
    range.offset = 0;
    range.size = sizeof(nickel::cgmath::Mat44) * 2;
    range.stage = rhi::ShaderStage::Vertex;
    layoutDesc.pushConstRanges.emplace_back(range);
    return device_.CreatePipelineLayout(layoutDesc);
}

void RenderUIContext::initPipelineShader(rhi::APIPreference api) {
    rhi::ShaderModule::Descriptor shaderDesc;

    if (api == rhi::APIPreference::GL) {
        shaderDesc.code =
            nickel::ReadWholeFile<std::vector<char>>("shader/gl/ui.vert")
                .value();
        vertexShader = device_.CreateShaderModule(shaderDesc);

        shaderDesc.code =
            nickel::ReadWholeFile<std::vector<char>>("shader/gl/ui.frag")
                .value();
        fragmentShader = device_.CreateShaderModule(shaderDesc);
    } else if (api == rhi::APIPreference::Vulkan) {
        shaderDesc.code = nickel::ReadWholeFile<std::vector<char>>(
                              "nickel/shader/vk/ui_vert.spv", std::ios::binary)
                              .value();
        vertexShader = device_.CreateShaderModule(shaderDesc);

        shaderDesc.code = nickel::ReadWholeFile<std::vector<char>>(
                              "nickel/shader/vk/ui_frag.spv", std::ios::binary)
                              .value();
        fragmentShader = device_.CreateShaderModule(shaderDesc);
    }
}

rhi::BindGroupLayout RenderUIContext::createBindGroupLayout(
    RenderContext& ctx) {
    rhi::BindGroupLayout::Descriptor desc;

    // sampler2D
    {
        rhi::SamplerBinding binding;
        binding.sampler =
            ctx.ctx2D->GetSampler(rhi::SamplerAddressMode::ClampToEdge,
                                  rhi::SamplerAddressMode::ClampToEdge,
                                  rhi::Filter::Linear, rhi::Filter::Linear);
        binding.name = "mySampler";

        rhi::Entry entry;
        entry.arraySize = 1;
        entry.binding.binding = 0;
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
        entry.binding.binding = 1;
        entry.binding.entry = binding;
        entry.visibility = rhi::ShaderStage::Fragment;
        desc.entries.emplace_back(entry);
    }

    return device_.CreateBindGroupLayout(desc);
}

void RenderUIContext::initPipelines(rhi::APIPreference api) {
    rhi::RenderPipeline::Descriptor desc;

    initPipelineShader(api);

    desc.vertex.module = vertexShader;
    desc.fragment.module = fragmentShader;

    rhi::RenderPipeline::BufferState bufferState{};
    bufferState.attributes.emplace_back(
        rhi::RenderPipeline::BufferState::Attribute{
            rhi::VertexFormat::Float32x3, 0, 0});
    bufferState.attributes.emplace_back(
        rhi::RenderPipeline::BufferState::Attribute{
            rhi::VertexFormat::Float32x2, 3 * 4, 1});
    bufferState.attributes.emplace_back(
        rhi::RenderPipeline::BufferState::Attribute{
            rhi::VertexFormat::Float32x4, 5 * 4, 2});
    bufferState.arrayStride = 9 * 4;
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
    desc.primitive.topology = rhi::Topology::TriangleList;
    desc.primitive.cullMode = rhi::CullMode::None;

    fillPipeline = device_.CreateRenderPipeline(desc);

    desc.primitive.topology = rhi::Topology::LineList;
    linePipeline = device_.CreateRenderPipeline(desc);
}

rhi::Buffer RenderUIContext::createIndexBuffer() {
    rhi::Buffer::Descriptor desc;
    desc.mappedAtCreation = true;
    desc.size = sizeof(uint32_t) * 6 * MaxRectSize;
    desc.usage = rhi::BufferUsage::Index;

    auto buffer = device_.CreateBuffer(desc);
    auto ptr = (uint32_t*)buffer.GetMappedRange();
    uint32_t indices[] = {0, 1, 2, 1, 2, 3};
    for (int i = 0; i < MaxRectSize; i++) {
        memcpy(ptr + i * 6, indices, sizeof(indices));
    }
    buffer.Unmap();

    return buffer;
}

rhi::Buffer RenderUIContext::createVertexBuffer() {
    rhi::Buffer::Descriptor desc;
    desc.mappedAtCreation = true;
    desc.size = sizeof(float) * 9 * MaxRectSize;
    desc.usage = rhi::BufferUsage::Vertex;

    return device_.CreateBuffer(desc);
}

rhi::BindGroup RenderUIContext::FindBindGroup(TextureHandle handle) {
    if (auto it = bindGroups.find(handle); it != bindGroups.end()) {
        return it->second;
    } else {
        auto mgr = ECS::Instance().World().res<TextureManager>();
        if (mgr->Has(handle)) {
            auto& texture = mgr->Get(handle);
            rhi::BindGroup::Descriptor desc;
            desc.layout = bindGroupLayout;
            rhi::BindingPoint binding;
            binding.binding = 1;
            rhi::TextureBinding entry;
            entry.view = texture.View();
            binding.entry = entry;
            desc.entries.push_back(binding);

            return bindGroups.emplace(handle, device_.CreateBindGroup(desc))
                .first->second;
        } else {
            return defaultBindGroup;
        }
    }
}

rhi::TextureView RenderUIContext::GetFontChar(FontHandle handle, uint32_t code) {
    auto mgr = ECS::Instance().World().res<FontManager>();
    if (!mgr->Has(handle)) {
        return {};
    }

    auto& font = mgr->Get(handle);

    constexpr int size = 20;    // TODO: use custom size to generate font

    auto glyph = font.GetGlyph(code, size);
}

void RenderUIContext::initDefaultBindGroup() {
    rhi::BindGroup::Descriptor desc;
    desc.layout = bindGroupLayout;
    defaultBindGroup = device_.CreateBindGroup(desc);
}

UIContext::UIContext(rhi::APIPreference api, rhi::Device device,
                     RenderContext& ctx, const cgmath::Vec2& windowSize)
    : renderCtx(api, device, ctx, windowSize) {}

}  // namespace nickel::ui