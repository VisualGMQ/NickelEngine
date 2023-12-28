#include "content_browser.hpp"
#include "asset_property_window.hpp"
#include "context.hpp"

ContentBrowserWindow::ContentBrowserWindow() {
    auto iconSize = nickel::cgmath::Vec2{IconSize, IconSize};
    initExtensionIconMap();
    dirIconHandle_ = textureMgr_.LoadSVG(
        "editor/resources/icons/folder-windows.svg", nickel::gogl::Sampler::CreateLinearRepeat(),
        iconSize);
    unknownFileIconHandle_ = textureMgr_.LoadSVG(
        "editor/resources/icons/assembly.svg", nickel::gogl::Sampler::CreateLinearRepeat(),
        iconSize);
}

void ContentBrowserWindow::initExtensionIconMap() {
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

void ContentBrowserWindow::RescanDir() {
    auto entry = std::filesystem::directory_entry{path_};
    if (!entry.exists() || !entry.is_directory()) {
        return;
    }

    files_.clear();
    for (auto content : std::filesystem::directory_iterator{path_}) {
        auto& path = content.path();
        if (!content.is_directory() && path.extension() != ".meta") {
            files_.emplace_back(content);
        }
    }
}

void ContentBrowserWindow::selectAndLoadAsset() {
    auto& reg = *gWorld->cur_registry();
    auto filenames = OpenFileDialog("load assets", {".*"});

    for (auto& filename : filenames) {
        auto type = nickel::DetectFileType(filename);
        auto copyPath = path_ / filename.filename();
        if (filename != copyPath) {
            if (std::filesystem::exists(copyPath)) {
                std::filesystem::remove(copyPath);
            }
            if (!std::filesystem::copy_file(filename, copyPath)) {
                LOGW(nickel::log_tag::Editor, "copy ", filename, " to ",
                     copyPath, " failed");
                continue;
            }
        }
    }
}

std::pair<const nickel::Texture&, bool> ContentBrowserWindow::getIcon(
    const std::filesystem::directory_entry& entry, nickel::FileType filetype,
    nickel::AssetManager& assetMgr) {
    if (entry.is_directory()) {
        return {textureMgr_.Get(dirIconHandle_), false};
    } else if (filetype == nickel::FileType::Image) {
        auto handle = assetMgr.TextureMgr().GetHandle(entry);
        if (handle) {
            return {assetMgr.Get(handle), true};
        }
    } else if (filetype == nickel::FileType::Font) {
        if (auto handle = assetMgr.FontMgr().GetHandle(entry); handle) {
            // TODO: return font preview texture
            return {FindTextureOrGen(entry.path().extension().string()), true};
        }
    } else if (filetype == nickel::FileType::Audio) {
        if (auto handle = assetMgr.AudioMgr().GetHandle(entry); handle) {
            // TODO: return audio preview texture
            return {FindTextureOrGen(entry.path().extension().string()), true};
        }
    }
    // TODO: other type assets

    return {FindTextureOrGen(entry.path().extension().string()), false};
}

void ContentBrowserWindow::showAssetOperationPopupMenu(
    nickel::FileType filetype, bool hasImported,
    const std::filesystem::directory_entry& entry,
    nickel::AssetManager& assetMgr) {
    auto ctx = gWorld->res<EditorContext>();
    if (filetype != nickel::FileType::Unknown) {
        if (ImGui::BeginPopupContextItem(entry.path().string().c_str())) {
            if (!hasImported) {
                if (ImGui::Button("import")) {
                    assetMgr.Load(entry);
                    ImGui::CloseCurrentPopup();
                }
            } else {
                if (ImGui::Button("release")) {
                    assetMgr.Destroy(entry);
                    ImGui::CloseCurrentPopup();
                }
            }
            if (ImGui::Button("delete")) {
                std::filesystem::remove(entry);
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }
    }
}

void ContentBrowserWindow::showOneIcon(
    nickel::AssetManager& assetMgr,
    const std::filesystem::directory_entry& entry) {
    auto extension = entry.path().extension().string();
    auto filetype = nickel::DetectFileType(entry.path());

    auto&& [texture, hasImported] = getIcon(entry, filetype, assetMgr);

    auto ctx = gWorld->res_mut<EditorContext>();
    ImGui::BeginGroup();
    {
        if (ImGui::ImageButton((ImTextureID)texture.Raw(),
                               ImVec2{thumbnailSize_.w, thumbnailSize_.h})) {
            if (entry.is_directory()) {
                path_ /= entry.path().filename();
                RescanDir();
            } else {
                switch (filetype) {
                    case nickel::FileType::Image:
                        ctx->texturePropWindow.Show();
                        ctx->texturePropWindow.ChangeTexture(
                            assetMgr.TextureMgr().GetHandle(entry));
                        break;
                    case nickel::FileType::Audio:
                        ctx->soundPropWindow.Show();
                        ctx->soundPropWindow.ChangeAudio(
                            assetMgr.AudioMgr().GetHandle(entry));
                        break;
                    case nickel::FileType::Font:
                    case nickel::FileType::Tilesheet:
                    case nickel::FileType::Animation:
                    case nickel::FileType::Timer:
                    case nickel::FileType::FileTypeCount:
                    case nickel::FileType::Unknown:
                        break;
                }
            }
        }

        showAssetOperationPopupMenu(filetype, hasImported, entry, assetMgr);

        ImGui::Text("%s", entry.path().filename().string().c_str());

        ImGui::EndGroup();
    }
}

void ContentBrowserWindow::showIcons() {
    ImGui::BeginGroup();
    {
        float groupX =
            ImGui::GetWindowPos().x + ImGui::GetWindowContentRegionMax().x;

        auto& assetMgr = gWorld->res_mut<nickel::AssetManager>().get();
        auto& fontMgr = gWorld->res_mut<nickel::FontManager>().get();
        static std::optional<int> clickedIdx;

        // please ensure all files are exists
        for (int i = 0; i < files_.size(); i++) {
            showOneIcon(assetMgr, files_[i]);

            float buttonX = ImGui::GetItemRectMax().x;
            float nextButtonX = buttonX + ImGui::GetStyle().ItemSpacing.x +
                                ImGui::GetItemRectSize().x;
            if (i + 1 < files_.size() && nextButtonX < groupX) {
                ImGui::SameLine();
            }
        }

        ImGui::EndGroup();
    }
}

void ContentBrowserWindow::Update() {
    if (!IsVisible()) return;

    if (ImGui::Begin("content browser", &show_)) {
        auto relativePath = std::filesystem::relative(path_, rootPath_);
        ImGui::Text("Res://%s", relativePath.string().c_str());
        ImGui::SameLine();

        // import button
        if (ImGui::Button("+")) {
            selectAndLoadAsset();
        }

        // goto parent dir button
        if (path_ != rootPath_) {
            if (ImGui::Button("..")) {
                path_ = path_.parent_path();
                RescanDir();
            }
        }

        showIcons();
    }
    ImGui::End();

    // auto ctx = gWorld->res_mut<EditorContext>();
    // ctx->texturePropWindow.PrepareOpen();
    // ctx->soundPropWindow.PrepareOpen();
}

nickel::Texture& ContentBrowserWindow::FindTextureOrGen(
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
            auto handle =
                textureMgr_.LoadSVG(it2->second.string(),
                                    nickel::gogl::Sampler::CreateLinearRepeat(),
                                    nickel::cgmath::Vec2{IconSize, IconSize});
            extensionHandleMap_.insert_or_assign(extension, handle);
            return textureMgr_.Get(handle);
        }
        return textureMgr_.Get(unknownFileIconHandle_);
    }
}

void releaseAsset(const ReleaseAssetEvent& event,
                  gecs::resource<gecs::mut<nickel::AssetManager>> assetMgr) {
    if (event.handle) {
        std::visit(
            [&](auto&& h) {
                const_cast<gecs::resource<gecs::mut<nickel::AssetManager>>&>(
                    assetMgr)
                    ->Destroy(h);
            },
            event.handle.value());
    } else if (event.path) {
        const_cast<gecs::resource<gecs::mut<nickel::AssetManager>>&>(assetMgr)
            ->Destroy(event.path.value());
    }
}

void RegistEventHandler(
    gecs::event_dispatcher<ReleaseAssetEvent> releaseAssetEvent) {
    releaseAssetEvent.sink().add<releaseAsset>();
}