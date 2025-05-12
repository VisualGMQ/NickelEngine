#include "nickel/refl/drefl/factory.hpp"
#include "nickel/refl/drefl/class_Visitor.hpp"

namespace nickel::refl {

void NumericProperty::Visit(ClassVisitor* Visitor) {
    (*Visitor)(*this);
}

void EnumProperty::Visit(ClassVisitor* Visitor) {
    (*Visitor)(*this);
}

void ClassProperty::Visit(ClassVisitor* Visitor) {
    (*Visitor)(*this);
}

void StringProperty::Visit(ClassVisitor* Visitor) {
    (*Visitor)(*this);
}

void BooleanProperty::Visit(ClassVisitor* Visitor) {
    (*Visitor)(*this);
}

void PointerProperty::Visit(ClassVisitor* Visitor) {
    (*Visitor)(*this);
}

void ArrayProperty::Visit(ClassVisitor* Visitor) {
    (*Visitor)(*this);
}

void OptionalProperty::Visit(ClassVisitor* Visitor) {
    (*Visitor)(*this);
}

}  // namespace nickel::refl