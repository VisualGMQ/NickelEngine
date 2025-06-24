#include "catch2/catch_test_macros.hpp"
#include "nickel/context.hpp"
#include "nickel/script/binding/module.hpp"
#include "nickel/script/binding/runtime.hpp"
#include "nickel/script/internal/script_impl.hpp"

using namespace nickel;

struct Person {
    enum class Enum {
        Value1, Value2
    };
    
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

enum class MyEnum {
    Value1 = 0,
    Value2,
    Value3,
};

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

        {
            std::string_view pre_code = R"(
                import * as test_module from 'test_module'
                globalThis.test_module = test_module

                globalThis.CheckExists = function(value) {
                    if (value == undefined || value == null) {
                        throw new Error("value not exists")
                    }
                }

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

    script::QJSRuntime runtime;

    auto& ctx = runtime.GetContext();
    auto& module = ctx.NewModule("test_module");
    module.AddClass<Person>("Person")
            .AddConstructor<std::string>()
            .AddField<&Person::age>("age")
            .AddField<&Person::height>("height")
            .AddField<&Person::name>("name")
            .AddField<&Person::const_value>("const_value")
            .AddFunction<&Person::Introduce>("Introduce")
            .AddFunction<&Person::SayHello>("SayHello")
            .AddStaticField("static_elem", &Person::static_elem)
            .AddEnum<Person::Enum>("Enum")
                .AddItem(Person::Enum::Value1, "Value1")
                .AddItem(Person::Enum::Value2, "Value2")
            .EndEnum()
        .EndClass()
        .AddProperty<+[](const Person& p) -> const Person& { return p; }>(
            "ConstRefTest")
        .AddProperty<+[](Person& p) -> Person& {
            return p;
        }>("RefTest")
        .AddProperty<+[](Person* p) -> Person* { return p; }>("PtrTest")
        .AddProperty<+[](const Person* p) -> const Person* { return p; }>(
            "ConstPtrTest")
        .AddEnum<MyEnum>("MyEnum")
            .AddItem(MyEnum::Value1, "Value1")
            .AddItem(MyEnum::Value2, "Value2")
            .AddItem(MyEnum::Value3, "Value3")
        .EndEnum()
    .EndModule();

    {
        std::string_view pre_code = R"(
                import * as test_module from 'test_module'
                globalThis.test_module = test_module

                globalThis.CheckExists = function(value) {
                    if (value == undefined || value == null) {
                        throw new Error("value not exists")
                    }
                }
            )";
        JSValue value = JS_Eval(ctx, pre_code.data(), pre_code.size(),
                                "module import", JS_EVAL_TYPE_MODULE);
        if (JS_IsException(value)) {
            LogJSException(ctx);
        }
        REQUIRE_FALSE(JS_IsException(value));
        JS_FreeValue(ctx, value);
    }

    SECTION("binding class", "[new instance in js]") {
        std::string_view code = R"(
            let person = new test_module.Person("John")
            CheckExists(person.const_value)

            CheckExists(person.age)
            CheckExists(person.height)
            CheckExists(person.name)

            test_module.Person.SayHello()
            person.Introduce()

            person.age = 123
            if (person.age != 123) {
                throw new Error("test failed");
            }
            person.name = "VisualGMQ"
            if (person.name != "VisualGMQ") {
                throw new Error("test failed")
            }
            CheckExists(test_module.Person.static_elem)
        )";

        JSValue value = JS_Eval(ctx, code.data(), code.size(), "test file",
                                JS_EVAL_TYPE_GLOBAL | JS_EVAL_FLAG_STRICT);

        if (JS_IsException(value)) {
            LogJSException(ctx);
        }
        REQUIRE_FALSE(JS_IsException(value));

        JS_FreeValue(ctx, value);
    }

    SECTION("binding class", "[pass by reference/pointer]") {
        std::string_view code = R"(
            let person = new test_module.Person("John")
            CheckExists(test_module.RefTest)
            let ref_p = test_module.RefTest(person)
            ref_p.Introduce()

            CheckExists(test_module.ConstRefTest)
            let cref_p = test_module.ConstRefTest(person)
            cref_p.Introduce()

            CheckExists(test_module.PtrTest)
            let ptr_p = test_module.PtrTest(person)
            ptr_p.Introduce()

            CheckExists(test_module.ConstPtrTest)
            let cptr_p = test_module.ConstPtrTest(person)
            cptr_p.Introduce()
        )";

        JSValue value = JS_Eval(ctx, code.data(), code.size(), "test file",
                                JS_EVAL_TYPE_GLOBAL | JS_EVAL_FLAG_STRICT);

        if (JS_IsException(value)) {
            LogJSException(ctx);
        }
        REQUIRE_FALSE(JS_IsException(value));

        JS_FreeValue(ctx, value);
    }

    SECTION("binding enum") {
        std::string_view code = R"(
            CheckExists(test_module.MyEnum)
            CheckExists(test_module.MyEnum.Value1)
            CheckExists(test_module.MyEnum.Value2)
            CheckExists(test_module.MyEnum.Value3)

            CheckExists(test_module.Person.Enum.Value1)
            CheckExists(test_module.Person.Enum.Value2)

            function CheckValue(v, value) {
                if (v != value) {
                    throw new Error("test failed");
                }
            }

            CheckValue(test_module.MyEnum.Value1, 0)
            CheckValue(test_module.MyEnum.Value2, 1)
            CheckValue(test_module.MyEnum.Value3, 2)

            CheckValue(test_module.Person.Enum.Value1, 0)
            CheckValue(test_module.Person.Enum.Value2, 1)
        )";

        JSValue value = JS_Eval(ctx, code.data(), code.size(), "test file",
                                JS_EVAL_TYPE_GLOBAL | JS_EVAL_FLAG_STRICT);

        if (JS_IsException(value)) {
            LogJSException(ctx);
        }
        REQUIRE_FALSE(JS_IsException(value));

        JS_FreeValue(ctx, value);
    }
}
