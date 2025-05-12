#include "nickel/refl/drefl/bool.hpp"
#include "nickel/refl/drefl/any.hpp"
#include "nickel/refl/drefl/cast_any.hpp"
#include "nickel/refl/drefl/exception.hpp"
#include "nickel/refl/drefl/make_any.hpp"

namespace nickel::refl {

Any construct_boolean() {
    return AnyMakeCopy(true);
}

Boolean::Boolean(): Type(ValueKind::Boolean, "bool", construct_boolean) {}

void Boolean::SetValue(Any& dst, bool value) const {
    if (!SET_VALUE_CHECK(dst, ValueKind::Boolean)) {
        LOGE("can't set boolean value to any");
        return;
    }

    *TryCast<bool>(dst) = value;
}

bool Boolean::GetValue(const Any& value) const {
    return *TryCastConst<bool>(value);
}

}