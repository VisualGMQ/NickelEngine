#pragma once
#include <fstream>
#include <vector>

namespace nickel {

std::vector<char> ReadWholeFile(const char* filename);

}