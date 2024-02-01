#include "show_component.hpp"
#include "asset_list_window.hpp"
#include "context.hpp"
#include "image_view_canva.hpp"

ComponentShowMethods::show_fn ComponentShowMethods::Find(type_info type) {
    if (auto it = methods_.find(type); it != methods_.end()) {
        return it->second;
    }

    if (type->is_numeric()) {
        return DefaultMethods::ShowNumeric;
    }

    if (type->is_boolean()) {
        return DefaultMethods::ShowBoolean;
    }

    if (type->is_class()) {
        return DefaultMethods::ShowClass;
    }

    if (type->is_enum()) {
        return DefaultMethods::ShowEnum;
    }

    if (type->is_optional()) {
        return DefaultMethods::ShowOptional;
    }

    if (type->is_string()) {
        return DefaultMethods::ShowString;
    }

    return nullptr;
}

void ComponentShowMethods::DefaultMethods::ShowClass(type_info parent,
                                                     std::string_view name,
                                                     ::mirrow::drefl::any& obj,
                                                     gecs::registry reg) {
    if (ImGui::TreeNode(name.data())) {
        auto classInfo = obj.type_info()->as_class();
        for (auto&& var : classInfo->properties()) {
            auto varType = var->type_info();
            auto ref = var->call(obj);
            DisplayComponent(varType, var->name(), ref, reg);
        }

        ImGui::TreePop();
    }
}

void ComponentShowMethods::DefaultMethods::ShowNumeric(
    type_info parent, std::string_view name, ::mirrow::drefl::any& obj,
    gecs::registry) {
    auto numeric = obj.type_info()->as_numeric();

    ImGui::BeginDisabled(obj.is_constref());
    if (numeric->is_integer()) {
        int i = numeric->get_value(obj);
        ImGui::DragInt(name.data(), &i);
        if (!obj.is_constref()) {
            numeric->set_value(obj, (long)i);
        }
    } else {
        float f = numeric->get_value(obj);
        ImGui::DragFloat(name.data(), &f);
        if (!obj.is_constref()) {
            numeric->set_value(obj, f);
        }
    }
    ImGui::EndDisabled();
}

void ComponentShowMethods::DefaultMethods::ShowBoolean(
    type_info parent, std::string_view name, ::mirrow::drefl::any& obj,
    gecs::registry) {
    auto boolean = obj.type_info()->as_boolean();

    ImGui::BeginDisabled(obj.is_constref());
    ImGui::Checkbox(name.data(), mirrow::drefl::try_cast<bool>(obj));
    ImGui::EndDisabled();
}

void ComponentShowMethods::DefaultMethods::ShowString(type_info parent,
                                                      std::string_view name,
                                                      ::mirrow::drefl::any& obj,
                                                      gecs::registry) {
    auto string_type = obj.type_info()->as_string();
    if (string_type->is_string()) {
        ImGui::BeginDisabled(obj.is_constref());
        auto str = string_type->get_str(obj);
        char buf[1024] = {0};
        strcpy(buf, str.c_str());
        ImGui::InputText(name.data(), buf, sizeof(buf),
                         obj.is_constref() ? ImGuiInputTextFlags_ReadOnly : 0);
        if (!obj.is_constref() && buf != str) {
            string_type->set_value(obj, std::string(buf));
        }
        ImGui::EndDisabled();
    } else {
        auto view = string_type->get_str_view(obj);
        ImGui::InputText(name.data(), (char*)view.data(), view.size(),
                         ImGuiInputTextFlags_ReadOnly);
    }
}

void ComponentShowMethods::DefaultMethods::ShowEnum(type_info parent,
                                                    std::string_view name,
                                                    ::mirrow::drefl::any& obj,
                                                    gecs::registry) {
    auto enum_info = obj.type_info()->as_enum();

    static std::vector<const char*> enumNames;
    enumNames.clear();

    int curItem = enum_info->get_value(obj);
    int idx = 0;
    auto& enums = enum_info->enums();

    for (int i = 0; i < enums.size(); i++) {
        auto& e = enums[i];

        enumNames.push_back(e.name().c_str());

        if (e.value() == curItem) {
            idx = i;
        }
    }

    ImGui::BeginDisabled(obj.is_constref());
    ImGui::Combo(name.data(), &idx, enumNames.data(), enumNames.size());
    ImGui::EndDisabled();

    if (!obj.is_constref()) {
        enum_info->set_value(obj, enums[idx].value());
    }
}

void ComponentShowMethods::DefaultMethods::ShowOptional(
    type_info parent, std::string_view name, ::mirrow::drefl::any& obj,
    gecs::registry reg) {
    auto optional_type = obj.type_info()->as_optional();
    if (optional_type->has_value(obj)) {
        auto elem = optional_type->get_value(obj);

        DisplayComponent(elem.type_info(), name, elem, reg);
    } else {
        if (ImGui::TreeNode(name.data())) {
            if (optional_type->elem_type()->is_default_constructible()) {
                if (ImGui::Button("create")) {
                    auto newValue =
                        optional_type->elem_type()->default_construct();
                    optional_type->set_inner_value(newValue, obj);
                }
            } else {
                ImGui::Text("none");
            }
            ImGui::TreePop();
        }
    }
}

void DisplayVec2(const mirrow::drefl::type* parent, std::string_view name,
                 mirrow::drefl::any& obj, gecs::registry) {
    auto vec = mirrow::drefl::try_cast<nickel::cgmath::Vec2>(obj);
    ImGui::BeginDisabled(obj.is_constref());
    ImGui::DragFloat2(name.data(), vec->data);
    ImGui::EndDisabled();
}

void DisplayVec3(const mirrow::drefl::type* parent, std::string_view name,
                 mirrow::drefl::any& obj, gecs::registry) {
    auto vec = mirrow::drefl::try_cast<nickel::cgmath::Vec3>(obj);
    ImGui::BeginDisabled(obj.is_constref());
    ImGui::DragFloat3(name.data(), vec->data);
    ImGui::EndDisabled();
}

void DisplayVec4(const mirrow::drefl::type* parent, std::string_view name,
                 mirrow::drefl::any& obj, gecs::registry) {
    auto& attrs = parent->attributes();
    ImGui::BeginDisabled(obj.is_constref());
    auto vec = mirrow::drefl::try_cast<nickel::cgmath::Vec4>(obj);
    if (std::find(attrs.begin(), attrs.end(), nickel::AttrRange01) !=
        attrs.end()) {
        ImGui::DragFloat4(name.data(), vec->data);
    } else if (std::find(attrs.begin(), attrs.end(), nickel::AttrColor) !=
               attrs.end()) {
        ImGui::ColorEdit4(name.data(), vec->data);
    } else {
        ImGui::DragFloat4(name.data(), vec->data);
    }
    ImGui::EndDisabled();
}

template <typename AssetType>
AssetListWindow<AssetType>* GetAssetListWindow(EditorContext& ctx) {
    if constexpr (std::is_same_v<AssetType, nickel::Texture>) {
        return &ctx.textureAssetListWindow;
    } else if constexpr (std::is_same_v<AssetType, nickel::Font>) {
        return &ctx.fontAssetListWindow;
    } else if constexpr (std::is_same_v<AssetType, nickel::Sound>) {
        return &ctx.soundAssetListWindow;
    } else if constexpr (std::is_same_v<AssetType, nickel::Animation>) {
        return &ctx.animAssetListWindow;
    } else if constexpr (std::is_same_v<AssetType, nickel::Tilesheet>) {
        return &ctx.tilesheetAssetListWindow;
    }
    return nullptr;
}

template <typename HandleType>
void LoadAssetSelector(EditorContext& ctx, std::string_view buttonText,
                       std::function<void(HandleType)> callback = nullptr) {
    if (ImGui::Selectable(buttonText.data())) {
        auto& window = *GetAssetListWindow<typename HandleType::ValueType>(ctx);
        window.Show();
        window.SetSelectCallback(callback);
    }
}

template <typename HandleType>
void AssetCreator(
    EditorContext& ctx, const std::string& name,
    std::function<void(const std::filesystem::path&)> createCallback) {
    if (ImGui::Selectable("create")) {
        auto filetype =
            nickel::DetectFileType<typename HandleType::ValueType>();
        auto extension = nickel::GetMetaFileExtension(filetype);

        auto filename = SaveFileDialog(name.c_str(), {extension.data()});

        if (filename.empty()) {
            return;
        }

        filename = filename.extension() != extension
                       ? filename.replace_extension(extension)
                       : filename;

        if (createCallback) {
            createCallback(filename);
        }
    }
}

template <typename T>
struct show_tmpl;

template <typename HandleType>
bool BeginDisplayHandle(
    EditorContext& ctx, std::string_view name, nickel::AssetManager& mgr,
    HandleType handle,
    std::function<void(HandleType)> handleChangeCallback = nullptr,
    std::function<void(const std::filesystem::path&)> createCallback =
        nullptr) {
    char buf[1024] = {0};
    if (mgr.Has(handle)) {
        auto& asset = mgr.Get(handle);
        std::filesystem::path path = asset.RelativePath();
        snprintf(buf, sizeof(buf), "Res://%s", path.string().c_str());
    } else {
        snprintf(buf, sizeof(buf), "no asset");
    }

    char id[1024] = {0};
    snprintf(id, sizeof(id), "##%s", name.data());
    if (ImGui::BeginCombo(id, buf)) {
        if (createCallback) {
            AssetCreator<HandleType>(ctx, "create", createCallback);
        }
        if (handleChangeCallback) {
            LoadAssetSelector<HandleType>(ctx, "load", handleChangeCallback);
        }
        return true;
    }

    return false;
}

template <>
bool BeginDisplayHandle<nickel::TextureHandle>(
    EditorContext& ctx, std::string_view name, nickel::AssetManager& mgr,
    nickel::TextureHandle handle,
    std::function<void(nickel::TextureHandle)> handleChangeCallback,
    std::function<void(const std::filesystem::path&)> createCallback) {
    char buf[1024] = {0};
    if (mgr.Has(handle)) {
        auto& asset = mgr.Get(handle);
        std::filesystem::path path = asset.RelativePath();
        snprintf(buf, sizeof(buf), "Res://%s", path.string().c_str());
    } else {
        snprintf(buf, sizeof(buf), "no asset");
    }

    float size = ImGui::GetWindowContentRegionMax().x -
                 (ImGui::GetItemRectMin().x - ImGui::GetWindowPos().x);
    static ImageViewCanva imageViewer;
    if (mgr.Has(handle)) {
        imageViewer.ChangeTexture(handle);
        imageViewer.Resize({size, size});
        imageViewer.Update();
    }

    char id[1024] = {0};
    snprintf(id, sizeof(id), "##%s", name.data());
    if (ImGui::BeginCombo(id, buf)) {
        if (createCallback) {
            AssetCreator<nickel::TextureHandle>(ctx, "create", createCallback);
        }
        if (handleChangeCallback) {
            LoadAssetSelector(ctx, "load", handleChangeCallback);
        }
        return true;
    }

    return false;
}

void EndDisplayHandle() {
    ImGui::EndCombo();
}

void DisplaySprite(const mirrow::drefl::type* parent, std::string_view name,
                   mirrow::drefl::any& obj, gecs::registry reg) {
    auto classInfo = obj.type_info()->as_class();
    for (auto&& prop : classInfo->properties()) {
        if (prop->type_info() ==
            ::mirrow::drefl::typeinfo<nickel::TextureHandle>()) {
            continue;
        }

        auto member = prop->call(obj);
        DisplayComponent(member.type_info(), prop->name(), member, reg);
    }

    auto& sprite = *mirrow::drefl::try_cast<nickel::Sprite>(obj);
    auto mgr = reg.res_mut<nickel::AssetManager>();

    auto changeHandle = [&](nickel::TextureHandle h) { sprite.texture = h; };
    auto& ctx = *reg.res_mut<EditorContext>();

    if (BeginDisplayHandle<nickel::TextureHandle>(*reg.res_mut<EditorContext>(),
                                                  name, *mgr, sprite.texture,
                                                  changeHandle)) {
        if (ImGui::Selectable("load tilesheet")) {
            ctx.tilesheetAssetListWindow.Show();
            ctx.tilesheetAssetListWindow.SetSelectCallback(
                [&](nickel::TilesheetHandle h) {
                    ctx.tilesheetEditor.Show();
                    ctx.tilesheetEditor.ChangeTilesheet(h);
                    ctx.tilesheetEditor.SetSelectCallback(
                        [&](nickel::Tile tile) {
                            sprite.texture = tile.handle;
                            sprite.region = tile.region;
                            sprite.customSize = tile.region.size;
                        });
                });
        }
        if (ImGui::Selectable("create tilesheet")) {
            ctx.textureAssetListWindow.Show();
            ctx.textureAssetListWindow.SetSelectCallback(
                [&](nickel::TextureHandle h) {
                    auto filename =
                        SaveFileDialog("create new tilesheet", {".tilesheet"});
                    if (!filename.empty()) {
                        filename =
                            filename.extension() != ".tilesheet"
                                ? filename.replace_extension(".tilesheet")
                                : filename;
                        ctx.tilesheetEditor.Show();
                        auto& tilesheetMgr = mgr->TilesheetMgr();
                        auto handle = tilesheetMgr.Create(h, 1, 1);
                        tilesheetMgr.AssociateFile(
                            handle, ctx.GetRelativePath(filename));
                        ctx.tilesheetEditor.ChangeTilesheet(handle);
                        ctx.tilesheetEditor.SetSelectCallback(
                            [&](nickel::Tile tile) {
                                sprite.texture = tile.handle;
                                sprite.region = tile.region;
                                sprite.customSize = tile.region.size;
                            });
                    }
                });
        }

        EndDisplayHandle();
    }
}

void DisplayTextureHandle(const mirrow::drefl::type* parent,
                          std::string_view name, mirrow::drefl::any& obj,
                          gecs::registry reg) {
    auto& handle = *mirrow::drefl::try_cast<nickel::TextureHandle>(obj);
    auto mgr = reg.res_mut<nickel::AssetManager>();

    if (BeginDisplayHandle(*reg.res_mut<EditorContext>(), name, *mgr, handle)) {
        EndDisplayHandle();
    }
}

void DisplayAnimationPlayer(const mirrow::drefl::type* parent,
                            std::string_view name, mirrow::drefl::any& obj,
                            gecs::registry reg) {
    nickel::AnimationPlayer& player =
        *mirrow::drefl::try_cast<nickel::AnimationPlayer>(obj);
    auto handle = player.Anim();
    auto& mgr = *reg.res_mut<nickel::AssetManager>();

    auto create = [&](const std::filesystem::path& path) {
        auto ctx = nickel::ECS::Instance().World().cur_registry()->res_mut<EditorContext>();
        auto handle = mgr.AnimationMgr().Create(
            std::make_unique<nickel::Animation>(), ctx->GetRelativePath(path));
        ctx->animEditor.sequence->player.ChangeAnim(handle);
        ctx->animEditor.sequence->entity = ctx->entityListWindow.GetSelected();
    };

    auto changeHandle = [&player](nickel::AnimationHandle handle) {
        player.ChangeAnim(handle);
        nickel::ECS::Instance().World().cur_registry()
            ->res_mut<EditorContext>()
            ->animEditor.sequence->player.ChangeAnim(handle);
    };

    if (BeginDisplayHandle<nickel::AnimationHandle>(
            *reg.res_mut<EditorContext>(), name, mgr, handle, changeHandle,
            create)) {
        EndDisplayHandle();
    }
}

void DisplayLabel(const mirrow::drefl::type* parent, std::string_view name,
                  mirrow::drefl::any& value, gecs::registry reg) {
    auto& label = *mirrow::drefl::try_cast<nickel::ui::Label>(value);
    auto& fontMgr = reg.res_mut<nickel::AssetManager>()->FontMgr();

    static char buf[1024] = {0};
    auto text = label.GetText().to_string();
    std::strcpy(buf, text.c_str());

    ImGui::InputTextMultiline("text", buf, sizeof(buf));

    if (buf != text) {
        nickel::utf8string str{buf};
        label.SetText(str);
    }

    ImGui::ColorEdit4("color", label.color.data);
    ImGui::ColorEdit4("pressing color", label.pressColor.data);
    ImGui::ColorEdit4("hovering color", label.hoverColor.data);

    // show font
    auto handle = label.GetFont();
    auto& mgr = *reg.res_mut<nickel::AssetManager>();

    auto changeHandle = [&](nickel::FontHandle h) { label.ChangeFont(h); };

    if (BeginDisplayHandle<nickel::FontHandle>(
            *reg.res_mut<EditorContext>(), name, mgr, handle, changeHandle)) {
        EndDisplayHandle();
    }

    // show pt
    int size = label.GetPtSize();
    ImGui::DragInt("pt", &size, 1.0, 4);
    if (size != label.GetPtSize()) {
        label.SetPtSize(size);
    }
}

void DisplaySoundPlayer(const mirrow::drefl::type* parent,
                        std::string_view name, mirrow::drefl::any& value,
                        gecs::registry reg) {
    auto& player = *mirrow::drefl::try_cast<nickel::SoundPlayer>(value);

    auto& mgr = *reg.res_mut<nickel::AssetManager>();
    auto ctx = reg.res_mut<EditorContext>();

    auto changeHandle = [&](nickel::SoundHandle h) { player.ChangeSound(h, reg.res_mut<nickel::AssetManager>()->AudioMgr()); };

    auto handle = player.Handle();

    if (BeginDisplayHandle<nickel::SoundHandle>(
            *reg.res_mut<EditorContext>(), name, mgr, handle, changeHandle)) {
        EndDisplayHandle();
    }

    ImGui::SameLine();

    if (mgr.Has(player.Handle())) {
        auto& sound = mgr.Get(player.Handle());
        if (ImGui::Button("edit")) {
            ctx->soundPropWindow.ChangeAudio(player.Handle());
            ctx->soundPropWindow.Show();
        }
    }
}

void DisplayScript(const mirrow::drefl::type* parent, std::string_view name,
                  mirrow::drefl::any& value, gecs::registry reg) {
    nickel::ScriptHandle& handle = *mirrow::drefl::try_cast<nickel::ScriptHandle>(value);
    auto& mgr = *reg.res_mut<nickel::AssetManager>();
    auto ctx = reg.res_mut<EditorContext>();

    auto changeHandle = [&](nickel::ScriptHandle h) { handle = h; };

    if (BeginDisplayHandle<nickel::ScriptHandle>(ctx.get(), name, mgr, handle, changeHandle)) {
        EndDisplayHandle();
    }
}