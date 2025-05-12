#include "nickel/common/assert.hpp"
#include "nickel/refl/drefl/factory.hpp"
#include "nickel/refl/drefl/array.hpp"
#include "nickel/refl/drefl/class_visitor.hpp"
#include "nickel/refl/drefl/make_any.hpp"
#include "nickel/refl/drefl/type.hpp"
#include "nickel/refl/drefl/value_kind.hpp"
#include "nickel/refl/internal/serd_backends/tomlplusplus.hpp"

#include "toml++/toml.hpp"

namespace nickel::refl {

void deserialize_numeric(Any& obj, const toml::node& node) {
    NICKEL_ASSERT(node.is_number(),
                  "can't deserialize non-number node to numeric type");

    double value = 0;
    if (node.is_integer()) {
        value = node.as_integer()->get();
    } else {
        value = node.as_floating_point()->get();
    }
    obj.TypeInfo()->AsNumeric()->SetValue(obj, value);
}

void deserialize_boolean(Any& obj, const toml::node& node) {
    NICKEL_ASSERT(node.is_boolean(),
                  "can't deserialize non-bool node to boolean type");

    obj.TypeInfo()->AsBoolean()->SetValue(obj, node.as_boolean()->get());
}

void deserialize_string(Any& obj, const toml::node& node) {
    NICKEL_ASSERT(node.is_string(),
                  "can't deserialize non-bool node to boolean type");

    obj.TypeInfo()->AsString()->SetValue(obj, node.as_string()->get());
}

void deserialize_enum(Any& obj, const toml::node& node) {
    NICKEL_ASSERT(node.is_integer(),
                  "can't deserialize non-integer node to enum type");

    obj.TypeInfo()->AsEnum()->SetValue(obj, node.as_integer()->get());
}

void deserialize_optional(Any& obj, const toml::node& node);

void deserialize_class(Any& obj, const toml::node& node);

void deserialize_array(Any& obj, const toml::node& node);

class class_deserialize_visitor : public ClassVisitor {
public:
    class_deserialize_visitor(Any& obj, const toml::table& tbl)
        : obj_(obj), tbl_(tbl) {}

    void operator()(NumericProperty& prop) {
        if (tbl_.contains(prop.Name())) {
            auto value = prop.Call(obj_);
            deserialize_numeric(value, *tbl_.get(prop.Name()));
        } else {
            LOGE("property " + prop.Name() + " don't exists, ignore");
        }
    }

    void operator()(EnumProperty& prop) {
        if (tbl_.contains(prop.Name())) {
            auto value = prop.Call(obj_);
            deserialize_enum(value, *tbl_.get(prop.Name()));
        } else {
            LOGE("property " + prop.Name() + " don't exists, ignore");
        }
    }

    void operator()(ClassProperty& prop) {
        if (tbl_.contains(prop.Name())) {
            auto value = prop.Call(obj_);
            deserialize_class(value, *tbl_.get(prop.Name()));
        } else {
            LOGE("property " + prop.Name() + " don't exists, ignore");
        }
    }

    void operator()(StringProperty& prop) {
        if (tbl_.contains(prop.Name())) {
            auto value = prop.Call(obj_);
            deserialize_string(value, *tbl_.get(prop.Name()));
        } else {
            LOGE("property " + prop.Name() + " don't exists, ignore");
        }
    }

    void operator()(BooleanProperty& prop) {
        if (tbl_.contains(prop.Name())) {
            auto value = prop.Call(obj_);
            deserialize_boolean(value, *tbl_.get(prop.Name()));
        } else {
            LOGE("property " + prop.Name() + " don't exists, ignore");
        }
    }

    void operator()(PointerProperty& prop) {
        LOGE("can't deserialize pointer property " + prop.Name());
    }

    void operator()(OptionalProperty& prop) {
        if (tbl_.contains(prop.Name())) {
            auto value = prop.Call(obj_);
            deserialize(value, *tbl_.get(prop.Name()));
        }
    }

    void operator()(ArrayProperty& prop) {
        if (tbl_.contains(prop.Name())) {
            auto node = tbl_.get(prop.Name());
            if (!node->is_array()) {
                LOGE("can't deserialize non-array node to array");
                return;
            }

            auto& arr_node = *node->as_array();
            auto elem_type = prop.TypeInfo()->AsArray()->ElemType();
            size_t count = arr_node.size();
            auto arr = prop.Call(obj_);
            auto arr_type = arr.TypeInfo()->AsArray();
            if (arr_type->ArrayType() == Array::ArrayType::Static) {
                count = std::min(count, arr_type->Size(arr));
            }
            for (size_t i = 0; i < count; i++) {
                auto& node = arr_node[i];
                nickel::refl::Any elem;
                bool type_satisfied = true;
                switch (elem_type->Kind()) {
                    case refl::ValueKind::None:
                        LOGE("can't deserialize unknown type");
                    case refl::ValueKind::Boolean:
                        if (!node.is_boolean()) {
                            type_satisfied = false;
                            break;
                        }
                        elem = AnyMakeConstRef(node.as_boolean()->get());
                        break;
                    case refl::ValueKind::Numeric: {
                        if (!node.is_number()) {
                            type_satisfied = false;
                            break;
                        }
                        auto numeric = elem_type->AsNumeric();
                        elem = numeric->DefaultConstruct();
                        double num = 0;
                        if (node.is_integer()) {
                            num = node.as_integer()->get();
                        } else {
                            num = node.as_floating_point()->get();
                        }
                        numeric->SetValue(elem, num);
                    } break;
                    case refl::ValueKind::String:
                        if (!node.is_string()) {
                            type_satisfied = false;
                            break;
                        }
                        elem = AnyMakeConstRef(node.as_string()->get());
                        break;
                    case refl::ValueKind::Enum:
                        if (!node.is_integer()) {
                            type_satisfied = false;
                            break;
                        }
                        elem = AnyMakeConstRef(node.as_integer()->get());
                        break;
                    case refl::ValueKind::Class:
                        elem = elem_type->AsClass()->DefaultConstruct();
                        deserialize_class(elem, node);
                        break;
                    case refl::ValueKind::Array:
                        // TODO: add support
                    case refl::ValueKind::Optional:
                    case refl::ValueKind::Property:
                    case refl::ValueKind::Pointer:
                        type_satisfied = false;
                        LOGE(
                            "can't deserialize property/pointer/optional");
                        continue;
                }

                if (!type_satisfied) {
                    LOGE("type not satisfied");
                    continue;
                }

                if (arr_type->ArrayType() == Array::ArrayType::Static) {
                    arr_type->Get(i, arr).StealAssign(std::move(elem));
                } else {
                    arr_type->PushBack(elem, arr);
                }
            }
        }
    }

private:
    Any& obj_;
    const toml::table& tbl_;
};

void deserialize_class(Any& obj, const toml::node& node) {
    NICKEL_ASSERT(node.is_table(),
                  "can't deserialize non-table node to class type");

    auto deserd =
        serialize_method_storage::instance().get_deserialize(obj.TypeInfo());

    if (deserd) {
        deserd(node, obj);
    } else {
        auto& tbl = *node.as_table();
        auto& clazz = *obj.TypeInfo()->AsClass();

        class_deserialize_visitor visitor{obj, tbl};

        for (auto& prop : clazz.Properties()) {
            prop->Visit(&visitor);
        }
    }
}

void deserialize_array(Any& obj, const toml::node& node) {
    NICKEL_ASSERT(node.is_array(),
                  "can't deserialize non-array node to array type");

    auto& arr = *node.as_array();

    size_t size = arr.size();
    auto arr_type = obj.TypeInfo()->AsArray();
    if (arr_type->ArrayType() == Array::ArrayType::Static) {
        size = std::min(arr_type->Size(obj), arr.size());
    }

    for (int i = 0; i < size; i++) {
        auto node = arr.get(i);
        nickel::refl::Any elem;
        switch (arr_type->ElemType()->Kind()) {
            case refl::ValueKind::None:
                LOGE("unknown type, can't deserialize");
                break;
            case refl::ValueKind::Boolean:
                deserialize_boolean(elem, *node);
                arr_type->PushBack(elem, obj);
                break;
            case refl::ValueKind::Numeric:
                deserialize_numeric(elem, *node);
                arr_type->PushBack(elem, obj);
                break;
            case refl::ValueKind::String:
                deserialize_string(elem, *node);
                arr_type->PushBack(elem, obj);
                break;
            case refl::ValueKind::Enum:
                deserialize_enum(elem, *node);
                arr_type->PushBack(elem, obj);
                break;
            case refl::ValueKind::Class:
                deserialize_class(elem, *node);
                arr_type->PushBack(elem, obj);
                break;
            case refl::ValueKind::Array:
                deserialize_array(elem, *node);
                arr_type->PushBack(elem, obj);
                break;
            case refl::ValueKind::Optional:
                deserialize_optional(elem, *node);
                arr_type->PushBack(elem, obj);
                break;
            case refl::ValueKind::Property:
            case refl::ValueKind::Pointer:
                LOGE("can't deserialize property/pointer");
                break;
        }
    }
}

void do_deserialize(Any& obj, const toml::node& node) {
    if (auto func = serialize_method_storage::instance().get_deserialize(
            obj.TypeInfo());
        func) {
        func(node, obj);
        return;
    }

    switch (obj.TypeInfo()->Kind()) {
        case refl::ValueKind::Boolean:
            deserialize_boolean(obj, node);
            break;
        case refl::ValueKind::Numeric:
            deserialize_numeric(obj, node);
            break;
        case refl::ValueKind::String:
            deserialize_string(obj, node);
            break;
        case refl::ValueKind::Enum:
            deserialize_enum(obj, node);
            break;
        case refl::ValueKind::Class:
            deserialize_class(obj, *node.as_table());
            break;
        case refl::ValueKind::Array:
            deserialize_array(obj, node);
            break;
        case refl::ValueKind::Optional:
            deserialize_optional(obj, node);
            break;
        default:;
    }
}

void deserialize(Any& obj, const toml::node& node) {
    auto typeinfo = obj.TypeInfo();
    switch (typeinfo->Kind()) {
        case refl::ValueKind::None:
            LOGE("unknown type");
            break;
        case refl::ValueKind::Boolean:
        case refl::ValueKind::Numeric:
        case refl::ValueKind::String:
        case refl::ValueKind::Enum:
        case refl::ValueKind::Class:
        case refl::ValueKind::Array:
        case refl::ValueKind::Optional:
            do_deserialize(obj, node);
            break;
        case refl::ValueKind::Property:
        case refl::ValueKind::Pointer:
            LOGE("don't known how to deserialize property/pointer");
            break;
    }
}

void deserialize_optional(Any& obj, const toml::node& node) {
    auto optional_type = obj.TypeInfo()->AsOptional();

    auto innerType = optional_type->ElemType();
    auto value = innerType->DefaultConstruct();

    switch (innerType->Kind()) {
        case refl::ValueKind::None:
            break;
        case refl::ValueKind::Boolean:
            NICKEL_ASSERT(node.is_boolean(),
                          "can't deserialize non-integer node to enum type");
            deserialize_boolean(value, node);
            break;
        case refl::ValueKind::Numeric:
            NICKEL_ASSERT(node.is_number(),
                          "can't deserialize non-integer node to enum type");
            deserialize_numeric(value, node);
            break;
        case refl::ValueKind::String:
            NICKEL_ASSERT(node.is_string(),
                          "can't deserialize non-integer node to enum type");
            deserialize_string(value, node);
            break;
        case refl::ValueKind::Enum:
            NICKEL_ASSERT(node.is_integer(),
                          "can't deserialize non-integer node to enum type");
            deserialize_enum(value, node);
            break;
        case refl::ValueKind::Class:
            NICKEL_ASSERT(node.is_table(),
                          "can't deserialize non-integer node to enum type");
            deserialize_class(value, node);
            break;
        case refl::ValueKind::Array:
            NICKEL_ASSERT(node.is_array(),
                          "can't deserialize non-integer node to enum type");
            deserialize_array(value, node);
            break;
        case refl::ValueKind::Property:
        case refl::ValueKind::Pointer:
        case refl::ValueKind::Optional:
            LOGE("can't deserialize property/pointer/optional<optional>");
            break;
    }

    if (value.HasValue()) {
        optional_type->SetInnerValue(value, obj);
    }
}

}  // namespace nickel::refl