#pragma once

#include <vector>
#include <string>
#include <filesystem>

std::filesystem::path OpenDirDialog(const std::string& title);
std::vector<std::filesystem::path> OpenFileDialog(
    const std::string& title, const std::vector<std::string>& extensions);
std::filesystem::path SaveFileDialog(
    const std::string& title, const std::vector<std::string>& extensions);