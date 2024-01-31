#pragma once

#include <string_view>

namespace nickel {

void ProfileBegin();
void ProfileEnd();

void InitProfile(std::string_view filename);

}  // namespace nickel

#ifdef NICKEL_ENABLE_PROFILE
#define PROFILE_BEGIN() nickel::ProfileBegin()
#define PROFILE_END() nickel::ProfileEnd()
#else
#define PROFILE_BEGIN()
#define PROFILE_END()
#endif