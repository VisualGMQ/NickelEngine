#pragma once
#include "quickjs.h"

namespace nickel::script {

/// bunche of class id for T, const T, const T*, T*, const T&, T&
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

template <typename T>
class QJSClassIDManager {
public:
    static void RegisterID(JSClassID id) { QJSClassIDManager::id = id; }

    static JSClassID GetOrGen(JSRuntime* runtime) {
        if (id == 0) {
            id = JS_NewClassID(runtime, &id);

            JSClassDef def{};
            def.class_name = "";
            if constexpr (!std::is_reference_v<T> && !std::is_pointer_v<T>) {
                def.finalizer = +[](JSRuntime* rt, JSValue val) {
                    T* p =
                        static_cast<T*>(JS_GetOpaque(val, JS_GetClassID(val)));
                    if (p) delete p;
                };
            }
            if (JS_NewClass(runtime, id, &def) < 0) {
                LOGE("QJS new class failed");
            }
        }
        return id;
    }

private:
    static JSClassID id;
};

template <typename T>
JSClassID QJSClassIDManager<T>::id{};

template <typename T>
concept PureClass =
    std::is_class_v<T> && !std::is_const_v<T> && !std::is_volatile_v<T>;

/** manage @ref QJSClassIDFamily for class
 * @note T must be pure class type
 */
template <PureClass T>
class QJSClassIDFamilyManager {
public:
    static const QJSClassIDFamily& GetOrGen(JSRuntime* runtime) {
        if (!m_id_family) {
            m_id_family.m_id = QJSClassIDManager<T>::GetOrGen(runtime);
            m_id_family.m_const_id =
                QJSClassIDManager<const T>::GetOrGen(runtime);
            m_id_family.m_pointer_id =
                QJSClassIDManager<T*>::GetOrGen(runtime);
            m_id_family.m_const_pointer_id =
                QJSClassIDManager<const T*>::GetOrGen(runtime);
            m_id_family.m_ref_id = QJSClassIDManager<T&>::GetOrGen(runtime);
            m_id_family.m_const_ref_id =
                QJSClassIDManager<const T&>::GetOrGen(runtime);
        }

        return m_id_family;
    }

    static bool IsClassIDCreated() noexcept { return m_id_family != 0; }

private:
    static QJSClassIDFamily m_id_family;
};

template <PureClass T>
QJSClassIDFamily QJSClassIDFamilyManager<T>::m_id_family;

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