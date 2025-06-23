#include "nickel/script/binding/class_factory.hpp"

namespace nickel::script {

void QJSClassFactory::DoRegister() const {
    for (auto& clazz : m_classes) {
        clazz->EndClass();
    }
}

}  // namespace nickel::script