#include "spawn_component.hpp"

void SpawnComponentMethods::Spawn(const mirrow::drefl::type* type,
                                  gecs::entity ent) {
    if (auto it = methods_.find(type); it != methods_.end()) {
        it->second(ent);
    }
}

const std::vector<SpawnComponentMethods::TypeInfo>&
SpawnComponentMethods::RegistedTypes() const {
    return types_;
}

void RegistSpawnMethods() {
    auto& instance = SpawnComponentMethods::Instance();

    instance.Regist<nickel::Transform>();
    instance.Regist<nickel::GlobalTransform>();
    instance.Regist<nickel::Sprite>();
    instance.Regist<nickel::SoundPlayer>();
    instance.Regist<nickel::SpriteMaterial>();
    instance.Regist<nickel::AnimationPlayer>();
    // instance.Regist<nickel::ui::Style>(GeneralSpawnMethod<nickel::ui::Style>);
    // instance.Regist<nickel::ui::Button>(GeneralSpawnMethod<nickel::ui::Button>);
    // instance.Regist<nickel::ui::Label>(GeneralSpawnMethod<nickel::ui::Label>);
}
