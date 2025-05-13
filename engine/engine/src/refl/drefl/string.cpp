#include "nickel/refl/drefl/string.hpp"
#include "nickel/refl/drefl/any.hpp"
#include "nickel/refl/drefl/cast_any.hpp"
#include "nickel/refl/drefl/make_any.hpp"


namespace nickel::refl {

String::String(enum StringKind skind, const std::string& name)
    : Type(ValueKind::String, name,
           [=]() {
               if (skind == StringKind::String) {
                   return AnyMakeCopy<std::string>("");
               } else {
                   return AnyMakeCopy<std::string_view>("");
               }
           }),
      m_kind(skind) {}

void String::SetValue(Any& a, const std::string& value) const {
    if (!SET_VALUE_CHECK(a, ValueKind::String)) return;

    switch (a.TypeInfo()->AsString()->StringKind()) {
        case StringKind::Unknown:
            LOGE("unknown string type");
            break;
        case StringKind::String:
            *TryCast<std::string>(a) = value;
            break;
        case StringKind::StringView:
            *TryCast<std::string_view>(a) = value;
            break;
    }
}

void String::SetValue(Any& a, std::string_view& value) const {
    if (!SET_VALUE_CHECK(a, ValueKind::String)) return;

    switch (a.TypeInfo()->AsString()->StringKind()) {
        case StringKind::Unknown:
            LOGE("unknown string type");
            break;
        case StringKind::String:
            *TryCast<std::string>(a) = value;
            break;
        case StringKind::StringView:
            *TryCast<std::string_view>(a) = value;
            break;
    }
}

std::string String::GetStr(const Any& value) const {
    if (value.TypeInfo()->AsString()->IsStringView()) {
        return std::string(*(std::string_view*)(value.Payload()));
    } else {
        return *(std::string*)(value.Payload());
    }
}

std::string_view String::GetStrView(const Any& value) const {
    if (value.TypeInfo()->AsString()->IsStringView()) {
        return *(std::string_view*)(value.Payload());
    } else {
        return *(std::string*)(value.Payload());
    }
}

}  // namespace nickel::refl