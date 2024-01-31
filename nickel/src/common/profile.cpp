#include "common/profile.hpp"

#include "easy/profiler.h"

namespace nickel {

void ProfileBegin() {
#ifdef __PRETTY_FUNCTION__
    EASY_BLOCK(__PRETTY_FUNCTION__);
#else
    EASY_BLOCK(__func__);
#endif
}

void ProfileEnd() {
#ifdef __PRETTY_FUNCTION__
    EASY_END_BLOCK;
#else
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