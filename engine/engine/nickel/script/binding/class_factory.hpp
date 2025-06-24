#pragma once
#include "nickel/script/binding/class_base.hpp"
#include "nickel/script/binding/class_id.hpp"
#include "quickjs.h"

#include <unordered_set>

namespace nickel::script {

template <typename T>
class QJSClass;

class QJSClassFactory {
public:
    template <typename T>
    QJSClass<T>& CreateOrGet(JSContext* ctx, const std::string& name) {
        QJSClassIDFamily ids = QJSClassIDManager<T>::GetOrGen(JS_GetRuntime(ctx));

        if (m_class_ids.contains(ids)) {
            return static_cast<QJSClass<T>&>(*m_classes_map[ids.m_id]);
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
        m_classes_map[ids.m_id] = ptr;

        return static_cast<QJSClass<T>&>(*ptr);
    }

private:
    std::vector<std::unique_ptr<QJSClassBase>> m_classes;

    std::unordered_set<QJSClassIDFamily> m_class_ids;
    std::unordered_map<JSClassID, QJSClassBase*> m_classes_map;
};

}  // namespace nickel::script