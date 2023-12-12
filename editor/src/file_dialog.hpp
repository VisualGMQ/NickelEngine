#pragma once

#include <vector>
#include <string>
#include <filesystem>

std::vector<std::filesystem::path> OpenFileDialog(const std::string& title);
std::filesystem::path OpenDirDialog(const std::string& title);