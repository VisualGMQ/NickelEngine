#pragma once
#include "quickjs.h"

namespace nickel::script {

struct QJSClassIDFamily {
    JSClassID m_id{};
    JSClassID m_const_id{};
    JSClassID m_pointer_id{};
    JSClassID m_const_pointer_id{};
    JSClassID m_ref_id{};
    JSClassID m_const_ref_id{};

    operator bool() const noexcept {
        return !(m_id == 0 || m_const_id == 0 || m_pointer_id == 0 ||
                 m_const_pointer_id == 0 || m_ref_id == 0 ||
                 m_const_ref_id == 0);
    }

    bool Has(JSClassID id) const noexcept {
        return m_id == id || m_const_id == id || m_pointer_id == id ||
               m_const_pointer_id == id || m_ref_id == id ||
               m_const_ref_id == id;
    }

    bool operator==(const QJSClassIDFamily& o) const noexcept {
        return o.m_id == m_id && o.m_const_id == m_const_id &&
               o.m_pointer_id == m_pointer_id &&
               o.m_const_pointer_id == m_const_pointer_id &&
               o.m_ref_id == m_ref_id && o.m_const_ref_id == m_const_ref_id;
    }
};

template <typename>
class QJSClassIDManager {
public:
    static const QJSClassIDFamily& GetOrGen(JSRuntime* runtime) {
        if (!m_id_family) {
            m_id_family.m_id = JS_NewClassID(runtime, &m_id_family.m_id);
            m_id_family.m_const_id =
                JS_NewClassID(runtime, &m_id_family.m_const_id);
            m_id_family.m_pointer_id =
                JS_NewClassID(runtime, &m_id_family.m_pointer_id);
            m_id_family.m_const_pointer_id =
                JS_NewClassID(runtime, &m_id_family.m_const_pointer_id);
            m_id_family.m_ref_id =
                JS_NewClassID(runtime, &m_id_family.m_ref_id);
            m_id_family.m_const_ref_id =
                JS_NewClassID(runtime, &m_id_family.m_const_ref_id);
        }

        return m_id_family;
    }

    static bool IsClassIDCreated() noexcept { return m_id_family; }

private:
    static QJSClassIDFamily m_id_family;
};

template <typename T>
QJSClassIDFamily QJSClassIDManager<T>::m_id_family;

}  // namespace nickel::script

namespace std {
template <>
struct hash<nickel::script::QJSClassIDFamily> {
    size_t operator()(
        const nickel::script::QJSClassIDFamily& p) const noexcept {
        auto hasher = hash<JSClassID>();
        return hasher(p.m_id) | hasher(p.m_const_id) |
               hasher(p.m_const_pointer_id) | hasher(p.m_pointer_id) |
               hasher(p.m_const_ref_id) | hasher(p.m_const_ref_id);
    }
};
}  // namespace std