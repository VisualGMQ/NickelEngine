#include "nickel.hpp"
#include "misc/project.hpp"
#include "rhi/enums.hpp"
#include "rhi/vulkan/instance.hpp"
#include "rhi/vulkan/device.hpp"
#include "rhi/vulkan/surface.hpp"
#include "rhi/vulkan/swapchain.hpp"
#include "rhi/vulkan/pipeline.hpp"
#include "rhi/vertex.hpp"
#include "rhi/vertex_layout.hpp"

void StartupSystem(gecs::resource<gecs::mut<nickel::Window>> window) {
    nickel::rhi::vulkan::Instance instance(window.get());
    nickel::rhi::vulkan::Surface surface(instance);
    nickel::rhi::vulkan::Device dev(instance, &surface);
    nickel::rhi::vulkan::Swapchain swapchain(&dev, std::move(surface));

    nickel::rhi::VertexLayout vertexLayout = nickel::rhi::Vertex::Layout();
    nickel::rhi::InputAssemblyState inputAsmState;
    nickel::rhi::ViewportState viewportState;
    auto& imageExtent = swapchain.ImageInfo().extent;
    viewportState.viewports.emplace_back(nickel::rhi::Viewport{
        0, 0, (float)imageExtent.width, (float)imageExtent.height, 0, 1});
    viewportState.sissors.emplace_back(
        nickel::rhi::Sissor{0, 0, imageExtent.width, imageExtent.height});
    nickel::rhi::RasterizationState rasterState;
    rasterState.lineWidth = 1.0;
    rasterState.cullMode = nickel::rhi::CullMode::None;
    rasterState.polygonMode = nickel::rhi::PolygonMode::Fill;
    nickel::rhi::DepthStencilState depthStencilState;
    nickel::rhi::MultisampleState multisampleState;
    nickel::rhi::ColorBlendState colorBlendState;
    nickel::rhi::vulkan::DescriptorSetLayout descriptorLayout{&dev, {}};
    nickel::rhi::vulkan::PipelineLayout layout(&dev, {&descriptorLayout});
    nickel::rhi::vulkan::GraphicsPipeline pipeline(
        vertexLayout, inputAsmState, {}, viewportState, rasterState,
        depthStencilState, multisampleState, colorBlendState, layout);
}

void BootstrapSystem(gecs::world& world,
                     typename gecs::world::registry_type& reg) {
    auto cmds = reg.commands();
    nickel::Window& window = cmds.emplace_resource<nickel::Window>(
        "vulkan testbed", 1024, 720, nickel::Window::Flag::Vulkan);

    reg
        // startup systems
        .regist_startup_system<nickel::VideoSystemInit>()
        .regist_startup_system<nickel::EventPollerInit>()
        .regist_startup_system<nickel::InputSystemInit>()
        .regist_startup_system<StartupSystem>()
        // shutdown systems
        .regist_shutdown_system<nickel::EngineShutdown>()
        // update systems
        .regist_update_system<nickel::VideoSystemUpdate>()
        // other input handle event must put here(after mouse/keyboard update)
        .regist_update_system<nickel::Mouse::Update>()
        .regist_update_system<nickel::Keyboard::Update>()
        .regist_update_system<nickel::HandleInputEvents>();
}