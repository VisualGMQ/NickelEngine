#include "content_browser.hpp"
#include "asset_property_window.hpp"

ContentBrowserInfo::ContentBrowserInfo() {
    textureMgr_.SetRootPath("./editor/resources");
    initExtensionIconMap();
    dirIconHandle_ = textureMgr_.LoadSVG(
        "icons/folder-windows.svg", nickel::gogl::Sampler::CreateLinearRepeat(),
        iconSize_);
    unknownFileIconHandle_ = textureMgr_.LoadSVG(
        "icons/assembly.svg", nickel::gogl::Sampler::CreateLinearRepeat(),
        iconSize_);
}

void ContentBrowserInfo::initExtensionIconMap() {
    auto result = toml::parse_file(iconConfigFilename_.string());
    if (!result) {
        LOGW(nickel::log_tag::Editor, "parse icon config file ",
             iconConfigFilename_, " failed!", result.error());
        return;
    }

    const auto& tbl = result.table();
    for (auto&& [iconFilename, extensions] : tbl) {
        Assert(extensions.is_array(),
               "require array value in " + iconConfigFilename_.string());
        for (auto&& extension : *extensions.as_array()) {
            Assert(extension.is_string(), "extension must be string");
            auto& extStr = *extension.as_string();

            registFileIcon(extension.as_string()->get(), iconFilename.data());
        }
    }
}

void ContentBrowserInfo::RescanDir() {
    auto entry = std::filesystem::directory_entry{path};
    if (!entry.exists() || !entry.is_directory()) {
        return;
    }

    files_.clear();
    for (auto content : std::filesystem::directory_iterator{path}) {
        files_.emplace_back(content);
    }
}

void SelectAndLoadAsset(gecs::registry reg) {
    auto filenames = OpenFileDialog("load assets");

    bool contentChanged = false;
    auto& cbInfo = reg.res<gecs::mut<ContentBrowserInfo>>().get();

    for (auto& filename : filenames) {
        auto type = nickel::DetectFileType(filename);
        auto copyPath = cbInfo.path / filename.filename();
        if (filename != copyPath) {
            if (!std::filesystem::copy_file(filename, copyPath)) {
                LOGW(nickel::log_tag::Editor, "copy ", filename, " to ",
                     copyPath, " failed");
                continue;
            }
        }

        auto& textureMgr = reg.res_mut<nickel::TextureManager>().get();
        auto& fontMgr = reg.res_mut<nickel::FontManager>().get();
        contentChanged = nickel::ImportAsset(copyPath, textureMgr, fontMgr);
    }

    // TODO: use file watcher to do this
    if (contentChanged) {
        cbInfo.RescanDir();
    }
}

std::pair<const nickel::Texture&, bool> getIcon(
    const std::filesystem::directory_entry& entry, nickel::FileType filetype,
    ContentBrowserInfo& cbInfo, const nickel::TextureManager& textureMgr,
    const nickel::FontManager& fontMgr) {
    if (entry.is_directory()) {
        return {cbInfo.GetDirIcon(), false};
    } else if (filetype == nickel::FileType::Image) {
        auto handle = textureMgr.GetHandle(entry);
        if (handle) {
            return {textureMgr.Get(handle), true};
        }
    } else if (filetype == nickel::FileType::Font) {
        if (auto handle = fontMgr.GetHandle(entry); handle) {
            // TODO: return font preview texture to texture
            return {cbInfo.FindTextureOrGen(entry.path().extension().string()),
                    true};
        }
    }
    // TODO: other type assets

    return {cbInfo.FindTextureOrGen(entry.path().extension().string()), false};
}

void showAssetOperationMenu(nickel::FileType filetype, bool hasImported,
                            const std::filesystem::directory_entry& entry,
                            nickel::TextureManager& textureMgr,
                            nickel::FontManager& fontMgr) {
    if (filetype != nickel::FileType::Unknown) {
        if (ImGui::BeginPopupContextItem()) {
            if (!hasImported) {
                if (ImGui::Button("import")) {
                    nickel::ImportAsset(entry, textureMgr, fontMgr);
                    ImGui::CloseCurrentPopup();
                }
            } else {
                if (ImGui::Button("destroy")) {
                    nickel::RemoveAsset(entry, textureMgr, fontMgr);
                    ImGui::CloseCurrentPopup();
                }
            }
            ImGui::EndPopup();
        }
    }
}

enum class OperateOnIcon {
    GotoParent,
    ClickOnTexture,
};

bool showOneIcon(ContentBrowserInfo& cbInfo, nickel::TextureManager& textureMgr,
                 nickel::FontManager& fontMgr,
                 const std::filesystem::directory_entry& entry) {
    auto extension = entry.path().extension().string();
    auto filetype = nickel::DetectFileType(entry.path());

    auto&& [texture, hasImported] =
        getIcon(entry, filetype, cbInfo, textureMgr, fontMgr);

    bool clicked = false;

    ImGui::BeginGroup();
    {
        auto assetPropertyWindowCtx =
            gWorld->res_mut<AssetPropertyWindowContext>();
        static std::string texturePropertyWindowTitle = "texture property";
        if (ImGui::ImageButton(
                (ImTextureID)texture.Raw(),
                ImVec2{cbInfo.thumbnailSize.w, cbInfo.thumbnailSize.h})) {
            clicked = true;
            if (entry.is_directory()) {
                cbInfo.path /= entry.path().filename();
            } else if (filetype == nickel::FileType::Image) {
                ImGui::OpenPopup(texturePropertyWindowTitle.c_str());
                assetPropertyWindowCtx->sampler =
                    textureMgr.Get(entry).Sampler();
            } else if (filetype == nickel::FileType::Font) {
                // TODO: show font property window
            }
        }

        showAssetOperationMenu(filetype, hasImported, entry, textureMgr,
                               fontMgr);

        ImGui::Text("%s", entry.path().filename().string().c_str());

        ImGui::EndGroup();
    }

    return clicked;
}

void showIcons(ContentBrowserInfo& cbInfo,
               AssetPropertyWindowContext& assetPropCtx) {
    ImGui::BeginGroup();
    {
        float groupX =
            ImGui::GetWindowPos().x + ImGui::GetWindowContentRegionMax().x;

        auto& textureMgr =
            gWorld->res<gecs::mut<nickel::TextureManager>>().get();
        auto& fontMgr = gWorld->res_mut<nickel::FontManager>().get();
        auto& files = cbInfo.Files();
        static std::optional<int> clickedIdx;

        // please ensure all files are exists
        for (int i = 0; i < files.size(); i++) {
            if (showOneIcon(cbInfo, textureMgr, fontMgr, files[i])) {
                clickedIdx = i;
            }

            float buttonX = ImGui::GetItemRectMax().x;
            float nextButtonX = buttonX + ImGui::GetStyle().ItemSpacing.x +
                                ImGui::GetItemRectSize().x;
            if (i + 1 < cbInfo.Files().size() && nextButtonX < groupX) {
                ImGui::SameLine();
            }
        }

        if (clickedIdx && files[clickedIdx.value()].is_directory()) {
            cbInfo.RescanDir();
            clickedIdx = std::nullopt;
        }

        bool clickedImage = TexturePropertyPopupWindow(
            cbInfo.texturePropertyPopupWindowTitle,
            clickedIdx ? textureMgr.GetHandle(files[clickedIdx.value()])
                       : nickel::TextureHandle::Null(),
            assetPropCtx);

        ImGui::EndGroup();
    }
}

void EditorContentBrowser(bool& show) {
    auto cbInfo = gWorld->res_mut<ContentBrowserInfo>();
    auto assetPropCtx = gWorld->res_mut<AssetPropertyWindowContext>();

    if (ImGui::Begin("content browser", &show)) {
        auto relativePath =
            std::filesystem::relative(cbInfo->path, cbInfo->rootPath);
        ImGui::Text("Res://%s", relativePath.string().c_str());
        ImGui::SameLine();

        // import button
        if (ImGui::Button("+")) {
            SelectAndLoadAsset(*gWorld->cur_registry());
        }

        // goto parent dir button
        if (cbInfo->path != cbInfo->rootPath) {
            if (ImGui::Button("..")) {
                cbInfo->path = cbInfo->path.parent_path();
                cbInfo->RescanDir();
            }
        }

        showIcons(cbInfo.get(), assetPropCtx.get());

        ImGui::End();
    }
}

nickel::Texture& ContentBrowserInfo::FindTextureOrGen(
    const std::string& extension) {
    if (auto it = extensionHandleMap_.find(extension);
        it != extensionHandleMap_.end()) {
        auto handle = it->second;
        if (handle) {
            return textureMgr_.Get(handle);
        } else {
            return nickel::Texture::Null;
        }
    } else {
        if (auto it2 = extensionIconMap_.find(extension);
            it2 != extensionIconMap_.end()) {
            auto handle = textureMgr_.LoadSVG(
                it2->second.string(),
                nickel::gogl::Sampler::CreateLinearRepeat(), iconSize_);
            extensionHandleMap_.insert_or_assign(extension, handle);
            return textureMgr_.Get(handle);
        }
        return textureMgr_.Get(unknownFileIconHandle_);
    }
}