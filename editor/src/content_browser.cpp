#include "content_browser.hpp"

void ContentBrowserInfo::RescanDir() {
    auto entry = std::filesystem::directory_entry{path};
    if (!entry.exists() || !entry.is_directory()) {
        return;
    }

    files.clear();
    for (auto content : std::filesystem::directory_iterator{path}) {
        files.emplace_back(content);
    }
}

void EditorContentBrowser(bool& show, ContentBrowserInfo& cbInfo) {
    if (ImGui::Begin("content browser", &show)) {
        auto relativePath =
            std::filesystem::relative(cbInfo.path, cbInfo.rootPath);
        ImGui::Text("Res://%s", relativePath.string().c_str());

        if (cbInfo.path != cbInfo.rootPath) {
            if (ImGui::Button("..")) {
                cbInfo.path = cbInfo.path.parent_path();
                cbInfo.RescanDir();
            }
        }

        ImGui::BeginGroup();
        {
            float groupX = ImGui::GetWindowPos().x + ImGui::GetWindowContentRegionMax().x;
            bool enteredDir = false;
            // please ensure all files are exists
            for (int i = 0; i < cbInfo.Files().size(); i++) {
                auto& entry = cbInfo.Files()[i];
                std::string name = entry.path().filename().string();

                // TODO: change these to image
                if (entry.is_directory()) {
                    name = "[D]" + name;
                } else if (entry.is_regular_file()) {
                    name = "[F]" + name;
                } else if (entry.is_fifo()) {
                    name = "[FIFO]" + name;
                } else if (entry.is_socket()) {
                    name = "[SOCK]" + name;
                } else if (entry.is_symlink()) {
                    name = "[SIMLINK]" + name;
                } else {
                    name = "[OTHER]" + name;
                }

                ImGui::PushID(i);
                if (ImGui::Button(name.c_str())) {
                    if (entry.is_directory()) {
                        std::cout << "clicked" << std::endl;
                        cbInfo.path /= entry.path().filename();
                        enteredDir = true;
                    }
                }
                ImGui::PopID();

                float buttonX = ImGui::GetItemRectMax().x;
                float nextButtonX = buttonX + ImGui::GetStyle().ItemSpacing.x + ImGui::GetItemRectSize().x;
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