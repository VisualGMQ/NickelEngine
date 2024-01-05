#pragma once

#define TOML_EXCEPTIONS 0
#define TOML_HEADER_ONLY 0
#include "toml++/toml.hpp"

#include "core/assert.hpp"

#include "glad/glad.h"
#include "stb_image.h"


#include "miniaudio.h"

#define RAPIDXML_NO_EXCEPTIONS
#include "rapidxml.hpp"
#include "rapidxml_print.hpp"
#include "rapidxml_utils.hpp"

#include "SDL.h"

#define MIRROW_ASSERT(expr, msg) Assert(expr, msg)
#include "mirrow/drefl/drefl.hpp"
#include "mirrow/serd/dynamic/backends/tomlplusplus.hpp"
#include "mirrow/serd/static/backends/tomlplusplus.hpp"
#include "mirrow/srefl/reflect.hpp"
#include "mirrow/util/const_str.hpp"
#include "mirrow/util/function_traits.hpp"
#include "mirrow/util/misc.hpp"
#include "mirrow/util/type_list.hpp"
#include "mirrow/util/variable_traits.hpp"

#define GECS_ASSERT(expr, msg) Assert(expr, msg)
#define GECS_TYPE_INFO_TYPE const ::mirrow::drefl::type*
#define GECS_GET_TYPE_INFO(type) ::mirrow::drefl::typeinfo<type>()
#define GECS_ANY ::mirrow::drefl::any
#define GECS_MAKE_ANY_REF(x) ::mirrow::drefl::any_make_ref(x)
#include "gecs/gecs.hpp"

#include "nameof.hpp"

#include "ft2build.h"
#include FT_FREETYPE_H

#include "lunasvg.h"

#include <algorithm>
#include <array>
#include <chrono>
#include <cmath>
#include <filesystem>
#include <iostream>
#include <memory>
#include <random>
#include <sstream>
#include <string>
#include <string_view>
#include <thread>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>
#include <cstring>
#include <type_traits>
#include <variant>
#include <charconv>


#ifdef _WIN32
#ifndef DLLEXPORT
#define DLLEXPORT __declspec(dllexport)
#else
#define DLLEXPORT
#endif
// some windows evil macro
// evil macro to remove Window SDK min&max macro
// #ifndef NOMINMAX
// #define NOMINMAX 
// #endif
// // some windows evil macro undef
// #undef near
// #undef far
#endif

extern std::unique_ptr<gecs::world> gWorld;

// #define _CRTDBG_MAP_ALLOC
// #include <stdlib.h>
// #include <crtdbg.h>