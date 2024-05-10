#pragma once

#include "common/singlton.hpp"
#include "common/ecs.hpp"

namespace nickel {

toml::array SaveAsPrefab(gecs::entity, gecs::registry);
gecs::entity CreateFromPrefab(const toml::array& arr, gecs::registry reg);

class ComponentEmplaceRegistrar: public Singlton<ComponentEmplaceRegistrar, false> {
public:
    using EmplaceFn = void (*)(gecs::commands, gecs::entity,
                               mirrow::drefl::any& any);

    template <typename T>
    void RegistEmplaceFn() {
        fns_[mirrow::drefl::typeinfo<T>()] = doEmplace<T>;
    }

    void Emplace(gecs::entity, mirrow::drefl::any&);

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

void RegistComponents();

}  // namespace nickel
