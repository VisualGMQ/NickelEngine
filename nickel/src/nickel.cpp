#include "pch.hpp"
#include "core/log.hpp"
#include "config/config.hpp"
#include "core/errort.hpp"
#include "window/window.hpp"
#include "window/event.hpp"

using namespace nickel;

void ErrorCallback(int error, const char* description) {
    LOGE(GlfwErr, description);
}

int main() {
    glfwSetErrorCallback(ErrorCallback);

    if (!glfwInit()) {
        LOGE(GlfwErr, "init glfw failed");
        return -1;
    }

    std::unique_ptr<Window> window = std::make_unique<Window>(WindowBuilder().FromConfig("nickel-config.toml").Build());
    EventPoller event_poller;

    while (!window->ShouldClose()) {
        window->SwapBuffer();
        event_poller.PollAndDispatch();
    }

    window.reset();
    glfwTerminate();
    return 0;
}