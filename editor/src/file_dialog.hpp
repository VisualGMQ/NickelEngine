#pragma once

#ifdef _WIN32
#include <windows.h>
#include <shlobj.h>
#endif

#include <vector>
#include <string>
#include <filesystem>

std::vector<std::filesystem::path> OpenFileDialog(const std::string& title);
std::filesystem::path OpenDirDialog(const std::string& title);