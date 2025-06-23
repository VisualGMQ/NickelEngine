#include "catch2/catch_test_macros.hpp"
#include "nickel/context.hpp"
#include "nickel/script/binding/module.hpp"
#include "nickel/script/binding/runtime.hpp"
#include "nickel/script/internal/script_impl.hpp"

using namespace nickel;

struct Person {
    static int static_elem;

    int age = 12;
    float height = 180;
    std::string name;
    const int const_value = 996;

    Person(std::string name) : name{name} {}

    static void SayHello() { LOGI("I am person"); }

    void Introduce() const {
        LOGI("I am {}, age = {}, height = {}", name, age, height);
    }
};

int Person::static_elem = 255;

TEST_CASE("binding") {
    SECTION("binding fundamental") {
        int int_elem = 1;
        char char_elem = 2;
        long long_elem = 3;
        unsigned int uint_elem = 4;
        unsigned char uchar_elem = 5;
        unsigned long ulong_elem = 6;
        float float_elem = 7;
        double double_elem = 8;
        bool bool_elem = true;
        std::string_view str_view_elem = "string view";
        const char* str_literal = "string literal";
        std::string string_elem = "string";

        script::QJSRuntime runtime;
        script::QJSContext& ctx = runtime.GetContext();

        auto& module = runtime.GetContext().NewModule("test_module");

        module.AddProperty("int_elem", int_elem)
            .AddProperty("char_elem", char_elem)
            .AddProperty("long_elem", long_elem)
            .AddProperty("uint_elem", uint_elem)
            .AddProperty("uchar_elem", uchar_elem)
            .AddProperty("ulong_elem", ulong_elem)
            .AddProperty("float_elem", float_elem)
            .AddProperty("double_elem", double_elem)
            .AddProperty("bool_elem", bool_elem)
            .AddProperty("string_view", str_view_elem)
            .AddProperty("str_literal", str_literal)
            .AddProperty("string_elem", string_elem)
            .EndModule();

        runtime.DoRegister();

        {
            std::string_view pre_code = R"(
                import * as test_module from 'test_module'
                globalThis.test_module = test_module
            )";
            JSValue value = JS_Eval(ctx, pre_code.data(), pre_code.size(),
                                    "module import", JS_EVAL_TYPE_MODULE);
            if (JS_IsException(value)) {
                LogJSException(ctx);
            }
            REQUIRE_FALSE(JS_IsException(value));
            JS_FreeValue(ctx, value);
        }

        std::string_view code = R"(
            function CheckExists(value) {
                if (value == undefined || value == null) {
                    throw new Error("value not exists");
                }
            }

            CheckExists(test_module.int_elem)
            CheckExists(test_module.uint_elem)
            CheckExists(test_module.char_elem)
            CheckExists(test_module.uchar_elem)
            CheckExists(test_module.long_elem)
            CheckExists(test_module.ulong_elem)
            CheckExists(test_module.float_elem)
            CheckExists(test_module.double_elem)
            CheckExists(test_module.bool_elem)
            CheckExists(test_module.string_view)
            CheckExists(test_module.str_literal)
            CheckExists(test_module.string_elem)
        )";

        JSValue value = JS_Eval(ctx, code.data(), code.size(), "test file",
                                JS_EVAL_TYPE_GLOBAL | JS_EVAL_FLAG_STRICT);

        if (JS_IsException(value)) {
            LogJSException(ctx);
        }
        REQUIRE_FALSE(JS_IsException(value));

        JS_FreeValue(ctx, value);
    }

    /*
    auto& module =
        script::QJSRuntime::GetInst().GetContext().NewModule("test_module");
    module.AddClass<Person>("Person")
        .AddConstructor<std::string>()
        .AddField<&Person::age>("age")
        .AddField<&Person::height>("height")
        .AddField<&Person::name>("name")
        .AddField<&Person::const_value>("const_value")
        .AddFunction<&Person::Introduce>("Introduce")
        .AddFunction<&Person::SayHello>("SayHello")
        .AddStaticField("static_elem", &Person::static_elem);
    module.EndModule();

    script::QJSClassFactory::GetInst().DoRegister();

    LOGI("id = {}", script::QJSClassIDManager<Person>::GetOrGen().m_id);
    LOGI("const type id = {}", script::QJSClass<Person>::GetConstTypeID());

    std::ifstream file("tests/script/qjs/test.js",
                       std::ios::in | std::ios::binary);
    if (file.fail()) {
        LOGE("fuck");
    }

    std::stringstream sstream;
    sstream << file.rdbuf();

    std::string str = sstream.str();
    auto script = mgr.Load(std::span{str});
    JSContext* ctx = script::QJSRuntime::GetInst().GetContext();
    JSValue global = JS_GetGlobalObject(ctx);
    JSValue fn = JS_GetPropertyStr(ctx, global, "test_const_var");
    Person* person = new Person{"John"};
    JSValue param = script::JSValueWrapper<const Person>{}.Wrap(ctx, *person);
    JS_Call(ctx, fn, JS_UNDEFINED, 1, &param);
    JS_FreeValue(ctx, global);
    JS_FreeValue(ctx, param);
    JS_FreeValue(ctx, fn);
    */
}
