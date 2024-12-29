#define SDL_MAIN_USE_CALLBACKS
#include "SDL3/SDL_main.h"
#include "nickel/common/log.hpp"
#include "nickel/graphics/adapter.hpp"
#include "nickel/video/window.hpp"

namespace nickel {

class Runtime : public Singlton<Runtime, true> {
public:
    Runtime();
    ~Runtime();
    void Run();
    void HandleEvent(const SDL_Event &);
    bool ShouldExit() const;

private:
    bool m_should_exit = false;
    std::unique_ptr<video::Window> m_window;
    std::unique_ptr<graphics::Adapter> m_graphics_context;
};

Runtime::Runtime() {
    LOGI("init SDL");
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_SENSOR |
                  SDL_INIT_GAMEPAD | SDL_INIT_JOYSTICK)) {
        LOGC("SDL init failed: {}", SDL_GetError());
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