#include "content_browser.hpp"
#include "asset_property_window.hpp"
#include "context.hpp"

ContentBrowserWindow::ContentBrowserWindow(EditorContext* ctx) : ctx_(ctx) {
    auto iconSize = nickel::cgmath::Vec2{IconSize, IconSize};
    initExtensionIconMap();
    dirIconHandle_ = textureMgr_.LoadSVG(
        ctx_->Convert2EditorRelatePath(
            "editor/resources/icons/folder-windows.svg"));
    unknownFileIconHandle_ = textureMgr_.LoadSVG(
        ctx_->Convert2EditorRelatePath("editor/resources/icons/assembly.svg"));
}

void ContentBrowserWindow::initExtensionIconMap() {
    auto result = toml::parse_file(
        ctx_->Convert2EditorRelatePath(iconConfigFilename_).string());
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

            registFileIcon(extension.as_string()->get(),
                           ctx_->Convert2EditorRelatePath(iconFilename.data()));
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
        if (path.extension() != ".meta") {
            files_.emplace_back(content);
        }
    }
}

void ContentBrowserWindow::selectAndLoadAsset() {
    auto& reg = *nickel::ECS::Instance().World().cur_registry();
    auto filenames = OpenFileDialog("load assets", {".*"});

    for (auto& filename : filenames) {
        auto type = nickel::DetectFileType(filename);
        auto copyPath = path_ / filename.filename();
        std::error_code err;
        if (filename != copyPath) {
            if (std::filesystem::exists(copyPath, err)) {
                FS_CALL(std::filesystem::remove(copyPath, err), err);
            }
            if (!std::filesystem::copy_file(filename, copyPath, err)) {
                FS_LOG_ERR(err, "copy ", filename, " to ", copyPath, " failed");
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
            auto& texts = ctx_->FindOrGenFontPrewview(handle).Texts();
            if (!texts.empty()) {
                return {*texts[0].texture, true};
            } else {
                return {FindTextureOrGen(entry.path().extension().string()),
                        true};
            }
        }
    } else if (filetype == nickel::FileType::Audio) {
        if (auto handle = assetMgr.AudioMgr().GetHandle(entry); handle) {
            // TODO: return audio preview texture
            return {FindTextureOrGen(entry.path().extension().string()), true};
        }
    } else if (filetype == nickel::FileType::Tilesheet) {
        if (auto handle = assetMgr.TilesheetMgr().GetHandle(entry); handle) {
            return {FindTextureOrGen(entry.path().extension().string()), true};
        }
    }
    // TODO: other type assets

    return {FindTextureOrGen(entry.path().extension().string()), false};
}

void ContentBrowserWindow::showAssetOperationPopupMenu(
    const std::filesystem::path& path, nickel::AssetManager& assetMgr) {
    auto ctx = nickel::ECS::Instance().World().res<EditorContext>();
    if (ImGui::BeginPopupContextItem(path.string().c_str())) {
        if (ImGui::Button("delete")) {
            std::error_code err;
            FS_CALL(std::filesystem::remove_all(path, err), err);
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
}

void ContentBrowserWindow::showOneIcon(
    nickel::AssetManager& assetMgr,
    const std::filesystem::directory_entry& entry) {
    auto extension = entry.path().extension().string();
    auto filetype = nickel::DetectFileType(entry.path());

    std::filesystem::directory_entry relativeEntry{
        ctx_->GetRelativePath(entry.path())};
    auto&& [texture, hasImported] = getIcon(relativeEntry, filetype, assetMgr);

    auto ctx = nickel::ECS::Instance().World().res_mut<EditorContext>();
    ImGui::BeginGroup();
    {
        if (ImGui::ImageButton(entry.path().string().c_str(), texture,
                               ImVec2{thumbnailSize_.w, thumbnailSize_.h})) {
            if (entry.is_directory()) {
                path_ /= entry.path().filename();
                RescanDir();
            } else {
                auto path = relativeEntry.path();
                switch (filetype) {
                    case nickel::FileType::Image:
                        ctx->texturePropWindow.Show();
                        ctx->texturePropWindow.ChangeTexture(
                            assetMgr.TextureMgr().GetHandle(path));
                        break;
                    case nickel::FileType::Audio:
                        ctx->soundPropWindow.Show();
                        ctx->soundPropWindow.ChangeAudio(
                            assetMgr.AudioMgr().GetHandle(path));
                        break;
                    case nickel::FileType::Font:
                        ctx->fontPropWindow.Show();
                        ctx->fontPropWindow.ChangeFont(
                            assetMgr.FontMgr().GetHandle(path));
                        break;
                    case nickel::FileType::Tilesheet:
                        ctx->tilesheetEditor.Show();
                        ctx->tilesheetEditor.ChangeTilesheet(
                            assetMgr.TilesheetMgr().GetHandle(path));
                        break;
                    case nickel::FileType::Animation:
                        ctx->animEditor.Show();
                        ctx->animEditor.ChangePlayer(
                            gecs::null_entity,
                            assetMgr.AnimationMgr().GetHandle(path));
                        break;
                    case nickel::FileType::Timer:
                    case nickel::FileType::FileTypeCount:
                    case nickel::FileType::Unknown:
                        break;
                }
            }
        }

        showAssetOperationPopupMenu(relativeEntry.path(), assetMgr);

        ImGui::Text("%s", relativeEntry.path().filename().string().c_str());

        ImGui::EndGroup();
    }
}

void ContentBrowserWindow::showIcons() {
    ImGui::BeginGroup();
    {
        float groupX =
            ImGui::GetWindowPos().x + ImGui::GetWindowContentRegionMax().x;

        auto& assetMgr = nickel::ECS::Instance().World().res_mut<nickel::AssetManager>().get();
        auto& fontMgr = nickel::ECS::Instance().World().res_mut<nickel::FontManager>().get();
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

void ContentBrowserWindow::update() {
    std::error_code err;
    std::filesystem::path relativePath;
    FS_CALL(relativePath = std::filesystem::relative(path_, rootPath_, err),
            err);
    ImGui::Text("Res://%s", relativePath.string().c_str());
    ImGui::SameLine();

    // import button
    if (ImGui::Button("+")) {
        selectAndLoadAsset();
    }
    ImGui::SameLine();
    // create directory
    if (ImGui::Button("create directory")) {
        auto ctx = nickel::ECS::Instance().World().res_mut<EditorContext>();
        auto curPath = ctx->contentBrowserWindow.CurPath();
        ctx->inputTextWindow.SetCallback([=](const std::string& dirName) {
            auto dir = curPath / dirName;
            std::error_code err;
            if (!std::filesystem::create_directory(dir, err)) {
                FS_LOG_ERR(err, "create directory ", dir, " failed");
            }
        });
        ctx->inputTextWindow.Show();
        ImGui::CloseCurrentPopup();
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