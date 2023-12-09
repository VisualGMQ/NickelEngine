#pragma once

#include "nickel.hpp"
#include "imgui_plugin.hpp"
#include "context.hpp"

struct ContentBrowserInfo {
    std::filesystem::path rootPath;
    std::filesystem::path path;

    void RescanDir();

    auto& Files() const { return files; }

private:
    std::vector<std::filesystem::directory_entry> files;
};

void EditorContentBrowser(bool& show, ContentBrowserInfo&);