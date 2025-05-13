#pragma once

#include "nickel/refl/drefl/type.hpp"
#include "nickel/refl/drefl/any.hpp"
#include <string>
#include <vector>

namespace nickel::refl {

class EnumInfo;

class EnumItem final {
public:
    using enum_numeric_type = long;

    template <typename T>
    EnumItem(const std::string& name, T value,
              const class EnumInfo* enumerate)
        : m_enum_info(enumerate),
          m_name(name),
          m_value(static_cast<enum_numeric_type>(value)) {}

    auto& Name() const noexcept { return m_name; }

    long Value() const noexcept { return m_value; }

    const class EnumInfo* EnumInfo() const noexcept { return m_enum_info; }

private:
    const class EnumInfo* m_enum_info;
    std::string m_name;
    enum_numeric_type m_value;
};

class EnumInfo : public Type {
public:
    template <typename>
    friend class EnumFactory;

    using enum_numeric_type = typename EnumItem::enum_numeric_type;

    explicit EnumInfo() : Type(ValueKind::Enum, nullptr) {}

    explicit EnumInfo(const std::string& name, default_construct_fn fn)
        : Type(ValueKind::Enum, name, fn) {}

    long GetValue(const Any& elem) const { return *(long*)(elem.Payload()); }

    void SetValue(Any& elem, enum_numeric_type value) const {
        if (!SET_VALUE_CHECK(elem, ValueKind::Enum)) {
            LOGE("can't set enum value to any");
            return;
        }

        *(long*)(elem.Payload()) = value;
    }

    auto& Enums() const noexcept { return m_items; }

private:
    std::vector<EnumItem> m_items;

    template <typename T>
    void add(const std::string& name, T value) {
        m_items.emplace_back(name, value, this);
    }
};

}  // namespace nickel::refl