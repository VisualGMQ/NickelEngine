#include "nickel/runtime.hpp"
#include "nickel/common/log.hpp"
#include "nickel/internal/pch.hpp"

#include "SDL3/SDL_main.h"

namespace nickel {

Runtime::Runtime() {
    LOGI("init SDL");
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_SENSOR |
                  SDL_INIT_GAMEPAD | SDL_INIT_JOYSTICK)) {
        std::cout << "SDL_Init Error: " << SDL_GetError() << std::endl;
    }

    LOGI("init video system");
    m_window = std::make_unique<video::Window>("sandbox", 1024, 720);

    LOGI("init graphics system");
    m_graphics_context =
        std::make_unique<graphics::Adapter>(m_window->GetImpl());

    LOGI("running engine...");
}

Runtime::~Runtime() {
    LOGI("shutdown graphics system");
    m_graphics_context.reset();

    LOGI("shutdown window system");
    m_window.reset();
}

void Runtime::Run() {}

void Runtime::HandleEvent(const SDL_Event &event) {
    if (event.type == SDL_EVENT_QUIT) {
        m_should_exit = true;
    }
}

bool Runtime::ShouldExit() const {
    return m_should_exit;
}

}  // namespace nickel

SDL_AppResult SDL_AppInit(void **appstate, int argc, char **argv) {
    nickel::Runtime::Init();
    if (nickel::Runtime::GetInst().ShouldExit()) {
        return SDL_APP_FAILURE;
    }
    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void *appstate) {
    auto &runtime = nickel::Runtime::GetInst();
    runtime.Run();
    if (runtime.ShouldExit()) {
        return SDL_APP_SUCCESS;
    }
    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event) {
    nickel::Runtime::GetInst().HandleEvent(*event);
    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result) {
    nickel::Runtime::Delete();
}