#include "spawn_component.hpp"

SpawnComponentMethods::spawn_fn SpawnComponentMethods::Find(type_info type) {
    if (auto it = methods_.find(type); it != methods_.end()) {
        return it->second;
    }
    return {};
}