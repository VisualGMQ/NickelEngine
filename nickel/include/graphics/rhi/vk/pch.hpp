#pragma once

#include "common/log.hpp"
#include "common/log_tag.hpp"

#define NICKEL_VULKAN_ASSERT(expr)                                         \
    do {                                                                   \
        if (!(expr))                                                       \
            LOGE(::nickel::log_tag::Vulkan, "execute ", #expr, " failed"); \
    } while (0)

#define VULKAN_HPP_NO_EXCEPTIONS 
#define VULKAN_HPP_ASSERT NICKEL_VULKAN_ASSERT
#include "vulkan/vulkan.hpp"
#include "SDL_vulkan.h"

#include "stdpch.hpp"
