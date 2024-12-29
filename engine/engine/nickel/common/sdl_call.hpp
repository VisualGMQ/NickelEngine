#pragma once

#include "nickel/internal/pch.hpp"
#include "nickel/common/log.hpp"

namespace nickel {

#define SDL_CALL(x)                                      \
    do {                                                 \
        if (!(x)) LOGE("SDL error: {}", SDL_GetError()); \
    } while (0)

}