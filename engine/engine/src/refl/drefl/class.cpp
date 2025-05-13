#include "nickel/refl/drefl/class.hpp"
#include "nickel/refl/drefl/type.hpp"
#include "nickel/refl/drefl/any.hpp"

namespace nickel::refl {

void Class::SetValue(Any& from, Any& to) {
    if (!COPY_VALUE_CHECK(from, ValueKind::Class) ||
        !SET_VALUE_CHECK(to, ValueKind::Class))
        return;

    if (from.TypeInfo() != to.TypeInfo()) return;

    to.m_operations->copy_assignment(to.Payload(), from.Payload());
}

void Class::StealValue(Any& from, Any& to) {
    if (!COPY_VALUE_CHECK(from, ValueKind::Class) ||
        !SET_VALUE_CHECK(to, ValueKind::Class))
        return;

    if (from.TypeInfo() != to.TypeInfo()) return;

    to.m_operations->steal_assignment(to.Payload(), from.Payload());
}

}  // namespace nickel::refl