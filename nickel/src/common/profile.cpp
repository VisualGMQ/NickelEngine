#include "common/profile.hpp"

#ifdef NICKEL_ENABLE_PROFILE
#include "easy/profiler.h"
#endif

namespace nickel {

void ProfileBegin() {
#ifdef NICKEL_ENABLE_PROFILE
#ifdef __PRETTY_FUNCTION__
    EASY_BLOCK(__PRETTY_FUNCTION__);
#else
    EASY_BLOCK(__func__);
#endif
#endif
}

void ProfileEnd() {
#ifdef NICKEL_ENABLE_PROFILE
#ifdef __PRETTY_FUNCTION__
    EASY_END_BLOCK;
#else
    EASY_END_BLOCK;
#endif
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