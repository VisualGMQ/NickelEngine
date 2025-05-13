#pragma once

#include "nickel/refl/drefl/type.hpp"
#include "nickel/refl/drefl/value_kind.hpp"
#include "nickel/refl/util/misc.hpp"

namespace nickel::refl {

struct Type;

class Pointer final: public Type {
public:
    template <typename T>
    static Pointer Create(const Type* pure_type) {
        static_assert(std::is_pointer_v<T>);
        std::string pointer_name = pure_type->Name();
        int layer = pointer_layer_v<T>;
        for (int i = 0; i < layer; i++) {
            pointer_name += "*";
        }
        return {pointer_name,
                std::is_const_v<T>,
                std::is_const_v<remove_all_pointers_t<remove_cvref_t<T>>>,
                pure_type,
                layer};
    }

    bool IsConst() const noexcept { return m_is_const; }
    bool IsPointTypeConst() const noexcept { return m_is_point_type_const; }

    const Type* TypeInfo() const noexcept { return m_typeinfo; }

    int Layers() const noexcept { return m_layers; }

private:
    Pointer(): Type(ValueKind::Pointer, "", nullptr) {}

    Pointer(const std::string& name, bool is_const, bool is_point_type_const,
            const Type* typeinfo, int layers)
        : Type(ValueKind::Pointer, name, nullptr),
          m_is_const(is_const),
          m_is_point_type_const(is_point_type_const),
          m_typeinfo(typeinfo),
          m_layers(layers) {}

    bool m_is_const = false;  // means `int* const`(not `const int*`)
    bool m_is_point_type_const = false;   // means `const int*`(not `int* const`)
    const Type* m_typeinfo;
    int m_layers = 0;  // pointer layer count: int** == 2, int* == 1
};

}  // namespace nickel::refl
