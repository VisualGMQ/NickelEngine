#include "input_text_window.hpp"

InputTextWindow::InputTextWindow(const std::string& title)
    : PopupWindow(title) {
}

void InputTextWindow::update() {
    if (ImGui::BeginPopupModal(GetTitle().c_str(), &show_,
                               ImGuiWindowFlags_AlwaysAutoResize)) {
        static char buf[MaxTextLen] = {0};
        ImGui::InputText("name", buf, text_.capacity());
        text_ = buf;
        if (ImGui::Button("OK")) {
            if (callback_) {
                callback_(text_);
            }
            Hide();
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button("Cancel")) {
            Hide();
            ImGui::CloseCurrentPopup();
            callback_ = nullptr;
        }
        ImGui::EndPopup();
    }
}