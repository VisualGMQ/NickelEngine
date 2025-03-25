#pragma once
#include "nickel/fs/path.hpp"

#include <vector>

namespace nickel {

std::vector<char> ReadWholeFile(const Path& filename);

}