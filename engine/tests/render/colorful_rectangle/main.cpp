#include "nickel/common/common.hpp"
#include "nickel/graphics/common.hpp"
#include "nickel/main_entry/runtime.hpp"
#include "nickel/nickel.hpp"

using namespace nickel::graphics;

class Application : public nickel::Application {
public:
    void OnInit() override {
        Device device = nickel::Context::GetInst().GetGPUAdapter().GetDevice();

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

    void OnUpdate() override {
        auto& window = nickel::Context::GetInst().GetWindow();
        if (window.IsMinimize()) {
            return;
        }

        Device device = nickel::Context::GetInst().GetGPUAdapter().GetDevice();
        uint32_t idx = device.WaitAndAcquireSwapchainImageIndex();

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
        RenderPassEncoder render_pass = encoder.BeginRenderPass(
            m_render_pass, m_framebuffers[idx], render_area, {clear_value});
        render_pass.SetViewport(0, 0, window_size.w, window_size.h, 0, 1);
        render_pass.SetScissor(0, 0, window_size.w, window_size.h);
        render_pass.BindGraphicsPipeline(m_pipeline);
        
        render_pass.BindVertexBuffer(0, m_vertex_buffer, 0);
        render_pass.BindIndexBuffer(m_index_buffer, IndexType::Uint32, 0);
        render_pass.SetBindGroup(m_bind_group);
        render_pass.DrawIndexed(6, 1, 0, 0, 0);

        render_pass.End();
        Command cmd = encoder.Finish();

        device.Submit(cmd);
        device.EndFrame();
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
        image_entry.shader_stage = ShaderStage::Fragment;
        image_entry.type = BindGroupEntryType::UniformBuffer;
        image_entry.arraySize = 1;
        desc.entries[0] = std::move(image_entry);

        m_bind_layout = device.CreateBindGroupLayout(desc);
    }

    void createBindGroup(Device& device) {
        BindGroup::Descriptor desc;
        BindGroup::Entry entry;
        entry.shader_stage = ShaderStage::Fragment;
        BindGroup::BufferBinding binding;
        binding.type = BindGroup::BufferBinding::Type::Uniform;
        binding.buffer = m_color_buffer;
        entry.binding.entry = binding;
        entry.arraySize = 1;
        desc.entries[0] = entry;

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
        desc.layouts.push_back(m_bind_layout);
        m_pipeline_layout = device.CreatePipelineLayout(desc);
    }

    void bufferVertexData() {
        m_vertex_buffer.MapAsync();
        void* map = m_vertex_buffer.GetMappedRange();
        // clang-format off
        float datas[] = {
            // position, texcoord
            -0.5, -0.5,  0, 0,
             0.5, -0.5,  1, 0,
            -0.5,  0.5,  0, 1,
             0.5,  0.5,  1, 1,
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

        RenderPass::Descriptor::SubpassDescription subpass;
        RenderPass::Descriptor::AttachmentReference ref;
        ref.attachment = 0;
        ref.layout = ImageLayout::ColorAttachmentOptimal;
        subpass.colorAttachments.push_back(ref);
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
            nickel::ReadWholeFile("./tests/render/colorful_rectangle/vert.spv");

        auto frag_file_content =
            nickel::ReadWholeFile("./tests/render/colorful_rectangle/frag.spv");

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
            attr.format = VertexFormat::Float32x2;
            attr.offset = 0;
            state.attributes.push_back(attr);
        }

        // vertex color
        {
            GraphicsPipeline::Descriptor::BufferState::Attribute attr;
            attr.shaderLocation = 1;
            attr.format = VertexFormat::Float32x2;
            attr.offset = sizeof(float) * 2;
            state.attributes.push_back(attr);
        }

        state.arrayStride = sizeof(float) * 4;
        state.stepMode =
            GraphicsPipeline::Descriptor::BufferState::StepMode::Vertex;
        desc.vertex.buffers.push_back(state);

        GraphicsPipeline::Descriptor::BlendState blend_state;
        desc.blend_state.push_back(blend_state);

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