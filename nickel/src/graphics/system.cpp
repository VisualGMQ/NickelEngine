#include "graphics/system.hpp"
#include "graphics/rhi/vk/device.hpp"

namespace nickel {

void recreateSwapchain(const WindowResizeEvent& event,
                       gecs::resource<rhi::Adapter> adapter,
                       gecs::resource<gecs::mut<rhi::Device>> device) {
#ifdef NICKEL_HAS_VULKAN
    if (adapter->RequestAdapterInfo().api == rhi::APIPreference::Vulkan) {
        static_cast<rhi::vulkan::DeviceImpl*>(device->Impl())
            ->OnWindowResize(event.size);
    }
#endif
}

void waitDeviceIdle(const WindowResizeEvent&,
                    gecs::resource<gecs::mut<rhi::Device>> device) {
    device->WaitIdle();
}

void RenderSystemInit(gecs::event_dispatcher<WindowResizeEvent> event) {
    event.immediatly_sink().add<waitDeviceIdle>();
    event.immediatly_sink().add<RenderContext::OnWindowResize>();
    event.immediatly_sink().add<recreateSwapchain>();
    event.immediatly_sink().add<waitDeviceIdle>();
}

}  // namespace nickel