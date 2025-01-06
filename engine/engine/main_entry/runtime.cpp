#define SDL_MAIN_USE_CALLBACKS
#include "SDL3/SDL_main.h"
#include "nickel/common/log.hpp"
#include "nickel/fs/dialog.hpp"
#include "nickel/fs/storage.hpp"
#include "nickel/graphics/adapter.hpp"
#include "nickel/nickel.hpp"

namespace nickel {

class Runtime : public Singlton<Runtime, true> {
public:
    Runtime();
    ~Runtime();
    void Run();
    void HandleEvent(const SDL_Event &);
};

Runtime::Runtime() {
    LOGI("init SDL");
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_SENSOR |
                  SDL_INIT_GAMEPAD | SDL_INIT_JOYSTICK)) {
        LOGC("SDL init failed: {}", SDL_GetError());
    }

    Context::Init();

    LOGI("running engine...");
}

Runtime::~Runtime() {
    Context::Delete();
}

void Runtime::Run() {}

void Runtime::HandleEvent(const SDL_Event &event) {
    if (event.type == SDL_EVENT_QUIT) {
        Context::GetInst().Exit();
    }
}

}  // namespace nickel

SDL_AppResult SDL_AppInit(void **appstate, int argc, char **argv) {
    nickel::Runtime::Init();
    if (nickel::Context::GetInst().ShouldExit()) {
        return SDL_APP_FAILURE;
    }
    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void *appstate) {
    auto &runtime = nickel::Runtime::GetInst();
    runtime.Run();
    if (nickel::Context::GetInst().ShouldExit()) {
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