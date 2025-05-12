#pragma once

#include "nickel/refl/drefl/qualifier.hpp"
#include "nickel/refl/drefl/type.hpp"
#include <memory>
#include <type_traits>
#include <vector>

namespace nickel::refl {

#define SET_QUALIFIER_BIT(qualif, newBit)                       \
    qualif = static_cast<Qualifier>(static_cast<long>(qualif) | \
                                    static_cast<long>(newBit))

template <typename T>
Qualifier GetQualifier() {
    Qualifier qualif = Qualifier::None;

    if constexpr (std::is_lvalue_reference_v<T>) {
        SET_QUALIFIER_BIT(qualif, Qualifier::Ref);
        if constexpr (std::is_const_v<std::remove_reference_t<T>>) {
            SET_QUALIFIER_BIT(qualif, Qualifier::Const);
        }
    }
    if constexpr (std::is_const_v<T>) {
        SET_QUALIFIER_BIT(qualif, Qualifier::Const);
    }

    return qualif;
}

#undef SET_QUALIFIER_BIT

class Property;
class Any;

class Class final : public Type {
public:
    template <typename T>
    friend class ClassFactory;

    using default_construct_fn = Any(void);

    explicit Class(const std::string& name, default_construct_fn dc)
        : Type(ValueKind::Class, name, dc) {}

    Class() : Type(ValueKind::Class, nullptr) {}

    auto& Properties() const noexcept { return m_properties; }

    void SetValue(Any& from, Any& to);
    void StealValue(Any& from, Any& to);

private:
    std::vector<std::shared_ptr<Property>> m_properties;
};

}  // namespace nickel::refl
