#pragma once

#include "pch.hpp"
#include "core/singlton.hpp"

namespace nickel {

toml::table SaveAsPrefab(gecs::entity, gecs::registry);
gecs::entity CreateFromPrefab(const toml::table& tbl, gecs::registry reg);

class PrefabEmplaceMethods: public Singlton<PrefabEmplaceMethods, false> {
public:
    using EmplaceFn = void (*)(gecs::commands, gecs::entity,
                               mirrow::drefl::any& any);

    template <typename T>
    void RegistEmplaceFn() {
        fns_[mirrow::drefl::typeinfo<T>()] = doEmplace<T>;
    }

    EmplaceFn Find(const mirrow::drefl::type* type) const {
        if (auto it = fns_.find(type); it != fns_.end()) {
            return it->second;
        }
        return nullptr;
    }

private:
    std::unordered_map<const mirrow::drefl::type*, EmplaceFn> fns_;

    template <typename T>
    static void doEmplace(gecs::commands cmds, gecs::entity ent,
                          mirrow::drefl::any& any) {
        Assert(any.type_info() == mirrow::drefl::typeinfo<T>(),
               "incorrect type");
        cmds.emplace<T>(ent, std::move(*(T*)(any.payload())));
    }
};

}  // namespace nickel
