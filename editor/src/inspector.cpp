#include "inspector.hpp"
#include "context.hpp"
#include "type_displayer.hpp"

void ComponentDisplayWidget::Update() {
    auto reg = nickel::ECS::Instance().World().cur_registry();
    auto entity = EditorContext::Instance().entityListWindow.GetSelected();
    auto cmds = reg->commands();

    auto& types = mirrow::drefl::all_typeinfo();

    if (!reg->alive(entity)) {
        return;
    }

    int imguiID = 0;
    for (auto [name, typeInfo] : types) {
        if (reg->has(entity, typeInfo)) {
            auto& attrs = typeInfo->attributes();

            if (std::find(attrs.begin(), attrs.end(),
                          nickel::EditorNodisplay) != attrs.end()) {
                continue;
            }

            auto data = reg->get_mut(entity, typeInfo);

            ImGui::PushID(imguiID++);
            if (ImGui::Button("delete")) {
                cmds.remove(entity, typeInfo);
                ImGui::PopID();
                continue;
            }
            ImGui::PopID();
            ImGui::SameLine();
            if (ImGui::CollapsingHeader(typeInfo->name().c_str())) {
                TypeDisplayerRegistrar::Instance().Display(data);
            }
        }
    }

    // show add componet button
    auto& spawnMethods = SpawnComponentMethods::Instance();
    auto& items = spawnMethods.RegistedTypes();

    char names[1024] = {0};
    int idx = 0;
    for (auto& item : items) {
        strncpy(names + idx, item->name().c_str(), item->name().size());
        Assert(idx <= sizeof(names), "out of range");
        idx += item->name().size() + 1;
    }

    ImGui::Separator();

    static int selectedItem = -1;

    ImGui::Combo("components", &selectedItem, names);

    static bool replaceHintOpen = false;
    static bool shouldSpawn = false;

    if (ImGui::Button("add component") && selectedItem >= 0) {
        if (reg->has(entity, items[selectedItem])) {
            replaceHintOpen = true;
        } else {
            shouldSpawn = true;
        }
    }

    if (replaceHintOpen) {
        ImGui::SetNextWindowPos(ImVec2(100, 100), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowFocus();
        if (ImGui::Begin("spawn hint")) {
            ImGui::Text("component already exists, do you want replace it?");
            if (ImGui::Button("yes")) {
                shouldSpawn = true;
                replaceHintOpen = false;
            }

            ImGui::SameLine();
            if (ImGui::Button("cancel")) {
                replaceHintOpen = false;
            }
        }
        ImGui::End();
    }

    if (shouldSpawn) {
        spawnMethods.Spawn(items[selectedItem], entity);
        shouldSpawn = false;
    }
   
}

void InspectorWindow::update() {
    curWidget_->Update();
}
