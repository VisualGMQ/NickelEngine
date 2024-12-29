#pragma once
#include "common/singleton.hpp"
#include "nickel/graphics/adapter.hpp"
#include "nickel/video/window.hpp"

union SDL_Event;

namespace nickel {

class Runtime: public Singlton<Runtime, true> {
public:
    Runtime();
    ~Runtime();
    void Run();
    void HandleEvent(const SDL_Event&);
    bool ShouldExit() const;

private:
    bool m_should_exit = false;
    std::unique_ptr<video::Window> m_window;
    std::unique_ptr<graphics::Adapter> m_graphics_context;
};

}
