#pragma once
#include "quickjs.h"
#include "nickel/common/log.hpp"

namespace nickel::script {

/// bunche of class id for T, const T, const T*, T*, const T&, T&
struct QJSClassIDFamily {
    JSClassID m_id{};
    JSClassID m_const_id{};
    JSClassID m_pointer_id{};
    JSClassID m_const_pointer_id{};
    JSClassID m_ref_id{};
    JSClassID m_const_ref_id{};

    explicit operator bool() const noexcept {
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

class QJSClassIDManager {
public:
    explicit QJSClassIDManager(JSRuntime* runtime) : m_runtime{runtime} {}

    template <typename T>
    JSClassID GetOrGen() {
        static size_t type_id = m_cur_type_id++;

        if (!m_id_map.contains(type_id)) {
            JSClassID id = 0;
            id = JS_NewClassID(m_runtime, &id);
            m_id_map[type_id] = id;

            JSClassDef def{};
            def.class_name = "";
            if constexpr (!std::is_reference_v<T> && !std::is_pointer_v<T>) {
                def.finalizer = +[](JSRuntime* rt, JSValue val) {
                    T* p =
                        static_cast<T*>(JS_GetOpaque(val, JS_GetClassID(val)));
                    if (p) delete p;
                };
            }
            if (JS_NewClass(m_runtime, id, &def) < 0) {
                LOGE("QJS new class failed");
            }
            return id;
        }
        
        return m_id_map[type_id];
    }

private:
    JSRuntime* m_runtime{};
    std::unordered_map<uint32_t, JSClassID> m_id_map;
    static uint32_t m_cur_type_id;
};

template <typename T>
concept PureClass =
    std::is_class_v<T> && !std::is_const_v<T> && !std::is_volatile_v<T>;

/** manage @ref QJSClassIDFamily for class
 * @note T must be pure class type
 */
class QJSClassIDFamilyManager {
public:
    explicit QJSClassIDFamilyManager(JSRuntime* runtime, QJSClassIDManager& mgr)
        : m_mgr{mgr}, m_runtime{runtime} {}

    template <PureClass T>
    QJSClassIDFamily GetOrGen() {
        QJSClassIDFamily id_family;
        id_family.m_id = m_mgr.GetOrGen<T>();
        id_family.m_const_id = m_mgr.GetOrGen<const T>();
        id_family.m_pointer_id = m_mgr.GetOrGen<T*>();
        id_family.m_const_pointer_id = m_mgr.GetOrGen<const T*>();
        id_family.m_ref_id = m_mgr.GetOrGen<T&>();
        id_family.m_const_ref_id = m_mgr.GetOrGen<const T&>();

        return id_family;
    }

private:
    QJSClassIDManager& m_mgr;
    JSRuntime* m_runtime{};
};

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