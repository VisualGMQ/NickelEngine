#pragma once

#include "nickel.hpp"

class SpawnComponentMethods final : public nickel::Singlton<SpawnComponentMethods, false> {
public:
    using spawn_fn = void(*)(gecs::commands, gecs::entity, gecs::registry);
    using type_info = const ::mirrow::drefl::type*;

    template <typename T>
    void Regist(spawn_fn show) {
        Regist(::mirrow::drefl::typeinfo<T>(), show);
    }

    void Regist(type_info type, spawn_fn show) {
        methods_[type] = show;
    }

    auto& Methods() const { return methods_; }

    spawn_fn Find(type_info type);

private:
    std::map<type_info, spawn_fn> methods_;
};
