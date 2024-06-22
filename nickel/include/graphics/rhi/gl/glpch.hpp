#pragma once

enum class GLProfile {
    ES3,
};

#ifdef __EMSCRIPTEN__

#include <emscripten.h>

#include "SDL.h"
// #define GL_GLEXT_PROTOTYPES 1
// #include "SDL_opengles2.h"
#include <GLES3/gl3.h>

constexpr GLProfile OPENGL_PROFILE = GLProfile::ES3;

#endif