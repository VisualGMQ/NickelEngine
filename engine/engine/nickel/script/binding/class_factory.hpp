#pragma once
#include "nickel/common/singleton.hpp"
#include "nickel/script/binding/class_base.hpp"
#include "nickel/script/binding/class_id.hpp"
#include "quickjs.h"

namespace nickel::script {

template <typename T>
class QJSClass;

class QJSClassFactory {
public:
    template <typename T>
    QJSClass<T>& CreateOrGet(JSContext* ctx, const std::string& name) {
        QJSClassIDFamily ids = QJSClassIDManager<T>::GetOrGen(JS_GetRuntime(ctx));

        if (ids.m_id != 0) {
            return static_cast<QJSClass<T>&>(*m_mutable_map[ids.m_id]);
        }

        if (ids.m_const_id != 0) {
            return static_cast<QJSClass<T>&>(*m_const_map[ids.m_const_id]);
        }

        if (ids.m_pointer_id != 0) {
            return static_cast<QJSClass<T>&>(*m_pointer_map[ids.m_pointer_id]);
        }

        if (ids.m_const_pointer_id != 0) {
            return static_cast<QJSClass<T>&>(
                *m_const_pointer_map[ids.m_const_pointer_id]);
        }

        if (ids.m_ref_id != 0) {
            return static_cast<QJSClass<T>&>(*m_ref_map[ids.m_ref_id]);
        }

        if (ids.m_const_ref_id != 0) {
            return static_cast<QJSClass<T>&>(
                *m_const_ref_map[ids.m_const_ref_id]);
        }

        ids.m_id = JS_NewClassID(JS_GetRuntime(ctx), &ids.m_id);
        ids.m_const_id = JS_NewClassID(JS_GetRuntime(ctx), &ids.m_const_id);
        ids.m_pointer_id = JS_NewClassID(JS_GetRuntime(ctx), &ids.m_pointer_id);
        ids.m_const_pointer_id =
            JS_NewClassID(JS_GetRuntime(ctx), &ids.m_const_pointer_id);
        ids.m_ref_id = JS_NewClassID(JS_GetRuntime(ctx), &ids.m_ref_id);
        ids.m_const_ref_id =
            JS_NewClassID(JS_GetRuntime(ctx), &ids.m_const_ref_id);

        auto& qjs_class = m_classes.emplace_back(
            std::make_unique<QJSClass<T>>(ctx, name));

        QJSClassBase* ptr = qjs_class.get();
        m_mutable_map.emplace(ids.m_id, ptr);
        m_const_map.emplace(ids.m_const_id, ptr);
        m_pointer_map.emplace(ids.m_pointer_id, ptr);
        m_const_pointer_map.emplace(ids.m_const_pointer_id, ptr);
        m_ref_map.emplace(ids.m_ref_id, ptr);
        m_const_ref_map.emplace(ids.m_const_ref_id, ptr);

        return static_cast<QJSClass<T>&>(*ptr);
    }

    void DoRegister() const;

private:
    std::vector<std::unique_ptr<QJSClassBase>> m_classes;

    std::unordered_map<JSClassID, QJSClassBase*> m_mutable_map;
    std::unordered_map<JSClassID, QJSClassBase*> m_const_map;
    std::unordered_map<JSClassID, QJSClassBase*> m_pointer_map;
    std::unordered_map<JSClassID, QJSClassBase*> m_const_pointer_map;
    std::unordered_map<JSClassID, QJSClassBase*> m_ref_map;
    std::unordered_map<JSClassID, QJSClassBase*> m_const_ref_map;
};

}  // namespace nickel::script