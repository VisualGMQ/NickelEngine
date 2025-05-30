#include "catch2/catch_test_macros.hpp"
#include "nickel/context.hpp"
#include "nickel/script/internal/script_impl.hpp"

using namespace nickel;

struct Person {
    static int static_elem;
    
    int age = 12;
    float height = 180;
    std::string name;
    const int const_value = 996;

    Person(std::string name): name{name} {}

    static void SayHello() {
        LOGI("I am person");
    }

    void Introduce() const {
        LOGI("I am {}, age = {}, height = {}", name, age, height);
    }
};

int Person::static_elem = 255;

TEST_CASE("test") {
    script::ScriptManager mgr;
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

    auto& module =
        script::QJSRuntime::GetInst().GetContext().NewModule("test_module");
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
        .AddProperty("string_elem", string_elem);
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

    LOGI("id = {}", script::QJSClass<Person>::GetID());
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
}
