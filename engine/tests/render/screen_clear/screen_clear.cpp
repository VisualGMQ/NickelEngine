#include "nickel/main_entry/runtime.hpp"
#include "nickel/nickel.hpp"

using namespace nickel::graphics;

class Application : public nickel::Application {
public:
    void OnInit() override {
        Device device = nickel::Context::GetInst().GetGPUAdapter().GetDevice();

        createRenderPass(device);
    }

    void OnUpdate() override {}

private:
    GraphicsPipeline m_pipeline;
    RenderPass m_render_pass;

    void createRenderPass(Device device) {
        RenderPass::Descriptor desc;
        RenderPass::Descriptor::AttachmentDescription attachment;
        attachment.samples = SampleCount::Count1;
        attachment.initialLayout = ImageLayout::Undefined;
        attachment.finalLayout = ImageLayout::ColorAttachmentOptimal;
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

        m_render_pass = device.CreateRenderPass(desc);
    }

    void createPipeline(Device device) {
        GraphicsPipeline::Descriptor desc;
        desc.m_render_pass = m_render_pass;
        // TODO: not finish

        m_pipeline = device.CreateGraphicPipeline(desc);
    }
};

std::unique_ptr<nickel::Application> CreateCustomApplication(nickel::Context&) {
    return std::make_unique<Application>();
}

