#include "nickel/common/common.hpp"
#include "nickel/main_entry/runtime.hpp"
#include "nickel/nickel.hpp"

#include <3rdlibs/SDL/src/video/khronos/vulkan/vulkan_core.h>

using namespace nickel::graphics;

class Application : public nickel::Application {
public:
    void OnInit() override {
        Device device = nickel::Context::GetInst().GetGPUAdapter().GetDevice();

        createRenderPass(device);
        createPipelineLayout(device);
        createPipeline(device);
        createFramebuffers(device);
    }

    void OnUpdate() override {
        auto& window = nickel::Context::GetInst().GetWindow();
        if (window.IsMinimize()) {
            return;
        }

        Device device = nickel::Context::GetInst().GetGPUAdapter().GetDevice();
        uint32_t idx = device.WaitAndAcquireSwapchainImageIndex();

        auto window_size = window.GetSize();

        CommandEncoder encoder = device.CreateCommandEncoder();
        ClearValue clear_value;
        clear_value.m_color_value = std::array<float, 4>{0.1, 0.1, 0.1, 1};
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
        render_pass.Draw(3, 1, 0, 0);

        render_pass.End();
        Command cmd = encoder.Finish();
        
        device.Submit(cmd);
        device.EndFrame();
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
            nickel::ReadWholeFile("./tests/render/screen_clear/vert.spv");
        auto frag_file_content =
            nickel::ReadWholeFile("./tests/render/screen_clear/frag.spv");

        ShaderModule vertex_shader = device.CreateShaderModule(
            (uint32_t*)vert_file_content.data(), vert_file_content.size());
        ShaderModule frag_shader = device.CreateShaderModule(
            (uint32_t*)frag_file_content.data(), frag_file_content.size());

        desc.m_shader_stages[ShaderStage::Vertex] = {vertex_shader};
        desc.m_shader_stages[ShaderStage::Fragment] = {frag_shader};

        GraphicsPipeline::Descriptor::BlendState blend_state;
        desc.blend_state.push_back(blend_state);

        m_pipeline = device.CreateGraphicPipeline(desc);
    }

    void createFramebuffers(Device& device) {
        auto images = device.GetSwapchainImages();
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

std::unique_ptr<nickel::Application> CreateCustomApplication(nickel::Context&) {
    return std::make_unique<Application>();
}

