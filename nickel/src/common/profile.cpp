#include "common/profile.hpp"

#ifdef NICKEL_ENABLE_PROFILE
#include "easy/profiler.h"
#endif

#include <iostream>

namespace nickel {

void ProfileBegin(const char* name) {
#ifdef NICKEL_ENABLE_PROFILE
    EASY_BLOCK(name);
#endif
}

void ProfileEnd() {
#ifdef NICKEL_ENABLE_PROFILE
    EASY_END_BLOCK;
#endif
}

void InitProfile(std::string_view filename) {
#ifdef NICKEL_ENABLE_PROFILE
    EASY_PROFILER_ENABLE;
    profiler::startListen();

    if (!filename.empty()) {
        profiler::dumpBlocksToFile(filename.data());
    }
#endif
}

}  // namespace nickel