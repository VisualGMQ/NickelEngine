#pragma once

#include "core/gogl.hpp"
#include "pch.hpp"
#include "nickel.hpp"
#include "imgui_plugin.hpp"

struct EditorContext {
    bool openContentBrowser = true;
    bool openInspector = true;
    bool openEntityList = true;
    bool openGameWindow = true;
    bool openDemoWindow = false;

    // icon fonts
    // ImFont* imguiIconFont;
    
    // opened project information
    nickel::ProjectInitInfo projectInfo;

    std::unique_ptr<nickel::gogl::RenderBuffer> renderBuffer;
    std::unique_ptr<nickel::gogl::Framebuffer> gameContentTarget;
    std::unique_ptr<nickel::gogl::Texture> gameContentTexture;

    EditorContext();
    ~EditorContext();

    void Init();
};
