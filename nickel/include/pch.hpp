#pragma once

#ifndef TOML_EXCEPTIONS
#define TOML_EXCEPTIONS 0
#endif
#include "toml++/toml.hpp"

#include "core/assert.hpp"

#include "stb_image.h"
#include "glad/glad.h"

#include "miniaudio.h"

#ifdef _WIN32
#define NOMINMAX    // evil macro to remove Window SDK min&max macro
#endif

#include "SDL.h"

#include "nameof.hpp"

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
#define GECS_TYPE_INFO_TYPE ::mirrow::drefl::type_info
#define GECS_GET_TYPE_INFO(type) ::mirrow::drefl::reflected_type<type>().type_node()
#define GECS_REFERENCE_ANY ::mirrow::drefl::reference_any
#include "gecs/gecs.hpp"

#include <iostream>
#include <memory>
#include <string>
#include <utility>
#include <string_view>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <array>
#include <filesystem>
#include <chrono>
#include <thread>
#include <cmath>
#include <algorithm>
#include <sstream>
#include <random>

#ifdef _WIN32
#define DLLEXPORT __declspec(dllexport)
#else
#define DLLEXPORT
#endif