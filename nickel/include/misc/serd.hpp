#pragma once

#include "common/ecs.hpp"
#include "common/singlton.hpp"
#include "stdpch.hpp"

namespace nickel {

class SerializeMethodRegistrar final
    : public Singlton<SerializeMethodRegistrar, false> {
public:
    using SerializeFn = std::function<toml::table(const mirrow::drefl::any&)>;
    using DeserializeFn = std::function<mirrow::drefl::any(const toml::node&)>;

    struct Function {
        SerializeFn serialize;
        DeserializeFn deserialize;
    };

    void Regist(const mirrow::drefl::type* typeinfo, SerializeFn ser,
                DeserializeFn deser) {
        fns_.emplace(typeinfo, Function{ser, deser});
    }

    mirrow::drefl::any Deserialize(const mirrow::drefl::type* typeinfo,
                                   const toml::node& node) {
        if (auto it = fns_.find(typeinfo); it != fns_.end()) {
            return it->second.deserialize(node);
        } else {
            if (typeinfo->is_default_constructible()) {
                mirrow::drefl::any result = typeinfo->default_construct();
                mirrow::serd::drefl::deserialize(result, node);
                return result;
            } else {
                LOGW(log_tag::Nickel, "type ", typeinfo->name(),
                     " is not default constructible");
                return {};
            }
        }
   }

    toml::table Serialize(const mirrow::drefl::any& payload) {
        if (auto it = fns_.find(payload.type_info()); it != fns_.end()) {
            return it->second.serialize(payload);
        } else {
            return mirrow::serd::drefl::serialize_class(payload);
        }
    }

private:
    std::unordered_map<const mirrow::drefl::type*, Function> fns_;
};

void RegistSerializeMethods();

}  // namespace nickel