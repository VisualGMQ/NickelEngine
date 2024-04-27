#include "type_displayer.hpp"
#include "context.hpp"
#include "image_view_canva.hpp"
#include "util.hpp"

void DisplayNumeric(mirrow::drefl::any& payload) {
    Assert(payload.type_info()->is_numeric(),
           "payload must be numeric in DisplayNumeric");
    auto typeinfo = payload.type_info()->as_numeric();

    // static std::unordered_map<enum mirrow::drefl::numeric::numeric_kind,
    //                           std::string_view>
    //     formatterMap = {
    //         {mirrow::drefl::numeric::Unknown,   "%d"},
    //         {   mirrow::drefl::numeric::Char, "%hhd"},
    //         {  mirrow::drefl::numeric::Short,  "%hd"},
    //         {    mirrow::drefl::numeric::Int,   "%d"},
    //         {   mirrow::drefl::numeric::Long,   "%d"},
    //         {  mirrow::drefl::numeric::Uint8, "%hho"},
    //         { mirrow::drefl::numeric::Uint16,  "%ho"},
    //         { mirrow::drefl::numeric::Uint32,   "%o"},
    //         { mirrow::drefl::numeric::Uint64, "%llo"},
    //         {  mirrow::drefl::numeric::Float,   "%f"},
    //         { mirrow::drefl::numeric::Double,  "%lf"},
    // };

    char id[128] = {0};
    IMGUI_MAKE_EMPTY_ID(id, payload);

    ImGui::BeginDisabled(payload.is_constref());
    if (typeinfo->is_integer()) {
        int value = typeinfo->get_value(payload);
        ImGui::DragInt(id, &value);
        if (!payload.is_constref()) {
            typeinfo->set_value(payload, (long)value);
        }
    } else {
        float value = typeinfo->get_value(payload);
        ImGui::DragFloat(id, &value);
        if (!payload.is_constref()) {
            typeinfo->set_value(payload, value);
        }
    }
    ImGui::EndDisabled();
}

void DisplayBoolean(mirrow::drefl::any& payload) {
    Assert(payload.type_info()->is_boolean(),
           "payload must be boolean in DisplayBoolean");

    auto typeinfo = payload.type_info()->as_boolean();

    char id[128] = {0};
    IMGUI_MAKE_EMPTY_ID(id, payload);

    auto value = typeinfo->get_value(payload);
    ImGui::BeginDisabled(payload.is_constref());
    ImGui::Checkbox(id, &value);
    ImGui::EndDisabled();

    if (!payload.is_constref()) {
        typeinfo->set_value(payload, value);
    }
}

void DisplayString(mirrow::drefl::any& payload) {
    Assert(payload.type_info()->is_string(),
           "payload must be string in DisplayString");

    auto typeinfo = payload.type_info()->as_string();

    bool isConst = payload.is_constref() || typeinfo->is_string_view();

    char buf[1024] = {0};
    strcpy(buf, typeinfo->get_str_view(payload).data());

    char id[128] = {0};
    IMGUI_MAKE_EMPTY_ID(id, payload);

    ImGui::BeginDisabled(isConst);
    ImGui::InputText(id, buf, sizeof(buf));
    ImGui::EndDisabled();

    if (!isConst && typeinfo->get_str_view(payload) != buf) {
        typeinfo->set_value(payload, buf);
    }
}

void DisplayEnum(mirrow::drefl::any& payload) {
    Assert(payload.type_info()->is_enum(),
           "payload must be enum in DisplayEnum");

    auto typeinfo = payload.type_info()->as_enum();

    int curItem = -1;

    char items[2048] = {0};
    int idx = 0;
    for (int i = 0; i < typeinfo->enums().size(); i++) {
        auto& item = typeinfo->enums()[i];
        strcpy(items + idx, item.name().c_str());
        idx += item.name().size() + 1;
        Assert(idx <= 2048, "out of range");
        if (item.value() == typeinfo->get_value(payload)) {
            curItem = i;
        }
    }

    char id[128] = {0};
    IMGUI_MAKE_EMPTY_ID(id, payload);

    ImGui::BeginDisabled(payload.is_constref());
    ImGui::Combo(id, &curItem, items);
    ImGui::EndDisabled();

    if (!payload.is_constref() && curItem >= 0) {
        typeinfo->set_value(payload, typeinfo->enums()[curItem].value());
    }
}

void DisplayClass(mirrow::drefl::any& payload) {
    Assert(payload.type_info()->is_class(),
           "payload must be class in DisplayClass");

    auto typeinfo = payload.type_info()->as_class();

    for (auto& prop : typeinfo->properties()) {
        auto& attrs = prop->attributes();

        if (prop->is_const()) {
            auto propPayload = prop->call_const(payload);
            ImGui::Text("%s", prop->name().c_str());

            if (std::find(attrs.begin(), attrs.end(),
                          nickel::Attribute::AttrColor) !=
                prop->attributes().end()) {
                ImGui::SameLine();
                DisplayColor(propPayload);
            } else {
                TypeDisplayerRegistrar::Instance().Display(propPayload, true);
            }
        } else {
            auto propPayload = prop->call(payload);
            ImGui::Text("%s", prop->name().c_str());

            if (std::find(attrs.begin(), attrs.end(),
                          nickel::Attribute::AttrColor) !=
                prop->attributes().end()) {
                ImGui::SameLine();
                DisplayColor(propPayload);
            } else {
                TypeDisplayerRegistrar::Instance().Display(propPayload, true);
            }
        }
    }
}

void DisplayTextureHandle(mirrow::drefl::any& payload) {
    Assert(
        payload.type_info() == mirrow::drefl::typeinfo<nickel::TextureHandle>(),
        "payload must be TextureHandle type");

    auto mgr = nickel::ECS::Instance().World().res<nickel::TextureManager>();

    nickel::TextureHandle handle =
        *mirrow::drefl::try_cast_const<nickel::TextureHandle>(payload);

    char id[128] = {0};
    IMGUI_MAKE_EMPTY_ID(id, payload);

    if (!mgr->Has(handle)) {
        char str[MAX_PATH_LENGTH] = "invalid texture";
        ImGui::BeginDisabled();
        ImGui::InputText(id, str, sizeof(str));
        ImGui::EndDisabled();
        return;
    }

    auto& texture = mgr->Get(handle);

    float size = ImGui::GetWindowContentRegionMax().x -
                 ImGui::GetStyle().WindowPadding.x * 2.0;

    static ImageViewCanva canva;
    canva.ChangeTexture(handle);
    canva.Resize({size, size});
    canva.Update();

    char buf[512] = {0};
    snprintf(buf, sizeof(buf), "Res://%s",
             texture.RelativePath().string().c_str());
    ImGui::InputText("path", buf, sizeof(buf), ImGuiInputTextFlags_ReadOnly);
}

void DisplaySoundHandle(mirrow::drefl::any& payload) {
    Assert(
        mirrow::drefl::typeinfo<nickel::SoundHandle>() == payload.type_info(),
        "payload must be SoundHandle");

    auto mgr = nickel::ECS::Instance().World().res<nickel::AudioManager>();

    auto handle = *mirrow::drefl::try_cast_const<nickel::SoundHandle>(payload);

    if (!mgr->Has(handle)) {
        char str[MAX_PATH_LENGTH] = "invalid sound";
        ImGui::BeginDisabled();
        ImGui::InputText("###invalid-sound", str, sizeof(str));
        ImGui::EndDisabled();
        return;
    }

    auto& sound = mgr->Get(handle);
    char buf[512] = {0};
    snprintf(buf, sizeof(buf), "Res://%s",
             sound.RelativePath().string().c_str());
    ImGui::InputText("path", buf, sizeof(buf), ImGuiInputTextFlags_ReadOnly);
}

void DisplayAnimationHandle(mirrow::drefl::any& payload) {
    Assert(mirrow::drefl::typeinfo<nickel::AnimationHandle>() ==
               payload.type_info(),
           "payload must be AnimationHandle");

    auto mgr = nickel::ECS::Instance().World().res<nickel::AnimationManager>();

    auto handle =
        *mirrow::drefl::try_cast_const<nickel::AnimationHandle>(payload);

    if (!mgr->Has(handle)) {
        char str[MAX_PATH_LENGTH] = "invalid animation";
        ImGui::BeginDisabled();
        ImGui::InputText("###invalid-animation", str, sizeof(str));
        ImGui::EndDisabled();
        return;
    }

    auto& anim = mgr->Get(handle);
    char buf[512] = {0};
    snprintf(buf, sizeof(buf), "Res://%s",
             anim.RelativePath().string().c_str());
    ImGui::InputText("path", buf, sizeof(buf), ImGuiInputTextFlags_ReadOnly);
}

void DisplayOptional(mirrow::drefl::any& payload) {
    Assert(payload.type_info()->is_optional(),
           "payload must be std::optional<> type");

    auto typeinfo = payload.type_info()->as_optional();
    auto elemTypeinfo = typeinfo->elem_type();

    if (typeinfo->has_value(payload)) {
        if (payload.is_constref()) {
            auto value = typeinfo->get_value_const(payload);
            TypeDisplayerRegistrar::Instance().Display(value, true);
        } else {
            auto value = typeinfo->get_value(payload);
            TypeDisplayerRegistrar::Instance().Display(value, true);
        }
    } else {
        if (payload.is_constref()) {
            ImGui::SameLine();
            ImGui::Text("empty");
        } else {
            if (elemTypeinfo->is_default_constructible()) {
                char label[64] = {0};
                IMGUI_MAKE_UNIID(label, "create", payload);
                if (ImGui::Button(label)) {
                    typeinfo->set_inner_value(elemTypeinfo->default_construct(),
                                              payload);
                }
            } else {
                ImGui::SameLine();
                ImGui::Text("empty(can't construct)");
            }
        }
    }
}

void DisplayArray(mirrow::drefl::any& payload) {
    Assert(payload.type_info()->is_array(), "payload must be array");

    auto typeinfo = payload.type_info()->as_array();
    auto elemTypeinfo = typeinfo->elem_type();

    if (typeinfo->size(payload) == 0) {
        ImGui::SameLine();
        ImGui::Text("empty");
        return;
    }

    char buf[512] = {0};
    for (int i = 0; i < typeinfo->size(payload); i++) {
        snprintf(buf, sizeof(buf), "elem %d", i);
        if (ImGui::TreeNodeEx(buf, ImGuiTreeNodeFlags_None)) {
            auto elem = typeinfo->get(i, payload);
            TypeDisplayerRegistrar::Instance().Display(elem);

            ImGui::TreePop();
        }
    }
}

void DisplayVec3(mirrow::drefl::any& payload) {
    Assert(
        payload.type_info() == mirrow::drefl::typeinfo<nickel::cgmath::Vec3>(),
        "payload must be Vec3 type");

    char id[128] = {0};
    IMGUI_MAKE_EMPTY_ID(id, payload);

    if (payload.is_constref()) {
        auto vec = mirrow::drefl::try_cast_const<nickel::cgmath::Vec3>(payload);
        ImGui::BeginDisabled();
        ImGui::DragFloat3(id, (float*)vec->data);
        ImGui::EndDisabled();
    } else {
        auto vec = mirrow::drefl::try_cast<nickel::cgmath::Vec3>(payload);
        ImGui::DragFloat3(id, vec->data);
    }
}

void DisplayVec2(mirrow::drefl::any& payload) {
    Assert(
        payload.type_info() == mirrow::drefl::typeinfo<nickel::cgmath::Vec2>(),
        "payload must be Vec2 type");

    char id[128] = {0};
    IMGUI_MAKE_EMPTY_ID(id, payload);

    if (payload.is_constref()) {
        auto vec = mirrow::drefl::try_cast_const<nickel::cgmath::Vec2>(payload);
        ImGui::BeginDisabled();
        ImGui::DragFloat2(id, (float*)vec->data);
        ImGui::EndDisabled();
    } else {
        auto vec = mirrow::drefl::try_cast<nickel::cgmath::Vec2>(payload);
        ImGui::DragFloat2(id, vec->data);
    }
}

void DisplayVec4(mirrow::drefl::any& payload) {
    Assert(
        mirrow::drefl::typeinfo<nickel::cgmath::Vec4>() == payload.type_info(),
        "payload must be Vec4");

    char id[128] = {0};
    IMGUI_MAKE_EMPTY_ID(id, payload);

    ImGui::BeginDisabled(payload.is_constref());
    auto vec = (nickel::cgmath::Vec4*)
        mirrow::drefl::try_cast_const<nickel::cgmath::Vec4>(payload);
    ImGui::DragFloat4(id, vec->data);
    ImGui::EndDisabled();
}

void DisplayColor(mirrow::drefl::any& payload) {
    Assert(
        mirrow::drefl::typeinfo<nickel::cgmath::Vec4>() == payload.type_info(),
        "payload must be Vec4");

    char id[128] = {0};
    IMGUI_MAKE_EMPTY_ID(id, payload);

    ImGui::BeginDisabled(payload.is_constref());
    auto vec = (nickel::cgmath::Vec4*)
        mirrow::drefl::try_cast_const<nickel::cgmath::Vec4>(payload);
    ImGui::ColorEdit4(id, vec->data);
    ImGui::EndDisabled();
}

void DisplayAnimationPlayer(mirrow::drefl::any& payload) {
    Assert(mirrow::drefl::typeinfo<nickel::AnimationPlayer>() ==
               payload.type_info(),
           "payload must be AnimationPlayer");

    auto& player =
        *mirrow::drefl::try_cast_const<nickel::AnimationPlayer>(payload);
    auto mgr =
        nickel::ECS::Instance().World().res_mut<nickel::AnimationManager>();
    auto handle = player.Anim();

    auto create = [&](const std::filesystem::path& path) {
        auto& ctx = EditorContext::Instance();
        auto handle = mgr->Create(std::make_unique<nickel::Animation>(),
                                  ctx.GetRelativePath(path));
        ctx.animEditor.sequence->player.ChangeAnim(handle);
        ctx.animEditor.sequence->entity =
            EditorContext::Instance().entityListWindow.GetSelected();
    };

    ImGui::Text("animation");
    ImGui::SameLine();
    if (!payload.is_constref()) {
        auto obj = mirrow::drefl::any_make_ref(handle);
        DisplayAnimationHandle(obj);

        char imguiID[64] = {0};
        IMGUI_MAKE_EMPTY_ID(imguiID, payload);
        if (ImGui::BeginCombo(imguiID, "", ImGuiComboFlags_NoPreview)) {
            if (ImGui::Selectable("load")) {
                auto changeHandle = [&payload](nickel::AnimationHandle handle) {
                    auto& mutablePlayer =
                        *mirrow::drefl::try_cast<nickel::AnimationPlayer>(
                            payload);
                    mutablePlayer.ChangeAnim(handle);
                    EditorContext::Instance()
                        .animEditor.sequence->player.ChangeAnim(handle);
                };

                auto& animListWindow =
                    EditorContext::Instance().animAssetListWindow;
                animListWindow.Show();
                animListWindow.SetSelectCallback(changeHandle);
            }
            if (ImGui::Selectable("create")) {
                auto extension =
                    nickel::GetMetaFileExtension(nickel::FileType::Animation)
                        .data();

                auto filename = SaveFileDialog("new animation", {extension});
                auto relativePath = EditorContext::Instance().GetRelativePath(
                    filename.replace_extension(extension));
                if (!filename.empty()) {
                    auto handle = mgr->Create(
                        std::make_unique<nickel::Animation>(), relativePath);
                    mgr->Get(handle).Save2AssociateFile();
                    auto mutablePlayer =
                        mirrow::drefl::try_cast<nickel::AnimationPlayer>(
                            payload);
                    mutablePlayer->ChangeAnim(handle);
                }
            }
            ImGui::EndCombo();
        }
    } else {
        auto obj = mirrow::drefl::any_make_constref(handle);
        DisplayAnimationHandle(obj);
    }
}

void DisplaySoundPlayer(mirrow::drefl::any& payload) {
    Assert(
        mirrow::drefl::typeinfo<nickel::SoundPlayer>() == payload.type_info(),
        "payload must be SoundPlayer");

    auto& player = *mirrow::drefl::try_cast<nickel::SoundPlayer>(payload);

    auto handle = player.Handle();

    ImGui::Text("audio");
    ImGui::SameLine();

    if (payload.is_constref()) {
        auto obj = mirrow::drefl::any_make_constref(handle);
        DisplayAnimationHandle(obj);
    } else {
        char imguiID[64] = {0};
        IMGUI_MAKE_EMPTY_ID(imguiID, payload);
        if (ImGui::BeginCombo(imguiID, "", ImGuiComboFlags_NoPreview)) {
            if (ImGui::Selectable("load")) {
                auto changeHandle = [&payload](nickel::SoundHandle handle) {
                    auto& mutablePlayer =
                        *mirrow::drefl::try_cast<nickel::SoundPlayer>(payload);
                    mutablePlayer.ChangeSound(handle);
                };

                auto& assetListWindow =
                    EditorContext::Instance().soundAssetListWindow;
                assetListWindow.Show();
                assetListWindow.SetSelectCallback(changeHandle);
            }
            ImGui::EndCombo();
        }

        auto any = mirrow::drefl::any_make_copy(player.Handle());
        TypeDisplayerRegistrar::Instance().Display(any);
    }
}

void DisplayMaterial2DHandle(mirrow::drefl::any& payload) {
    Assert(mirrow::drefl::typeinfo<nickel::Material2DHandle>() ==
               payload.type_info(),
           "payload must be Material2DHandle");

    auto& handle =
        *mirrow::drefl::try_cast_const<nickel::Material2DHandle>(payload);

    auto mgr =
        nickel::ECS::Instance().World().res_mut<nickel::Material2DManager>();

    if (!mgr->Has(handle)) {
        char str[MAX_PATH_LENGTH] = "invalid material2d";
        ImGui::BeginDisabled();
        ImGui::InputText("###invalid-material2d", str, sizeof(str));
        ImGui::EndDisabled();
        return;
    }

    auto& mtl = mgr->Get(handle);
    auto handlePayload = mirrow::drefl::any_make_constref(mtl.GetTexture());
    char buf[512] = {0};
    snprintf(buf, sizeof(buf), "Res://%s", mtl.RelativePath().string().c_str());
    ImGui::InputText("path", buf, sizeof(buf), ImGuiInputTextFlags_ReadOnly);
    DisplayTextureHandle(handlePayload);
}

void DisplaySpriteMaterial(mirrow::drefl::any& payload) {
    Assert(mirrow::drefl::typeinfo<nickel::SpriteMaterial>() ==
               payload.type_info(),
           "payload must be Material2D");

    auto& mtl = *mirrow::drefl::try_cast<nickel::SpriteMaterial>(payload);
    auto& handle = mtl.material;
    auto mgr =
        nickel::ECS::Instance().World().res_mut<nickel::Material2DManager>();

    char buf[64] = "invalid 2d material";
    if (mgr->Has(handle)) {
        auto& mtl = mgr->Get(handle);
        static ImageViewCanva canva;
        canva.Update();
        canva.ChangeTexture(mtl.GetTexture());

        strcpy(buf, mtl.RelativePath().string().c_str());
    }
    ImGui::InputText("path", buf, sizeof(buf), ImGuiInputTextFlags_ReadOnly);

    ImGui::SameLine();

    if (!payload.is_constref()) {
        auto mutablePlayer =
            mirrow::drefl::try_cast<nickel::SpriteMaterial>(payload);

        auto any = mirrow::drefl::any_make_constref(handle);

        ImGui::SameLine();

        char imguiID[64] = {0};
        IMGUI_MAKE_EMPTY_ID(imguiID, payload);
        if (ImGui::BeginCombo(imguiID, "", ImGuiComboFlags_NoPreview)) {
            if (ImGui::Selectable("load")) {
                auto changeHandle =
                    [=](nickel::Material2DHandle handle) {
                        mutablePlayer->material = handle;
                    };

                auto& assetListWindow =
                    EditorContext::Instance().mtl2dAssetListWindow;
                assetListWindow.Show();
                assetListWindow.SetSelectCallback(changeHandle);
            }
            if (ImGui::Selectable("create")) {
                auto extension =
                    nickel::GetMetaFileExtension(nickel::FileType::Material2D)
                        .data();

                auto filename = SaveFileDialog("load 2D material", {extension});
                if (!filename.empty()) {
                    auto relativePath =
                        EditorContext::Instance().GetRelativePath(
                            filename.replace_extension(extension));
                    auto handle = mgr->Create(relativePath);
                    mgr->Get(handle).Save2AssociateFile();
                    auto mutPayload =
                        mirrow::drefl::try_cast<nickel::SpriteMaterial>(
                            payload);
                    mutPayload->material = handle;
                }
            }
            ImGui::EndCombo();
        }
    }
}

void RegistDisplayMethods() {
    auto& inst = TypeDisplayerRegistrar::Instance();
    inst.RegistDefaultEnumFn(DisplayEnum);
    inst.RegistDefaultClassFn(DisplayClass);
    inst.RegistDefaultOptionalFn(DisplayOptional);
    inst.RegistDefaultArrayFn(DisplayArray);

    inst.Regist(mirrow::drefl::typeinfo<bool>(), DisplayBoolean);
    inst.Regist(mirrow::drefl::typeinfo<char>(), DisplayNumeric);
    inst.Regist(mirrow::drefl::typeinfo<short>(), DisplayNumeric);
    inst.Regist(mirrow::drefl::typeinfo<int>(), DisplayNumeric);
    inst.Regist(mirrow::drefl::typeinfo<long long>(), DisplayNumeric);
    inst.Regist(mirrow::drefl::typeinfo<uint8_t>(), DisplayNumeric);
    inst.Regist(mirrow::drefl::typeinfo<uint16_t>(), DisplayNumeric);
    inst.Regist(mirrow::drefl::typeinfo<uint32_t>(), DisplayNumeric);
    inst.Regist(mirrow::drefl::typeinfo<uint64_t>(), DisplayNumeric);
    inst.Regist(mirrow::drefl::typeinfo<float>(), DisplayNumeric);
    inst.Regist(mirrow::drefl::typeinfo<double>(), DisplayNumeric);
    inst.Regist(mirrow::drefl::typeinfo<std::string>(), DisplayString);
    inst.Regist(mirrow::drefl::typeinfo<std::string_view>(), DisplayString);

    inst.Regist(mirrow::drefl::typeinfo<nickel::cgmath::Vec2>(), DisplayVec2);
    inst.Regist(mirrow::drefl::typeinfo<nickel::cgmath::Vec3>(), DisplayVec3);
    inst.Regist(mirrow::drefl::typeinfo<nickel::cgmath::Vec4>(), DisplayVec4);
    inst.Regist(mirrow::drefl::typeinfo<nickel::TextureHandle>(),
                DisplayTextureHandle);
    inst.Regist(mirrow::drefl::typeinfo<nickel::AnimationHandle>(),
                DisplayAnimationHandle);
    inst.Regist(mirrow::drefl::typeinfo<nickel::SoundHandle>(),
                DisplaySoundHandle);
    inst.Regist(mirrow::drefl::typeinfo<nickel::SoundPlayer>(),
                DisplaySoundPlayer);
    inst.Regist(mirrow::drefl::typeinfo<nickel::AnimationPlayer>(),
                DisplayAnimationPlayer);
    inst.Regist(mirrow::drefl::typeinfo<nickel::Material2DHandle>(),
                DisplayMaterial2DHandle);
    inst.Regist(mirrow::drefl::typeinfo<nickel::SpriteMaterial>(),
                DisplaySpriteMaterial);
}
