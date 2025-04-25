#include "../common.hpp"
#include "nickel/common/common.hpp"
#include "nickel/main_entry/runtime.hpp"
#include "nickel/nickel.hpp"

#include <3rdlibs/SDL/src/video/khronos/vulkan/vulkan_core.h>

using namespace nickel::graphics;

class Application : public nickel::Application {
public:
    void OnInit() override {
        RenderTestCommonContext::Init();

        auto& ctx = nickel::Context::GetInst();
        ctx.EnableRender(false);

        Device device = ctx.GetGPUAdapter().GetDevice();

        createRenderPass(device);
        createPipelineLayout(device);
        createPipeline(device);
        createFramebuffers(device);
    }

    void OnQuit() override { RenderTestCommonContext::Delete(); }

    void OnUpdate(float delta_time) override {
        auto& window = nickel::Context::GetInst().GetWindow();
        if (window.IsMinimize()) {
            return;
        }

        auto& render_ctx = RenderTestCommonContext::GetInst();

        Device device = nickel::Context::GetInst().GetGPUAdapter().GetDevice();

        RenderTestCommonContext::GetInst().BeginFrame();

        auto window_size = window.GetSize();

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
        render_pass.Draw(3, 1, 0, 0);

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
    std::vector<Framebuffer> m_framebuffers;

    void createPipelineLayout(Device& device) {
        PipelineLayout::Descriptor desc;
        m_pipeline_layout = device.CreatePipelineLayout(desc);
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

        auto engine_relative_path =
            nickel::Context::GetInst().GetEngineRelativePath();
        auto vert_file_content =
            nickel::ReadWholeFile(engine_relative_path / "tests/render/triangle/vert.spv");
        auto frag_file_content =
            nickel::ReadWholeFile(engine_relative_path / "tests/render/triangle/frag.spv");

        ShaderModule vertex_shader = device.CreateShaderModule(
            (uint32_t*)vert_file_content.data(), vert_file_content.size());
        ShaderModule frag_shader = device.CreateShaderModule(
            (uint32_t*)frag_file_content.data(), frag_file_content.size());

        desc.m_shader_stages[ShaderStage::Vertex] = {vertex_shader};
        desc.m_shader_stages[ShaderStage::Fragment] = {frag_shader};

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
