#pragma once

#include "nickel.hpp"

class SpawnComponentMethods final : public nickel::Singlton<SpawnComponentMethods, false> {
public:
    using SpawnFnType = std::function<void(gecs::entity)>;
    using TypeInfo = const ::mirrow::drefl::type*;

    template <typename T>
    void Regist(SpawnFnType show) {
        Regist(::mirrow::drefl::typeinfo<T>(), show);
    }

    template <typename T>
    void Regist() {
        auto fn = [](gecs::entity ent) {
            auto reg = nickel::ECS::Instance().World().cur_registry();
            if (reg->has<T>(ent)) {
                reg->replace<T>(ent);
            } else {
                reg->emplace<T>(ent);
            }
        };

        Regist(::mirrow::drefl::typeinfo<T>(), fn);
    }

    void Regist(TypeInfo type, SpawnFnType show) {
        methods_[type] = show;
        types_.emplace_back(type);
    }

    void Spawn(TypeInfo, gecs::entity);
    const std::vector<TypeInfo>& RegistedTypes() const;

private:
    std::map<TypeInfo, SpawnFnType> methods_;
    std::vector<TypeInfo> types_;
};

void RegistSpawnMethods();