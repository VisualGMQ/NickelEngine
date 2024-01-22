#pragma once

#include "pch.hpp"

#ifdef NICKEL_ENABLE_PROFILE
#define PROFILE_BEGIN() EASY_BLOCK(__PRETTY_FUNCTION__);
#define PROFILE_END() EASY_END_BLOCK;
#else
#define PROFILE_BEGIN()
#define PROFILE_END()
#endif

namespace nickel {

inline void InitProfile(std::string_view filename) {
#ifdef NICKEL_ENABLE_PROFILE
    EASY_PROFILER_ENABLE;
    profiler::startListen();

    if (!filename.empty()) {
        profiler::dumpBlocksToFile(filename.data());
    }
#endif
}

}