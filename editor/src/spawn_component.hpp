#pragma once

#include "nickel.hpp"

class SpawnComponentMethods final : public nickel::Singlton<SpawnComponentMethods, false> {
public:
    using spawn_fn = void(*)(gecs::commands, gecs::entity, gecs::registry);
    using type_info = ::mirrow::drefl::type_info;

    template <typename T>
    void Regist(spawn_fn show) {
        Regist(::mirrow::drefl::reflected_type<T>(), show);
    }

    void Regist(type_info type, spawn_fn show) {
        methods_[type] = show;
    }

    auto& Methods() const { return methods_; }

    spawn_fn Find(type_info type);

private:
    struct TypeInfoLess {
        bool operator()(const ::mirrow::drefl::type_info& t1, const ::mirrow::drefl::type_info& t2) const {
            return t1.type_node() < t2.type_node();
        }
    };

    std::map<type_info, spawn_fn, TypeInfoLess> methods_;
};
