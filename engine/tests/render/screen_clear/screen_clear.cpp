#include "nickel/nickel.hpp"
#include "nickel/main_entry/runtime.hpp"

class Application: public nickel::Application {
public:
    void OnInit() override {
        nickel::graphics::Device device =
            nickel::Context::GetInst().GetGPUAdapter().GetDevice();

        createRenderPass(device);
    }
    
    void OnUpdate() override {
    }
    
private:
    nickel::graphics::GraphicsPipeline m_pipeline;
    nickel::graphics::RenderPass m_render_pass;

    void createRenderPass(nickel::graphics::Device device) {
        nickel::graphics::RenderPass::Descriptor desc;
        VkAttachmentDescription attachment{};
        attachment.format = device.GetSwapchainImageInfo().format.format;
        attachment.samples = VK_SAMPLE_COUNT_1_BIT;
        attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        attachment.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        attachment.storeOp= VK_ATTACHMENT_STORE_OP_STORE;
        attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        desc.attachments.push_back(attachment);

        VkSubpassDescription subpass{};
        subpass.colorAttachmentCount = 1;
        VkAttachmentReference color_attachment{};
        color_attachment.attachment = 0;
        color_attachment.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        subpass.pColorAttachments = &color_attachment;
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        desc.subpasses.push_back(subpass);
        
        m_render_pass = device.CreateRenderPass(desc);
    }

    void createPipeline(nickel::graphics::Device device) {
        nickel::graphics::GraphicsPipeline::Descriptor desc;
        desc.m_render_pass = m_render_pass;
        desc.m_subpass = 0;
        // TODO: not finish
        
        m_pipeline = device.CreateGraphicPipeline(desc);
    }
};

std::unique_ptr<nickel::Application> CreateCustomApplication(nickel::Context&) {
    return std::make_unique<Application>();
}
