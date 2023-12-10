#include "content_browser.hpp"
#include "import_asset.hpp"

ContentBrowserInfo::ContentBrowserInfo() {
    textureMgr_.SetRootPath("./editor/resources");
    dirIconHandle_ = textureMgr_.LoadSVG(
        "icons/folder-windows.svg", nickel::gogl::Sampler::CreateLinearRepeat(),
        iconSize_);
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

void EditorContentBrowser(bool& show) {
    auto& cbInfo =
        gWorld->cur_registry()->res<gecs::mut<ContentBrowserInfo>>().get();
    if (ImGui::Begin("content browser", &show)) {
        auto relativePath =
            std::filesystem::relative(cbInfo.path, cbInfo.rootPath);
        ImGui::Text("Res://%s", relativePath.string().c_str());
        ImGui::SameLine();

        if (ImGui::Button("+")) {
            SelectAndLoadAsset(*gWorld->cur_registry());
        }

        if (cbInfo.path != cbInfo.rootPath) {
            if (ImGui::Button("..")) {
                cbInfo.path = cbInfo.path.parent_path();
                cbInfo.RescanDir();
            }
        }

        ImGui::BeginGroup();
        {
            float groupX =
                ImGui::GetWindowPos().x + ImGui::GetWindowContentRegionMax().x;
            bool enteredDir = false;
            auto& textureMgr =
                gWorld->cur_registry()->res<nickel::TextureManager>().get();
            // please ensure all files are exists
            for (int i = 0; i < cbInfo.Files().size(); i++) {
                auto& entry = cbInfo.Files()[i];

                ImGui::PushID(i);

                auto extension = entry.path().extension().string();
                auto filetype = DetectFileType(entry.path());
                const nickel::Texture* texture = nullptr;
                if (entry.is_directory()) {
                    texture = &cbInfo.GetDirIcon();
                } else {
                    if (filetype == FileType::Image) {
                        auto handle = textureMgr.GetHandle(entry);
                        if (handle) {
                            texture = &textureMgr.Get(handle);
                        }
                    }
                }
                if (!texture) {
                    texture = &cbInfo.FindTextureOrGen(extension);
                }
                ImGui::BeginGroup();
                {
                    if (*texture) {
                        if (ImGui::ImageButton(
                                (ImTextureID)texture->Raw(),
                                ImVec2{cbInfo.thumbnailSize.w,
                                       cbInfo.thumbnailSize.h})) {
                            if (entry.is_directory()) {
                                std::cout << "clicked" << std::endl;
                                cbInfo.path /= entry.path().filename();
                                enteredDir = true;
                            }
                        }
                    }
                    ImGui::Text("%s", entry.path().filename().string().c_str());
                }
                ImGui::EndGroup();
                ImGui::PopID();

                float buttonX = ImGui::GetItemRectMax().x;
                float nextButtonX = buttonX + ImGui::GetStyle().ItemSpacing.x +
                                    ImGui::GetItemRectSize().x;
                if (i + 1 < cbInfo.Files().size() && nextButtonX < groupX) {
                    ImGui::SameLine();
                }
            }

            if (enteredDir) {
                cbInfo.RescanDir();
            }
        }
        ImGui::EndGroup();

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
        // TODO: return a default SVG
        return nickel::Texture::Null;
    }
}