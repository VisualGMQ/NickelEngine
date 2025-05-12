#include "nickel/refl/drefl/factory.hpp"
#include "nickel/refl/drefl/class_visitor.hpp"
#include "nickel/refl/drefl/value_kind.hpp"
#include "nickel/refl/internal/serd_backends/tomlplusplus.hpp"

#include "toml++/toml.hpp"

#include <string_view>

namespace nickel::refl {

double serialize_numeric(const Any& value) {
    auto& numeric = *value.TypeInfo()->AsNumeric();
    return numeric.GetValue(value);
}

bool serialize_boolean(const Any& value) {
    return value.TypeInfo()->AsBoolean()->GetValue(value);
}

std::string_view serialize_string(const Any& value) {
    return value.TypeInfo()->AsString()->GetStrView(value);
}

long serialize_enum(const Any& value) {
    return value.TypeInfo()->AsEnum()->GetValue(value);
}

void serialize_optional(const Any& value, std::string_view name,
                        toml::node& node);

toml::array serialize_array(const Any& value);

class serialize_class_visitor : public ClassVisitor {
public:
    serialize_class_visitor(toml::table& tbl, const Any& value)
        : tbl_(tbl), value_(value) {}

    void operator()(NumericProperty& prop) {
        tbl_.emplace(prop.Name(), serialize_numeric(prop.CallConst(value_)));
    }

    void operator()(EnumProperty& prop) {
        tbl_.emplace(prop.Name(), serialize_enum(prop.CallConst(value_)));
    }

    void operator()(ClassProperty& prop) {
        tbl_.emplace(prop.Name(), serialize_class(prop.CallConst(value_)));
    }

    void operator()(StringProperty& prop) {
        tbl_.emplace(prop.Name(), serialize_string(prop.CallConst(value_)));
    }

    void operator()(BooleanProperty& prop) {
        tbl_.emplace(prop.Name(), serialize_boolean(prop.CallConst(value_)));
    }

    void operator()(PointerProperty& prop) {
        LOGE("pointer property can't be serialize");
    }

    void operator()(ArrayProperty& prop) {
        tbl_.emplace(prop.Name(), serialize_array(prop.CallConst(value_)));
    }

    void operator()(OptionalProperty& prop) {
        auto value = prop.CallConst(value_);
        if (value.TypeInfo()->AsOptional()->HasValue(value)) {
            serialize_optional(value, prop.Name(), tbl_);
        }
    }

private:
    toml::table& tbl_;
    const Any& value_;
};

toml::table serialize_class(const Any& value) {
    toml::table tbl;

    if (auto f = serialize_method_storage::instance().get_serialize(
            value.TypeInfo());
        f) {
        f(tbl, value);
        return tbl;
    }

    auto& clazz = *value.TypeInfo()->AsClass();
    serialize_class_visitor visitor{tbl, value};
    for (auto& prop : clazz.Properties()) {
        prop->Visit(&visitor);
    }

    return tbl;
}

toml::array serialize_array(const Any& value) {
    // IMPROVE: use iterator to improve effect when value is std::list
    toml::array arr;

    auto arr_type = value.TypeInfo()->AsArray();
    for (int i = 0; i < arr_type->Size(value); i++) {
        auto elem = value.TypeInfo()->AsArray()->GetConst(i, value);
        auto type = elem.TypeInfo();
        switch (type->Kind()) {
            case ValueKind::None:
                LOGE("unknown type, can't serialize");
                break;
            case ValueKind::Array:
                arr.push_back(serialize_array(elem));
                break;
            case ValueKind::Boolean:
                arr.push_back(serialize_boolean(elem));
                break;
            case ValueKind::Numeric:
                arr.push_back(serialize_numeric(elem));
                break;
            case ValueKind::String:
                arr.push_back(serialize_string(elem));
                break;
            case ValueKind::Enum:
                arr.push_back(serialize_enum(elem));
                break;
            case ValueKind::Class:
                arr.push_back(serialize_class(elem));
                break;
            case ValueKind::Optional:
                serialize_optional(elem, "", arr);
                break;
            case ValueKind::Property:
            case ValueKind::Pointer:
                LOGE("can't serialize raw property/pointer");
                break;
        }
    }

    return arr;
}

void do_serialize(const Any& value, toml::table& tbl, std::string_view name) {
    auto serialize_method =
        serialize_method_storage::instance().get_serialize(value.TypeInfo());
    if (serialize_method) {
        serialize_method(tbl, value);
        return;
    }

    switch (value.TypeInfo()->Kind()) {
        case ValueKind::None:
            LOGE("unknown type, can't serialize");
            break;
        case ValueKind::Boolean:
            tbl.emplace(name, serialize_boolean(value));
            break;
        case ValueKind::Numeric:
            tbl.emplace(name, serialize_numeric(value));
            break;
        case ValueKind::String:
            tbl.emplace(name, serialize_string(value));
            break;
        case ValueKind::Enum:
            tbl.emplace(name, serialize_enum(value));
            break;
        case ValueKind::Class:
            tbl.emplace(name, serialize_class(value));
            break;
        case ValueKind::Array:
            tbl.emplace(name, serialize_array(value));
            break;
        case ValueKind::Optional:
            serialize_optional(value, name, tbl);
            break;
        case ValueKind::Property:
            LOGE("can't serialize property directly");
            break;
        case ValueKind::Pointer:
            LOGE("can't serialize pointer");
            break;
    }
}

void serialize(toml::table& tbl, const Any& value, std::string_view name) {
    auto type = value.TypeInfo();
    if (type->Kind() == ValueKind::Pointer ||
        type->Kind() == ValueKind::Property) {
        LOGE(
            "How can I serialize a pointer or property? I can't do this!");
    }

    do_serialize(value, tbl, name);
}

void serialize_optional(const Any& value, std::string_view name,
                        toml::node& node) {
    auto optional_type = value.TypeInfo()->AsOptional();

    if (!optional_type->HasValue(value)) {
        return;
    }
    if (node.is_table()) {
        serialize(*node.as_table(), optional_type->GetValueConst(value),
                  name);
    } else if (node.is_array()) {
        auto elem = optional_type->GetValueConst(value);
        auto arr = *node.as_array();
        switch (elem.TypeInfo()->Kind()) {
            case ValueKind::None:
                LOGE("can't serialize unknown value");
                break;
            case ValueKind::Boolean:
                arr.push_back(serialize_boolean(elem));
                break;
            case ValueKind::Numeric:
                arr.push_back(serialize_numeric(elem));
                break;
            case ValueKind::String:
                arr.push_back(serialize_string(elem));
                break;
            case ValueKind::Enum:
                arr.push_back(serialize_enum(elem));
                break;
            case ValueKind::Class:
                arr.push_back(serialize_class(elem));
                break;
            case ValueKind::Array:
                arr.push_back(serialize_array(elem));
                break;
            case ValueKind::Property:
            case ValueKind::Pointer:
            case ValueKind::Optional:
                LOGE(
                    "can't serialize property/pointer/optional<optional<>>");
                break;
        }
    }
}

}  // namespace nickel::refl