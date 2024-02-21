#pragma once

#ifdef __EMSCRIPTEN__

#include <emscripten.h>

#include "SDL.h"
#define GL_GLEXT_PROTOTYPES 1
#include "SDL_opengles2.h"

#else

#include "SDL.h"
#include "glad/glad.h"

#endif