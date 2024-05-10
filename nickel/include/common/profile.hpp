#pragma once

#include <string_view>

namespace nickel {

void ProfileBegin(const char*);
void ProfileEnd();

void InitProfile(std::string_view filename);

}  // namespace nickel

#ifdef NICKEL_ENABLE_PROFILE
#ifdef __PRETTY_FUNCTION__
#define PROFILE_BEGIN() nickel::ProfileBegin(__PRETTY_FUNCTION__)
#else
#define PROFILE_BEGIN() nickel::ProfileBegin(__func__)
#endif
#define PROFILE_END() nickel::ProfileEnd()
#else
#define PROFILE_BEGIN()
#define PROFILE_END()
#endif