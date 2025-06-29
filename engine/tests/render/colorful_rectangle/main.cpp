#include "../common.hpp"
#include "nickel/common/common.hpp"
#include "nickel/graphics/lowlevel/common.hpp"
#include "nickel/main_entry/runtime.hpp"
#include "nickel/nickel.hpp"

using namespace nickel::graphics;

class Application : public nickel::Application {
public:
    void OnInit() override {
        RenderTestCommonContext::Init();

        auto& ctx = nickel::Context::GetInst();
        ctx.EnableRender(false);
        Device device = ctx.GetGPUAdapter().GetDevice();

        createBindGropuLayout(device);
        createRenderPass(device);
        createPipelineLayout(device);
        createPipeline(device);
        createFramebuffers(device);
        createVertexBuffer(device);
        createIndicesBuffer(device);
        createColorBuffer(device);
        createBindGroup(device);
        bufferIndicesData();
        bufferVertexData();
    }

    void OnQuit() override { RenderTestCommonContext::Delete(); }

    void OnUpdate(float delta_time) override {
        auto& render_ctx = RenderTestCommonContext::GetInst();

        auto& window = nickel::Context::GetInst().GetWindow();
        if (window.IsMinimize()) {
            return;
        }

        Device device = nickel::Context::GetInst().GetGPUAdapter().GetDevice();

        RenderTestCommonContext::GetInst().BeginFrame();

        auto window_size = window.GetSize();

        if (m_increase) {
            m_color.r += 0.0001f;
        } else {
            m_color.r -= 0.0001f;
        }

        if (m_color.r > 1) {
            m_increase = false;
            m_color.r = 1;
        }
        if (m_color.r < 0) {
            m_increase = true;
            m_color.r = 0;
        }

        bufferColorData(m_color);

        CommandEncoder encoder = device.CreateCommandEncoder();
        ClearValue clear_value;
        clear_value.m_value = std::array<float, 4>{0.1, 0.1, 0.1, 1};
        nickel::Rect render_area;
        render_area.position.x = 0;
        render_area.position.y = 0;
        render_area.size.w = window_size.w;
        render_area.size.h = window_size.h;
        ClearValue values[] = {clear_value};
        RenderPassEncoder render_pass = encoder.BeginRenderPass(
            m_render_pass, m_framebuffers[render_ctx.CurFrameIdx()],
            render_area, std::span{values});
        render_pass.SetViewport(0, 0, window_size.w, window_size.h, 0, 1);
        render_pass.SetScissor(0, 0, window_size.w, window_size.h);
        render_pass.BindGraphicsPipeline(m_pipeline);

        render_pass.BindVertexBuffer(0, m_vertex_buffer, 0);
        render_pass.BindIndexBuffer(m_index_buffer, IndexType::Uint32, 0);
        render_pass.SetBindGroup(0, m_bind_group);
        render_pass.DrawIndexed(6, 1, 0, 0, 0);

        render_pass.End();
        Command cmd = encoder.Finish();

        device.Submit(cmd,
                      std::span{&render_ctx.GetImageAvaliableSemaphore(), 1},
                      std::span{&render_ctx.GetRenderFinishSemaphore(), 1},
                      render_ctx.GetFence());

        RenderTestCommonContext::GetInst().EndFrame();
    }

private:
    GraphicsPipeline m_pipeline;
    RenderPass m_render_pass;
    PipelineLayout m_pipeline_layout;
    BindGroupLayout m_bind_layout;
    Buffer m_vertex_buffer;
    Buffer m_index_buffer;
    Buffer m_color_buffer;
    std::vector<Framebuffer> m_framebuffers;
    BindGroup m_bind_group;
    nickel::Color m_color;
    bool m_increase = false;

    void createBindGropuLayout(Device& device) {
        BindGroupLayout::Descriptor desc;
        BindGroupLayout::Entry image_entry;
        image_entry.m_shader_stage = ShaderStage::Fragment;
        image_entry.m_type = BindGroupEntryType::UniformBuffer;
        image_entry.m_array_size = 1;
        desc.m_entries[0] = std::move(image_entry);

        m_bind_layout = device.CreateBindGroupLayout(desc);
    }

    void createBindGroup(Device& device) {
        BindGroup::Descriptor desc;
        BindGroup::Entry entry;
        entry.m_shader_stage = ShaderStage::Fragment;
        BindGroup::BufferBinding binding;
        binding.m_type = BindGroup::BufferBinding::Type::Uniform;
        binding.m_buffer = m_color_buffer;
        entry.m_binding.m_entry = binding;
        entry.m_array_size = 1;
        desc.m_entries[0] = entry;

        m_bind_group = m_bind_layout.RequireBindGroup(desc);
    }

    void createVertexBuffer(Device& device) {
        Buffer::Descriptor desc;
        desc.m_size = sizeof(float) * 4 * 4;
        desc.m_usage = BufferUsage::Vertex;
        desc.m_memory_type = MemoryType::Coherence;
        m_vertex_buffer = device.CreateBuffer(desc);
    }

    void createIndicesBuffer(Device& device) {
        Buffer::Descriptor desc;
        desc.m_size = sizeof(uint32_t) * 6;
        desc.m_usage = nickel::Flags{BufferUsage::Index} | BufferUsage::CopyDst;
        desc.m_memory_type = MemoryType::GPULocal;
        m_index_buffer = device.CreateBuffer(desc);
    }

    void createColorBuffer(Device& device) {
        Buffer::Descriptor desc;
        desc.m_size = sizeof(float) * 3;
        desc.m_usage = BufferUsage::Uniform;
        desc.m_memory_type = MemoryType::Coherence;
        m_color_buffer = device.CreateBuffer(desc);
    }

    void createPipelineLayout(Device& device) {
        PipelineLayout::Descriptor desc;
        desc.m_layouts.push_back(m_bind_layout);
        m_pipeline_layout = device.CreatePipelineLayout(desc);
    }

    void bufferVertexData() {
        m_vertex_buffer.MapAsync();
        void* map = m_vertex_buffer.GetMappedRange();
        // clang-format off
        float datas[] = {
            // position
            -0.5, -0.5,
             0.5, -0.5,
            -0.5,  0.5,
             0.5,  0.5,
        };
        // clang-format on
        memcpy(map, datas, sizeof(datas));

        m_vertex_buffer.Unmap();
    }

    void bufferColorData(const nickel::Color& color) {
        m_color_buffer.MapAsync();
        void* map = m_color_buffer.GetMappedRange();
        memcpy(map, color.Ptr(), sizeof(float) * 3);
        m_color_buffer.Unmap();
    }

    void bufferIndicesData() {
        uint32_t indices[] = {
            0, 1, 2, 1, 2, 3,
        };
        m_index_buffer.BuffData(&indices, sizeof(indices), 0);
    }

    void createRenderPass(Device& device) {
        RenderPass::Descriptor desc;
        RenderPass::Descriptor::AttachmentDescription attachment;
        attachment.m_samples = SampleCount::Count1;
        attachment.m_initial_layout = ImageLayout::Undefined;
        attachment.m_final_layout = ImageLayout::PresentSrcKHR;
        attachment.m_load_op = AttachmentLoadOp::Clear;
        attachment.m_store_op = AttachmentStoreOp::Store;
        attachment.m_stencil_load_op = AttachmentLoadOp::DontCare;
        attachment.m_stencil_store_op = AttachmentStoreOp::DontCare;
        attachment.m_format =
            device.GetSwapchainImageInfo().m_surface_format.format;
        desc.m_attachments.push_back(attachment);

        RenderPass::Descriptor::SubpassDescription subpass;
        RenderPass::Descriptor::AttachmentReference ref;
        ref.m_attachment = 0;
        ref.m_layout = ImageLayout::ColorAttachmentOptimal;
        subpass.m_color_attachments.push_back(ref);
        desc.m_subpasses.push_back(subpass);

        RenderPass::Descriptor::SubpassDependency deps;
        deps.m_src_subpass =
            RenderPass::Descriptor::SubpassDependency::ExternalSubpass;
        deps.m_dst_subpass = 0;
        deps.m_src_access_mask = Access::None;
        deps.m_dst_access_mask = Access::ColorAttachmentWrite;
        deps.m_src_stage_mask = PipelineStage::TopOfPipe;
        deps.m_dst_stage_mask = PipelineStage::ColorAttachmentOutput;
        desc.m_dependencies.push_back(deps);

        m_render_pass = device.CreateRenderPass(desc);
    }

    void createPipeline(Device& device) {
        GraphicsPipeline::Descriptor desc;
        desc.m_render_pass = m_render_pass;
        desc.m_layout = m_pipeline_layout;

        auto engine_relative_path = nickel::Context::GetInst().GetEngineRelativePath();
        auto vert_file_content = nickel::ReadWholeFile(
            engine_relative_path / "tests/render/colorful_rectangle/shader.vert.spv");

        auto frag_file_content = nickel::ReadWholeFile(
            engine_relative_path / "tests/render/colorful_rectangle/shader.frag.spv");

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
            attr.m_shader_location = 0;
            attr.m_format = VertexFormat::Float32x2;
            attr.m_offset = 0;
            state.m_attributes.push_back(attr);
        }

        state.m_array_stride = sizeof(float) * 2;
        state.m_step_mode =
            GraphicsPipeline::Descriptor::BufferState::StepMode::Vertex;
        desc.m_vertex.m_buffers.push_back(state);

        GraphicsPipeline::Descriptor::BlendState blend_state;
        desc.m_blend_state.push_back(blend_state);

        m_pipeline = device.CreateGraphicPipeline(desc);
    }

    void createFramebuffers(Device& device) {
        auto images = device.GetSwapchainImageViews();
        auto image_extent = device.GetSwapchainImageInfo().m_extent;
        for (auto& image : images) {
            Framebuffer::Descriptor desc;
            desc.m_extent.w = image_extent.w;
            desc.m_extent.h = image_extent.h;
            desc.m_extent.l = 1;
            desc.m_render_pass = m_render_pass;
            desc.m_views = {image};
            m_framebuffers.push_back(device.CreateFramebuffer(desc));
        }
    }
};

NICKEL_RUN_APP(Application);