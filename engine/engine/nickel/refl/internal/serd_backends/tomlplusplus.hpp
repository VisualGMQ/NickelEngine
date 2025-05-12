#pragma once

#include "nickel/refl/drefl/factory.hpp"
#include "nickel/refl/drefl/any.hpp"
#define TOML_EXCEPTIONS 0
#include "toml++/toml.hpp"

#include <functional>
#include <unordered_map>
#include <utility>

namespace nickel::refl {

using namespace ::nickel::refl;

class serialize_method_storage {
public:
    using serialize_fn = std::function<void(toml::node&, const Any&)>;
    using deserialize_fn = std::function<void(const toml::node&, Any&)>;

    struct serd_methods {
        serialize_fn serialize = nullptr;
        deserialize_fn deserialize = nullptr;
    };

    static auto& instance() {
        static serialize_method_storage instance;
        return instance;
    }

    void regist_serialize(const Type* type, const serialize_fn& f) {
        if (auto it = methods_.find(type); it != methods_.end()) {
            it->second.serialize = f;
        } else {
            methods_.insert_or_assign(type, serd_methods{f, nullptr});
        }
    }

    void regist_deserialize(const Type* type, const deserialize_fn& f) {
        if (auto it = methods_.find(type); it != methods_.end()) {
            it->second.deserialize = f;
        } else {
            methods_.insert_or_assign(type, serd_methods{nullptr, f});
        }
    }

    serialize_fn get_serialize(const Type* type) {
        if (auto it = methods_.find(type); it != methods_.end()) {
            return it->second.serialize;
        }
        return nullptr;
    }

    deserialize_fn get_deserialize(const Type* type) {
        if (auto it = methods_.find(type); it != methods_.end()) {
            return it->second.deserialize;
        }
        return nullptr;
    }

private:
    std::unordered_map<const Type*, serd_methods> methods_;
};

void serialize(toml::table& tbl, const Any& value, std::string_view name);
toml::table serialize_class(const Any& value);
void deserialize(Any& obj, const toml::node& node);

}  // namespace nickel::refl
