#include "misc/prefab.hpp"
#include "core/log_tag.hpp"
#include "mirrow/drefl/any.hpp"
#include "mirrow/drefl/drefl.hpp"
#include "mirrow/serd/dynamic/backends/tomlplusplus.hpp"
#include "misc/hierarchy.hpp"

namespace nickel {

toml::table saveAsPrefabNoHierarchy(gecs::entity entity, gecs::registry reg) {
    if (!reg.alive(entity)) {
        LOGW(log_tag::Nickel, "entity ", entity, " already dead");
        return {};
    }

    toml::table tbl;

    for (auto typeinfo : reg.typeinfos()) {
        // hierarchy must treat specially
        if (typeinfo.type_info == mirrow::drefl::typeinfo<Parent>() ||
            typeinfo.type_info == mirrow::drefl::typeinfo<Child>()) {
            continue;
        }

        toml::table componentTbl;

        if (reg.has(entity, typeinfo.type_info)) {
            auto component = reg.get_mut(entity, typeinfo.type_info);
            mirrow::serd::drefl::serialize(tbl, component,
                                           typeinfo.type_info->name());
            tbl.emplace(typeinfo.type_info->name(), std::move(componentTbl));
        }
    }

    return tbl;
}

toml::array SaveAsPrefab(gecs::entity entity, gecs::registry reg) {
    toml::array nodeArr;

    int index = 0;

    std::unordered_map<gecs::entity, int> entityIndexMap;

    HierarchyTool tool{entity};
    tool.PreorderVisit([&](gecs::entity ent, gecs::registry reg) {
        auto newTbl = saveAsPrefabNoHierarchy(ent, reg);
        if (!newTbl.empty()) {
            nodeArr.push_back(newTbl);
            entityIndexMap[ent] = index++;
        }
    });

    if (tool.HasHierarchy()) {
        tool.PreorderVisit([&](gecs::entity ent, gecs::registry reg) {
            auto& tbl = *nodeArr[entityIndexMap[ent]].as_table();
            if (reg.has<Child>(ent)) {
                auto& children = reg.get<Child>(ent).entities;
                toml::array childArr;
                for (auto child : children) {
                    childArr.push_back(entityIndexMap[child]);
                }
                tbl.emplace(mirrow::drefl::typeinfo<Child>()->name(), childArr);
            }
        });
    }

    return nodeArr;
}

gecs::entity createFromPrefabNoHierarchy(const toml::table& tbl,
                                         gecs::registry reg) {
    auto cmd = reg.commands();
    gecs::entity ent = cmd.create();

    for (auto [key, value] : tbl) {
        // skip hierarchy
        if (key == mirrow::drefl::typeinfo<Child>()->name() ||
            key == mirrow::drefl::typeinfo<Parent>()->name()) {
            continue;
        }

        auto typeinfo = mirrow::drefl::typeinfo(key);
        mirrow::drefl::any component;
        auto class_info = typeinfo->as_class();
        if (class_info->is_default_constructible()) {
            component = class_info->default_construct();
        }

        if (!component.has_value()) {
            LOGW(log_tag::Nickel, "read component ", key,
                 " from prefab failed");
            continue;
        }

        mirrow::serd::drefl::deserialize(component, *tbl[key].as_table());

        if (auto fn =
                PrefabEmplaceMethods::Instance().Find(component.type_info());
            fn) {
            fn(reg.commands(), ent, component);
        }
    }

    return ent;
}

gecs::entity CreateFromPrefab(const toml::array& arr, gecs::registry reg) {
    Assert(arr.is_array_of_tables(), "arr don't contain table");

    auto cmd = reg.commands();
    gecs::entity ent = cmd.create();

    std::vector<gecs::entity> entities(arr.size());

    // create from prefab
    for (int i = 0; i < arr.size(); i++) {
        auto tbl = arr.get(i);
        auto ent = createFromPrefabNoHierarchy(*tbl->as_table(), reg);
        entities[i] = ent;
    }

    // set hierarchy
    for (int i = 0; i < arr.size(); i++) {
        auto tbl = arr.get(i)->as_table();
        if (auto childNode = tbl->get(mirrow::drefl::typeinfo<Child>()->name());
            childNode && childNode->is_array()) {
            Assert(childNode->is_array(), "child is not array");
            auto& childArr = *childNode->as_array();
            HierarchyTool tool{entities[i]};
            for (int j = 0; j < childArr.size(); j++) {
                auto childEnt = entities[childArr[j].as_integer()->get()];
                tool.MoveEntityAsChild(childEnt);
            }
        }
    }

    return ent;
}

}  // namespace nickel