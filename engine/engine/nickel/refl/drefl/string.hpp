#pragma once

#include "nickel/refl/drefl/type.hpp"
#include "nickel/refl/drefl/value_kind.hpp"

namespace nickel::refl {

class Any;

class String : public Type {
public:
    enum class StringKind {
        Unknown,
        // ConstCharList
        String,
        StringView,
    };

    template <typename T>
    static String Create() {
        return {getKind<T>(), getName<T>()};
    }

    auto StringKind() const noexcept { return m_kind; }

    bool IsString() const noexcept { return m_kind == StringKind::String; }

    bool IsStringView() const noexcept {
        return m_kind == StringKind::StringView;
    }

    std::string GetStr(const Any&) const;
    std::string_view GetStrView(const Any&) const;

    void SetValue(Any&, const std::string&) const;
    void SetValue(Any&, std::string_view&) const;

private:
    enum StringKind m_kind;

    String(enum StringKind skind, const std::string& name);

    template <typename T>
    static auto getKind() {
        if constexpr (std::is_same_v<std::string, T>) {
            return StringKind::String;
        }
        if constexpr (std::is_same_v<std::string_view, T>) {
            return StringKind::StringView;
        }
        return StringKind::Unknown;
    }

    template <typename T>
    static std::string getName() {
        if constexpr (std::is_same_v<std::string, T>) {
            return "std::string";
        }
        if constexpr (std::is_same_v<std::string_view, T>) {
            return "std::string_view";
        }

        return "unknown-string-type";
    }
};

}  // namespace nickel::refl