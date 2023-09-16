#pragma once

#ifdef _WIN32
#include <windows.h>
#include <shlobj.h>
#endif

#include <vector>
#include <string>

std::vector<std::string> OpenFileDialog(const std::string& title);
std::string OpenDirDialog(const std::string& title);