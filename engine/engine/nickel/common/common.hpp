#pragma once
#include "nickel/fs/path.hpp"

#include <fstream>
#include <vector>

namespace nickel {

std::vector<char> ReadWholeFile(const Path& filename);

}