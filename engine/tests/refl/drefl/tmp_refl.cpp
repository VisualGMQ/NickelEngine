#include "nickel/refl/util/type_list.hpp"

#include <tuple>
#include <utility>

#include "catch2/catch_test_macros.hpp"

#include "nickel/refl/drefl/factory.hpp"

template <typename T>
class CustomTplClass {};

namespace nickel::refl {

namespace internal {

template <typename>
struct is_custom_tpl_class {
    static constexpr bool value = false;
};

template <typename T>
struct is_custom_tpl_class<CustomTplClass<T>> {
    static constexpr bool value = true;
};

}

template <typename T>
constexpr bool is_custom_tpl_class_v = internal::is_custom_tpl_class<T>::value;

struct CustomTplClassType final: public Type {
    template <typename>
    friend struct CustomTplClassFactory;
    
    CustomTplClassType(ValueKind kind, const std::string &name,
                       const default_construct_fn &fn)
        : Type(kind, name, fn) {}
};

template <typename T>
struct CustomTplClassFactory {
    static_assert(is_custom_tpl_class_v<T>);

    CustomTplClassFactory(): m_type{ValueKind::Class, "CustomTplClass", nullptr} {}

    static CustomTplClassFactory &Instance() noexcept {
        static CustomTplClassFactory inst;

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
    CustomTplClassType m_type;
};

template <typename T>
struct FactoryDeducer<CustomTplClass<T>> {
    using type = CustomTplClassFactory<CustomTplClass<T>>;
};

template <typename T>
struct PropertyFactoryDeducer<CustomTplClass<T>> {
    using type = CustomTplClassFactory<CustomTplClass<T>>;
};

}

TEST_CASE("custom template reflect") {
    auto type = nickel::refl::Factory<CustomTplClass<int>>::Info();
    REQUIRE(type->AsClass()->Name() == "CustomTplClass");
}
