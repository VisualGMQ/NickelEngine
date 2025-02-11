#include "nickel/common/common.hpp"
#include "nickel/graphics/lowlevel/common.hpp"
#include "nickel/main_entry/runtime.hpp"
#include "nickel/nickel.hpp"
#include "../common.hpp"

using namespace nickel::graphics;

struct MVP {
    nickel::Mat44 proj;
    nickel::Mat44 view;
    nickel::Mat44 model;
};

class Application : public nickel::Application {
public:
    void OnInit() override {
        RenderTestCommonContext::Init();
        
        auto& ctx = nickel::Context::GetInst();
        ctx.EnableRender(false);
        Device device = ctx.GetGPUAdapter().GetDevice();

        createMVPBuffer(device);
        initMVP();
        createBindGropuLayout(device);
        createDepthImage(device);
        loadImage(device);
        createSampler(device);
        createRenderPass(device);
        createPipelineLayout(device);
        createPipeline(device);
        createFramebuffers(device);
        createVertexBuffer(device);
        createBindGroup(device);
        bufferVertexData();
    }

    void OnUpdate() override {
        auto render_ctx = RenderTestCommonContext::GetInst();
        
        auto& window = nickel::Context::GetInst().GetWindow();
        if (window.IsMinimize()) {
            return;
        }

        LogicUpdate();

        Device device = nickel::Context::GetInst().GetGPUAdapter().GetDevice();
        
        RenderTestCommonContext::GetInst().BeginFrame();

        auto window_size = window.GetSize();

        CommandEncoder encoder = device.CreateCommandEncoder();

        std::vector<ClearValue> clear_values;

        {
            ClearValue clear_value;
            clear_value.m_value = std::array<float, 4>{0.1, 0.1, 0.1, 1};
            clear_values.push_back(clear_value);
        }
        {
            ClearValue clear_value;
            clear_value.m_value = ClearValue::DepthStencilValue{0.0, 0};
            clear_values.push_back(clear_value);
        }

        nickel::Rect render_area;
        render_area.position.x = 0;
        render_area.position.y = 0;
        render_area.size.w = window_size.w;
        render_area.size.h = window_size.h;
        RenderPassEncoder render_pass = encoder.BeginRenderPass(
            m_render_pass, m_framebuffers[render_ctx.CurFrameIdx()], render_area, clear_values);
        render_pass.SetViewport(0, 0, window_size.w, window_size.h, 0, 1);
        render_pass.SetScissor(0, 0, window_size.w, window_size.h);
        render_pass.BindGraphicsPipeline(m_pipeline);

        render_pass.BindVertexBuffer(0, m_vertex_buffer, 0);
        render_pass.SetBindGroup(0, m_bind_group);
        render_pass.Draw(36, 1, 0, 0);

        render_pass.End();
        Command cmd = encoder.Finish();
        
        device.Submit(cmd,
                      std::span{&render_ctx.GetImageAvaliableSemaphore(), 1},
                      std::span{&render_ctx.GetRenderFinishSemaphore(), 1},
                      render_ctx.GetFence());

        RenderTestCommonContext::GetInst().EndFrame();
    }
     
    void OnQuit() override {
        RenderTestCommonContext::Delete();
    }


private:
    GraphicsPipeline m_pipeline;
    RenderPass m_render_pass;
    PipelineLayout m_pipeline_layout;
    BindGroupLayout m_bind_layout;
    Buffer m_vertex_buffer;
    Buffer m_mvp_buffer;
    std::vector<Framebuffer> m_framebuffers;
    std::vector<Image> m_depth_images;
    std::vector<ImageView> m_depth_image_views;
    BindGroup m_bind_group;
    Sampler m_sampler;
    Image m_image;
    ImageView m_image_view;
    MVP m_mvp;

    void initMVP() {
        auto window_size = nickel::Context::GetInst().GetWindow().GetSize();
        float aspect = window_size.w / (float)window_size.h;

        m_mvp.model = nickel::Mat44::Identity();
        m_mvp.view = nickel::Mat44::Identity();
        m_mvp.proj =
            nickel::CreatePersp(nickel::Radians{nickel::Degrees{45.0f}},
                                aspect, 0.1f, 100.0f);
    }

    void LogicUpdate() {
        static float x = 0, y = 0;

        m_mvp_buffer.MapAsync();
        void* data = m_mvp_buffer.GetMappedRange();
        m_mvp.model = nickel::CreateTranslation<float>({0, 0, -5}) *
                      nickel::CreateXYZRotation<float>({x, y, 0});
        memcpy(data, &m_mvp, sizeof(m_mvp));

        m_mvp_buffer.Unmap();

        x += 0.001;
        y += 0.002;
    }

    void createBindGropuLayout(Device& device) {
        BindGroupLayout::Descriptor desc;
        {
            BindGroupLayout::Entry entry;
            entry.shader_stage = ShaderStage::Vertex;
            entry.type = BindGroupEntryType::UniformBuffer;
            entry.arraySize = 1;
            desc.entries[0] = std::move(entry);
        }
        {
            BindGroupLayout::Entry entry;
            entry.shader_stage = ShaderStage::Fragment;
            entry.type = BindGroupEntryType::CombinedImageSampler;
            entry.arraySize = 1;
            desc.entries[1] = std::move(entry);
        }

        m_bind_layout = device.CreateBindGroupLayout(desc);
    }

    void createBindGroup(Device& device) {
        BindGroup::Descriptor desc;
        {
            BindGroup::Entry entry;
            entry.shader_stage = ShaderStage::Vertex;
            BindGroup::BufferBinding binding;
            binding.buffer = m_mvp_buffer;
            binding.type = BindGroup::BufferBinding::Type::Uniform;
            entry.binding.entry = binding;
            entry.arraySize = 1;
            desc.entries[0] = entry;
        }

        {
            BindGroup::Entry entry;
            entry.shader_stage = ShaderStage::Fragment;
            BindGroup::CombinedSamplerBinding binding;
            binding.sampler = m_sampler;
            binding.view = m_image_view;
            entry.binding.entry = binding;
            entry.arraySize = 1;
            desc.entries[1] = entry;
        }

        m_bind_group = m_bind_layout.RequireBindGroup(desc);
    }

    void createSampler(Device& device) {
        Sampler::Descriptor desc;
        desc.minFilter = Filter::Nearest;
        desc.magFilter = Filter::Nearest;
        m_sampler = device.CreateSampler(desc);
    }

    void createDepthImage(Device& device) {
        for (uint32_t i = 0; i < device.GetSwapchainImageInfo().m_image_count;
             i++) {
            {
                auto size = nickel::Context::GetInst().GetWindow().GetSize();
                Image::Descriptor desc;
                desc.imageType = ImageType::Dim2;
                desc.extent.w = size.w;
                desc.extent.h = size.h;
                desc.extent.l = 1;
                desc.format = Format::D32_SFLOAT_S8_UINT;
                desc.usage = ImageUsage::DepthStencilAttachment;
                desc.tiling = ImageTiling::Optimal;
                m_depth_images.emplace_back(device.CreateImage(desc));
            }
            {
                ImageView::Descriptor view_desc;
                view_desc.format = Format::D32_SFLOAT_S8_UINT;
                view_desc.components = ComponentMapping::SwizzleIdentity;
                view_desc.subresourceRange.aspectMask =
                    nickel::Flags{ImageAspect::Depth} | ImageAspect::Stencil;
                view_desc.viewType = ImageViewType::Dim2;
                m_image_view = m_depth_images.back().CreateView(view_desc);
                m_depth_image_views.emplace_back(m_image_view);
            }
        }
    }

    void loadImage(Device& device) {
        ImageRawData raw_data{"./tests/render/resources/girl.png"};

        // create image
        {
            Image::Descriptor desc;
            desc.imageType = ImageType::Dim2;
            desc.extent.w = raw_data.GetExtent().w;
            desc.extent.h = raw_data.GetExtent().h;
            desc.extent.l = 1;
            desc.format = Format::R8G8B8A8_UNORM;
            desc.usage =
                nickel::Flags{ImageUsage::CopyDst} | ImageUsage::Sampled;
            m_image = device.CreateImage(desc);
        }

        // buffer data to image
        {
            Buffer::Descriptor desc;
            desc.m_memory_type = MemoryType::CPULocal;
            desc.m_size = 4 * raw_data.GetExtent().w * raw_data.GetExtent().h;
            desc.m_usage = BufferUsage::CopySrc;
            Buffer buffer = device.CreateBuffer(desc);
            buffer.MapAsync();
            void* data = buffer.GetMappedRange();
            memcpy(data, raw_data.GetData(), desc.m_size);
            buffer.Unmap();

            CommandEncoder encoder = device.CreateCommandEncoder();
            CopyEncoder copy = encoder.BeginCopy();
            CopyEncoder::BufferImageCopy copy_info;
            copy_info.bufferOffset = 0;
            copy_info.imageExtent.w = raw_data.GetExtent().w;
            copy_info.imageExtent.h = raw_data.GetExtent().h;
            copy_info.imageExtent.l = 1;
            copy_info.bufferImageHeight = 0;
            copy_info.bufferRowLength = 0;
            copy_info.imageSubresource.aspectMask = ImageAspect::Color;
            copy.CopyBufferToTexture(buffer, m_image, copy_info);
            copy.End();
            Command cmd = encoder.Finish();
            device.Submit(cmd, {}, {}, {});
            device.WaitIdle();
        }

        // create view
        {
            ImageView::Descriptor view_desc;
            view_desc.format = Format::R8G8B8A8_UNORM;
            view_desc.components = ComponentMapping::SwizzleIdentity;
            view_desc.subresourceRange.aspectMask = ImageAspect::Color;
            view_desc.viewType = ImageViewType::Dim2;
            m_image_view = m_image.CreateView(view_desc);
        }
    }

    void createVertexBuffer(Device& device) {
        Buffer::Descriptor desc;
        desc.m_size = sizeof(float) * 5 * 36;
        desc.m_usage = BufferUsage::Vertex;
        desc.m_memory_type = MemoryType::Coherence;
        m_vertex_buffer = device.CreateBuffer(desc);
    }

    void createMVPBuffer(Device& device) {
        Buffer::Descriptor desc;
        desc.m_size = sizeof(float) * 4 * 4 * 3;
        desc.m_usage = BufferUsage::Uniform;
        desc.m_memory_type = MemoryType::Coherence;
        m_mvp_buffer = device.CreateBuffer(desc);
    }

    void createPipelineLayout(Device& device) {
        PipelineLayout::Descriptor desc;
        desc.layouts.push_back(m_bind_layout);
        m_pipeline_layout = device.CreatePipelineLayout(desc);
    }

    void bufferVertexData() {
        m_vertex_buffer.MapAsync();
        void* map = m_vertex_buffer.GetMappedRange();
        // clang-format off
        static float vertices[] = {
           1, -1, 1,   0, 1,
           -1, -1, 1,  1, 1,
           -1, -1, -1, 1, 0,
           1, -1, -1,  0, 0,
           1, -1, 1,   0, 1,
           -1, -1, -1, 1, 0,
        
           1, 1, 1,    0, 1,
           1, -1, 1,   1, 1,
           1, -1, -1,  1, 0,
           1, 1, -1,   0, 0,
           1, 1, 1,    0, 1,
           1, -1, -1,  1, 0,
        
           -1, 1, 1,   0, 1,
           1, 1, 1,    1, 1,
           1, 1, -1,   1, 0,
           -1, 1, -1,  0, 0,
           -1, 1, 1,   0, 1,
           1, 1, -1,   1, 0,
        
           -1, -1, 1,  0, 1,
           -1, 1, 1,   1, 1,
           -1, 1, -1,  1, 0,
           -1, -1, -1, 0, 0,
           -1, -1, 1,  0, 1,
           -1, 1, -1,  1, 0,
        
           1, 1, 1,    0, 1,
           -1, 1, 1,   1, 1,
           -1, -1, 1,  1, 0,
           -1, -1, 1,  1, 0,
           1, -1, 1,   0, 0,
           1, 1, 1,    0, 1,
        
           1, -1, -1,  0, 1,
           -1, -1, -1, 1, 1,
           -1, 1, -1,  1, 0,
           1, 1, -1,   0, 0,
           1, -1, -1,  0, 1,
           -1, 1, -1,  1, 0,
        };
        // clang-format on
        memcpy(map, vertices, sizeof(vertices));

        m_vertex_buffer.Unmap();
    }

    void createRenderPass(Device& device) {
        RenderPass::Descriptor desc;

        {
            RenderPass::Descriptor::AttachmentDescription attachment;
            attachment.samples = SampleCount::Count1;
            attachment.initialLayout = ImageLayout::Undefined;
            attachment.finalLayout = ImageLayout::PresentSrcKHR;
            attachment.loadOp = AttachmentLoadOp::Clear;
            attachment.storeOp = AttachmentStoreOp::Store;
            attachment.stencilLoadOp = AttachmentLoadOp::DontCare;
            attachment.stencilStoreOp = AttachmentStoreOp::DontCare;
            attachment.format =
                device.GetSwapchainImageInfo().m_surface_format.format;
            desc.attachments.push_back(attachment);
        }

        {
            RenderPass::Descriptor::AttachmentDescription attachment;
            attachment.samples = SampleCount::Count1;
            attachment.initialLayout = ImageLayout::Undefined;
            attachment.finalLayout = ImageLayout::DepthStencilAttachmentOptimal;
            attachment.loadOp = AttachmentLoadOp::Clear;
            attachment.storeOp = AttachmentStoreOp::Store;
            attachment.stencilLoadOp = AttachmentLoadOp::DontCare;
            attachment.stencilStoreOp = AttachmentStoreOp::DontCare;
            attachment.format = Format::D32_SFLOAT_S8_UINT;
            desc.attachments.push_back(attachment);
        }

        RenderPass::Descriptor::SubpassDescription subpass;

        {
            RenderPass::Descriptor::AttachmentReference ref;
            ref.attachment = 0;
            ref.layout = ImageLayout::ColorAttachmentOptimal;
            subpass.colorAttachments.push_back(ref);
        }

        {
            RenderPass::Descriptor::AttachmentReference ref;
            ref.attachment = 1;
            ref.layout = ImageLayout::DepthStencilAttachmentOptimal;
            subpass.depthStencilAttachment = ref;
        }
        desc.subpasses.push_back(subpass);

        RenderPass::Descriptor::SubpassDependency deps;
        deps.srcSubpass =
            RenderPass::Descriptor::SubpassDependency::ExternalSubpass;
        deps.dstSubpass = 0;
        deps.srcAccessMask = Access::None;
        deps.dstAccessMask = Access::ColorAttachmentWrite;
        deps.srcStageMask = PipelineStage::TopOfPipe;
        deps.dstStageMask = PipelineStage::ColorAttachmentOutput;
        desc.dependencies.push_back(deps);

        m_render_pass = device.CreateRenderPass(desc);
    }

    void createPipeline(Device& device) {
        GraphicsPipeline::Descriptor desc;
        desc.m_render_pass = m_render_pass;
        desc.layout = m_pipeline_layout;

        auto vert_file_content =
            nickel::ReadWholeFile("./tests/render/cube3d/vert.spv");
        auto frag_file_content =
            nickel::ReadWholeFile("./tests/render/cube3d/frag.spv");

        ShaderModule vertex_shader = device.CreateShaderModule(
            (uint32_t*)vert_file_content.data(), vert_file_content.size());
        ShaderModule frag_shader = device.CreateShaderModule(
            (uint32_t*)frag_file_content.data(), frag_file_content.size());

        desc.m_shader_stages[ShaderStage::Vertex] = {vertex_shader};
        desc.m_shader_stages[ShaderStage::Fragment] = {frag_shader};

        GraphicsPipeline::Descriptor::BufferState state;

        // vertex position
        {
            GraphicsPipeline::Descriptor::BufferState::Attribute attr;
            attr.shaderLocation = 0;
            attr.format = VertexFormat::Float32x3;
            attr.offset = 0;
            state.attributes.push_back(attr);
        }

        // vertex color
        {
            GraphicsPipeline::Descriptor::BufferState::Attribute attr;
            attr.shaderLocation = 1;
            attr.format = VertexFormat::Float32x2;
            attr.offset = sizeof(float) * 3;
            state.attributes.push_back(attr);
        }

        state.arrayStride = sizeof(float) * 5;
        state.stepMode =
            GraphicsPipeline::Descriptor::BufferState::StepMode::Vertex;
        desc.vertex.buffers.push_back(state);

        GraphicsPipeline::Descriptor::BlendState blend_state;
        desc.blend_state.push_back(blend_state);

        {
            GraphicsPipeline::Descriptor::DepthStencilState depth_stencil_state;
            depth_stencil_state.depthFormat = Format::D32_SFLOAT_S8_UINT;
            desc.depthStencil = depth_stencil_state;
        }

        m_pipeline = device.CreateGraphicPipeline(desc);
    }

    void createFramebuffers(Device& device) {
        auto swapchain_image_views = device.GetSwapchainImageViews();
        auto image_extent = device.GetSwapchainImageInfo().m_extent;
        for (uint32_t i = 0; i < device.GetSwapchainImageInfo().m_image_count;
             i++) {
            Framebuffer::Descriptor desc;
            desc.m_extent.w = image_extent.w;
            desc.m_extent.h = image_extent.h;
            desc.m_extent.l = 1;
            desc.m_render_pass = m_render_pass;
            desc.m_views = {swapchain_image_views[i], m_depth_image_views[i]};
            m_framebuffers.push_back(device.CreateFramebuffer(desc));
        }
    }
};

NICKEL_RUN_APP(Application);