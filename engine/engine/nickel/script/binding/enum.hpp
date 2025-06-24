#pragma once
#include "nickel/script/binding/enum_base.hpp"
#include "nickel/script/binding/common.hpp"
#include "quickjs.h"

namespace nickel::script {

class QJSContext;

template <typename T, typename Upper>
requires std::is_enum_v<T>
class QJSEnum: public QJSEnumBase {
public:
    using underlying_type = std::underlying_type_t<T>;

    QJSEnum(Upper& upper_obj, JSContext* ctx,
            const std::string& name)
        : m_context(ctx), m_upper_obj{upper_obj}, m_name(name) {
        m_value = JS_NewObject(ctx);
        JS_VALUE_CHECK(ctx, m_value);
    }

    QJSEnum& AddItem(T value, const std::string& name) {
        JSValue item_value = JS_UNDEFINED;
        underlying_type numeric = static_cast<underlying_type>(value);
        if constexpr (sizeof(underlying_type) <= 4) {
            if constexpr (std::is_unsigned_v<underlying_type>) {
                item_value = JS_NewUint32(m_context, numeric);
            } else {
                item_value = JS_NewInt32(m_context, numeric);
            }
        } else {
            if constexpr (std::is_unsigned_v<underlying_type>) {
                item_value = JS_NewBigUint64(m_context, numeric);
            } else {
                item_value = JS_NewBigInt64(m_context, numeric);
            }
        }

        QJS_CALL(m_context, JS_SetPropertyStr(m_context, m_value, name.c_str(),
                                              item_value));
        return *this;
    }

    Upper& EndEnum() { return m_upper_obj; }

    const std::string& GetName() const override { return m_name; }

    JSValue GetValue() const override { return m_value; }

private:
    JSContext* m_context;
    Upper& m_upper_obj;
    JSValue m_value;
    std::string m_name;
};

}  // namespace nickel::script