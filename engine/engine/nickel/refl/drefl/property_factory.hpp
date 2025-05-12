#pragma once

#include "nickel/refl/drefl/factory.hpp"
#include "nickel/refl/drefl/class.hpp"
#include "nickel/refl/drefl/property.hpp"
#include "nickel/refl/util/function_traits.hpp"
#include "nickel/refl/util/misc.hpp"
#include "nickel/refl/util/variable_traits.hpp"
#include <memory>
#include <type_traits>

namespace nickel::refl {

template <typename>
class ClassFactory;

class EnumPropertyFactory final {
public:
    template <typename T>
    EnumPropertyFactory(const std::string& name, T accessor) {
        using traits = variable_traits<T>;
        using var_type = typename traits::type;
        using enum_type = remove_cvref_t<var_type>;

        static_assert(std::is_enum_v<enum_type>);

        auto& enum_info = EnumFactory<enum_type>::instance().info();

        property_ = std::make_shared<EnumPropertyImpl<T>>(
            name, GetQualifier<var_type>(), enum_info, accessor);
    }

    auto& Get() const noexcept { return property_; }

private:
    std::shared_ptr<EnumProperty> property_;
};

class NumericPropertyFactory final {
public:
    template <typename T>
    NumericPropertyFactory(const std::string& name, T accessor) {
        using traits = variable_traits<T>;
        using var_type = typename traits::type;

        property_ = std::make_shared<NumericPropertyImpl<T>>(
            name, &ClassFactory<typename traits::clazz>::instance().info(),
            GetQualifier<var_type>(), accessor);
    }

    auto& Get() const noexcept { return property_; }

private:
    std::shared_ptr<NumericProperty> property_;
};

class StringPropertyFactory final {
public:
    template <typename T>
    StringPropertyFactory(const std::string& name, T accessor) {
        using traits = variable_traits<T>;
        using var_type = typename traits::type;

        property_ = std::make_shared<StringPropertyImpl<T>>(
            name, &ClassFactory<typename traits::clazz>::instance().info(),
            GetQualifier<var_type>(), accessor);
    }

    auto& get() const noexcept { return property_; }

private:
    std::shared_ptr<StringProperty> property_;
};

class BooleanPropertyFactory final {
public:
    template <typename T>
    BooleanPropertyFactory(const std::string& name, T accessor) {
        using traits = variable_traits<T>;
        using var_type = typename traits::type;

        property_ = std::make_shared<BooleanPropertyImpl<T>>(
            name, &ClassFactory<typename traits::clazz>::instance().info(),
            GetQualifier<var_type>(), accessor);
    }

    auto& get() const noexcept { return property_; }

private:
    std::shared_ptr<BooleanProperty> property_;
};

class ClassPropertyFactory final {
public:
    template <typename T>
    ClassPropertyFactory(const std::string& name, T accessor) {
        using traits = variable_traits<T>;
        using var_type = typename traits::type;

        property_ = std::make_shared<ClassPropertyImpl<T>>(name, accessor);
    }

    auto& Get() const { return property_; }

private:
    std::shared_ptr<ClassProperty> property_;
};

class PropertyFactory final {
public:
    template <typename T>
    std::shared_ptr<Property> Create(const std::string& name, T accessor) {
        if constexpr (is_function_v<T>) {
            // TODO: use function_factory here
            return nullptr;
        } else {
            using traits = variable_traits<T>;
            using type = remove_cvref_t<typename traits::type>;

            if constexpr (std::is_same_v<bool, type>) {
                return BooleanPropertyFactory{name, accessor}.get();
            } else if constexpr (std::is_enum_v<type>) {
                return EnumPropertyFactory{name, accessor}.get();
            } else if constexpr (std::is_fundamental_v<type>) {
                return NumericPropertyFactory{name, accessor}.get();
            } else if constexpr (std::is_same_v<std::string, type> ||
                                 std::is_same_v<std::string_view, type>) {
                return StringPropertyFactory{name, accessor}.get();
            } else {
                return ClassPropertyFactory{name, accessor}.get();
            }
        }
    }
};

}  // namespace nickel::refl