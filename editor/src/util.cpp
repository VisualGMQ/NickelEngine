#include "util.hpp"

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

bool ChDir(const std::filesystem::path& path) {
#ifdef _WIN32
    return SetCurrentDirectory(path.string().c_str());
#else
    return chdir(path.string().c_str()) == 0;
#endif
}