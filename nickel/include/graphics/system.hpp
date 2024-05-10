#pragma once

#include "common/ecs.hpp"
#include "graphics/context.hpp"

namespace nickel {

void RenderSystemInit(gecs::event_dispatcher<WindowResizeEvent>);

}