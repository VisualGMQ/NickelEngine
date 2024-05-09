#include "graphics/system.hpp"

namespace nickel {

void recreateSwapchain(const WindowResizeEvent& event,
                       gecs::resource<rhi::Adapter> adapter,
                       gecs::resource<gecs::mut<rhi::Device>> device) {
    device->OnWindowResize(event.size.w, event.size.h);
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