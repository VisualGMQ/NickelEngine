#pragma once

#include "core/gogl.hpp"
#include "pch.hpp"
#include "nickel.hpp"

struct EditorContext {
    bool openContentBrowser = true;
    bool openInspector = true;
    bool openEntityList = true;
    bool openGameWindow = true;
    bool openDemoWindow = false;
    
    // opened project information
    nickel::ProjectInitInfo projectInfo;

    std::unique_ptr<nickel::gogl::RenderBuffer> renderBuffer_;
    std::unique_ptr<nickel::gogl::Framebuffer> gameContentTarget_;
    std::unique_ptr<nickel::gogl::Texture> gameContentTexture_;

    EditorContext();
    ~EditorContext();
};
