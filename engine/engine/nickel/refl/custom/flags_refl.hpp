#pragma once

#include "nickel/common/flags.hpp"
#include "nickel/refl/drefl/factory.hpp"

namespace nickel::refl {

namespace internal {

template <typename T>
struct is_flags {
    static constexpr bool m_value = false;
};

template <typename T>
struct is_flags<Flags<T>> {
    static constexpr bool m_value = true;
};

}

template <typename T>
constexpr bool is_flags_v = internal::is_flags<T>::m_value;

class FlagsType final : public Type {
public:
    template <typename>
    friend class FlagsFactory;
    
    FlagsType(ValueKind kind, const std::string& name, default_construct_fn fn, const Type* elem_type)
        : Type{kind, name, fn}, m_elem_type{elem_type} {}

    const Type* ElemType() const {
        return m_elem_type;
    }

    const Type* m_elem_type;
};

template <typename T>
class FlagsFactory final {
public:
    static_assert(is_flags_v<T>);
    
    FlagsFactory()
        : m_type{ValueKind::Enum, "Flags", nullptr,
                 Factory<typename T::enum_type>::Info()} {}

    static FlagsFactory& Instance() noexcept {
        static FlagsFactory inst;

        static bool inited = false;
        if (!inited) {
            inited = true;
            TypeDict::Instance().Add(&inst.m_type);
            inst.m_type.m_default_constructor = [] {
                return Any{Any::AccessType::Copy, new T{},
                           &TypeOperationTraits<T>::GetOperations(),
                           &inst.m_type};
            };
        }

        return inst;
    }

    auto& Info() const noexcept {
        return m_type;
    }

private:
    FlagsType m_type;
};

template <typename T>
struct FactoryDeducer<Flags<T>> {
    using type = FlagsFactory<Flags<T>>;
};

template <typename T>
struct PropertyFactoryDeducer<Flags<T>> {
    using type = FlagsFactory<Flags<T>>;
};


}  // namespace nickel::refl