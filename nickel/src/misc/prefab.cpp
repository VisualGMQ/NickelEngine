#include "misc/prefab.hpp"
#include "core/log_tag.hpp"
#include "mirrow/drefl/any.hpp"
#include "mirrow/drefl/drefl.hpp"
#include "mirrow/serd/dynamic/backends/tomlplusplus.hpp"

namespace nickel {

toml::table SaveAsPrefab(gecs::entity entity, gecs::registry reg) {
    if (!reg.alive(entity)) {
        LOGW(log_tag::Nickel, "entity ", entity, " already dead");
        return {};
    }

    toml::table tbl;

    for (auto typeinfo : reg.typeinfos()) {
        toml::table componentTbl;

        if (reg.has(entity, typeinfo.type_info)) {
            auto component = reg.get_mut(entity, typeinfo.type_info);
            mirrow::serd::drefl::serialize(tbl, component, typeinfo.type_info->name());
            tbl.emplace(typeinfo.type_info->name(), std::move(componentTbl));
        }
    }

    return tbl;
}

gecs::entity CreateFromPrefab(const toml::table& tbl, gecs::registry reg) {
    auto cmd = reg.commands();
    gecs::entity ent = cmd.create();

    for (auto [key, value]: tbl) {
        if (key == "id") {
            continue;
        }

        auto typeinfo = mirrow::drefl::typeinfo(key);
        mirrow::drefl::any component;
        auto class_info = typeinfo->as_class();
        if (class_info->is_default_constructbile()) {
            component = class_info->default_construct();
        }

        if (!component.has_value()) {
            LOGW(log_tag::Nickel, "read component ", key, " from prefab failed");
            continue;
        }

        mirrow::serd::drefl::deserialize(component, *tbl[key].as_table());

        if (auto fn = PrefabEmplaceMethods::Instance().Find(component.type_info()); fn) {
            fn(reg.commands(), ent, component);
        }
    }

    return ent;
}

}