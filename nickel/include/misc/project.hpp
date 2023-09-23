#pragma once

#include "pch.hpp"
#include "nickel.hpp"

namespace nickel {

void SaveProject(std::string_view rootPath, const TextureManager textureMgr);
void LoadProject(std::string_view rootPath, TextureManager& textureMgr);

}