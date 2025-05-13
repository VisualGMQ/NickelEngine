#include "catch2/catch_test_macros.hpp"
#include "nickel/refl/drefl/array.hpp"
#include "nickel/refl/drefl/any.hpp"
#include "nickel/refl/drefl/raw_type.hpp"
#include "nickel/refl/util/misc.hpp"

#include "nickel/refl/drefl/factory.hpp"
#include "nickel/refl/drefl/cast_any.hpp"
#include "nickel/refl/drefl/make_any.hpp"
#include "nickel/refl/drefl/type.hpp"
#include "nickel/refl/drefl/value_kind.hpp"

TEST_CASE("enum factory") {
    enum class MyEnum {
        Value1 = 1,
        Value2 = 2,
        Value3 = 3,
    };

    auto& inst = nickel::refl::EnumFactory<MyEnum>::Instance();

    inst.Regist("MyEnum")
        .Add("Value1", MyEnum::Value1)
        .Add("Value2", MyEnum::Value2)
        .Add("Value3", MyEnum::Value3);

    REQUIRE(inst.HasRegistered());
    auto& enum_info = inst.Info();

    REQUIRE(enum_info.Name() == "MyEnum");
    REQUIRE(enum_info.Enums()[0].Name() == "Value1");
    REQUIRE(enum_info.Enums()[0].Value() == 1);
    REQUIRE(enum_info.Enums()[1].Name() == "Value2");
    REQUIRE(enum_info.Enums()[1].Value() == 2);
    REQUIRE(enum_info.Enums()[2].Name() == "Value3");
    REQUIRE(enum_info.Enums()[2].Value() == 3);
}

TEST_CASE("simple property factory") {
    struct Person {
        float a;
        const bool& c;
        int* p;
    };

    SECTION("simple property") {
        auto prop =
            nickel::refl::NumericPropertyFactory{"a", &Person::a}.Get();
        REQUIRE_FALSE(prop->IsConst());
        REQUIRE_FALSE(prop->IsRef());
        REQUIRE(prop->Name() == "a");
    }

    SECTION("pointer property") {
        auto prop =
            nickel::refl::NumericPropertyFactory{"p", &Person::p}.Get();
        REQUIRE_FALSE(prop->IsConst());
        REQUIRE_FALSE(prop->IsRef());
        REQUIRE(prop->Name() == "p");
    }
}

TEST_CASE("numeric factory") {
    auto& n = nickel::refl::NumericFactory<int>::Instance().Info();

    SECTION("type check") {
        REQUIRE(n.Kind() == nickel::refl::ValueKind::Numeric);
        REQUIRE(n.NumericKind() == nickel::refl::Numeric::NumericKind::Int);
        REQUIRE(n.Name() == "int");
    }

    SECTION("operations") {
        int a = 123;
        auto ref = nickel::refl::AnyMakeRef(a);
        REQUIRE(n.GetValue(ref) == 123);
        n.SetValue(ref, 456l);
        REQUIRE(a == 456);
    }
}

TEST_CASE("boolean factory") {
    auto& n = nickel::refl::BooleanFactory::Instance().Info();
    REQUIRE(n.Kind() == nickel::refl::ValueKind::Boolean);
    REQUIRE(n.Name() == "bool");
}

TEST_CASE("string factory") {
    SECTION("std::string") {
        auto& n = nickel::refl::StringFactory<std::string>::Instance().Info();
        REQUIRE(n.Kind() == nickel::refl::ValueKind::String);
        REQUIRE(n.IsString());
        REQUIRE_FALSE(n.IsStringView());
        REQUIRE(n.Name() == "std::string");
    }

    SECTION("std::string_view") {
        auto& n =
            nickel::refl::StringFactory<std::string_view>::Instance().Info();
        REQUIRE(n.Kind() == nickel::refl::ValueKind::String);
        REQUIRE(n.IsStringView());
        REQUIRE_FALSE(n.IsString());
        REQUIRE(n.Name() == "std::string_view");
    }
}

TEST_CASE("pointer factory") {
    SECTION("int*") {
        auto& n = nickel::refl::PointerFactory<int*>::Instance().Info();
        REQUIRE(n.Kind() == nickel::refl::ValueKind::Pointer);
        REQUIRE(n.Name() == "int*");
        REQUIRE(n.Layers() == 1);
        REQUIRE_FALSE(n.IsConst());
        REQUIRE_FALSE(n.IsPointTypeConst());
        REQUIRE(n.TypeInfo() == nickel::refl::TypeInfo<int>());
    }

    SECTION("int* const") {
        auto& n = nickel::refl::PointerFactory<int* const>::Instance().Info();
        REQUIRE(n.Kind() == nickel::refl::ValueKind::Pointer);
        REQUIRE(n.Layers() == 1);
        REQUIRE(n.IsConst());
        REQUIRE_FALSE(n.IsPointTypeConst());
        REQUIRE(n.TypeInfo() == nickel::refl::TypeInfo<int>());
    }

    SECTION("const int*") {
        auto& n = nickel::refl::PointerFactory<const int*>::Instance().Info();
        REQUIRE(n.Kind() == nickel::refl::ValueKind::Pointer);
        REQUIRE(n.Layers() == 1);
        REQUIRE_FALSE(n.IsConst());
        REQUIRE(n.IsPointTypeConst());
        REQUIRE(n.TypeInfo() == nickel::refl::TypeInfo<int>());
    }

    SECTION("const int* const") {
        auto& n =
            nickel::refl::PointerFactory<const int* const>::Instance().Info();
        REQUIRE(n.Kind() == nickel::refl::ValueKind::Pointer);
        REQUIRE(n.Layers() == 1);
        REQUIRE(n.IsConst());
        REQUIRE(n.IsPointTypeConst());
        REQUIRE(n.TypeInfo() == nickel::refl::TypeInfo<int>());
    }

    SECTION("int***") {
        auto& n = nickel::refl::PointerFactory<int***>::Instance().Info();
        REQUIRE(n.Kind() == nickel::refl::ValueKind::Pointer);
        REQUIRE(n.Layers() == 3);
        REQUIRE_FALSE(n.IsConst());
        REQUIRE_FALSE(n.IsPointTypeConst());
        REQUIRE(n.TypeInfo() == nickel::refl::TypeInfo<int>());
    }

    SECTION("const int** const") {
        auto& n = nickel::refl::PointerFactory<const int** const>::Instance()
                      .Info();
        REQUIRE(n.Kind() == nickel::refl::ValueKind::Pointer);
        REQUIRE(n.Layers() == 2);
        REQUIRE(n.IsConst());
        REQUIRE(n.IsPointTypeConst());
        REQUIRE(n.TypeInfo() == nickel::refl::TypeInfo<int>());
    }
}

TEST_CASE("ordinary array", "array") {
    auto& n = nickel::refl::ArrayFactory<int[3]>::Instance().Info();

    SECTION("type check") {
        REQUIRE(n.ArrayType() == nickel::refl::Array::ArrayType::Static);
        REQUIRE(n.AddressingType() ==
                nickel::refl::Array::AddressingType::Random);
    }

    SECTION("operation check") {
        int arr[3] = {1, 2, 3};
        auto ref = nickel::refl::AnyMakeRef(arr);
        REQUIRE(ref.TypeInfo() == &n);

        auto elem = n.Get(0, ref);
        REQUIRE(elem.AccessType() == nickel::refl::Any::AccessType::Ref);
        REQUIRE(elem.TypeInfo()->AsNumeric()->GetValue(elem) == 1);
        REQUIRE(n.Size(ref) == 3);
        REQUIRE(n.Capacity(ref) == 3);
        REQUIRE_FALSE(n.PushBack(elem, ref));
        REQUIRE_FALSE(n.Back(ref).HasValue());
        REQUIRE_FALSE(n.Resize(3, ref));
        REQUIRE_FALSE(n.Insert(3, elem, ref));
    }
}

TEST_CASE("std::vector", "array") {
    using type = std::vector<int>;
    auto& n = nickel::refl::ArrayFactory<type>::Instance().Info();

    SECTION("type check") {
        REQUIRE(n.ArrayType() == nickel::refl::Array::ArrayType::Dynamic);
        REQUIRE(n.AddressingType() ==
                nickel::refl::Array::AddressingType::Random);
        REQUIRE(n.ElemType() == nickel::refl::Factory<int>::Info());
    }

    SECTION("operation check") {
        type arr = {1, 2, 3};
        auto ref = nickel::refl::AnyMakeRef(arr);
        REQUIRE(ref.TypeInfo() == &n);

        auto elem = n.Get(0, ref);
        REQUIRE(elem.AccessType() == nickel::refl::Any::AccessType::Ref);
        REQUIRE(elem.TypeInfo()->AsNumeric()->GetValue(elem) == 1);
        REQUIRE(n.Size(ref) == 3);
        REQUIRE(n.Capacity(ref) == arr.capacity());
        REQUIRE(n.PushBack(elem, ref));

        elem = n.Back(ref);
        REQUIRE(elem.TypeInfo()->AsNumeric()->GetValue(elem) == 1);
        REQUIRE(n.Resize(2, ref));
        REQUIRE(n.Size(ref) == 2);

        elem = elem.Copy();
        elem.TypeInfo()->AsNumeric()->SetValue(elem, 9l);
        REQUIRE(n.Insert(0, elem, ref));
        REQUIRE(arr[0] == 9);
    }
}

TEST_CASE("std::list", "array") {
    using type = std::list<int>;
    auto& n = nickel::refl::ArrayFactory<type>::Instance().Info();

    SECTION("type check") {
        REQUIRE(n.ArrayType() == nickel::refl::Array::ArrayType::Dynamic);
        REQUIRE(n.AddressingType() ==
                nickel::refl::Array::AddressingType::Forward);
        REQUIRE(n.ElemType() == nickel::refl::Factory<int>::Info());
    }

    SECTION("operation check") {
        type arr = {1, 2, 3};
        auto ref = nickel::refl::AnyMakeRef(arr);
        REQUIRE(ref.TypeInfo() == &n);

        auto elem = n.Get(0, ref);
        REQUIRE(elem.AccessType() == nickel::refl::Any::AccessType::Ref);
        REQUIRE(elem.TypeInfo()->AsNumeric()->GetValue(elem) == 1);
        REQUIRE(n.Size(ref) == 3);
        REQUIRE(n.Capacity(ref) == arr.size());
        REQUIRE(n.PushBack(elem, ref));

        elem = n.Back(ref);
        REQUIRE(elem.TypeInfo()->AsNumeric()->GetValue(elem) == 1);
        REQUIRE(n.Resize(2, ref));
        REQUIRE(n.Size(ref) == 2);

        elem = elem.Copy();
        elem.TypeInfo()->AsNumeric()->SetValue(elem, 9l);
        REQUIRE(n.Insert(0, elem, ref));
        REQUIRE(*arr.begin() == 9);
    }
}


TEST_CASE("class factory") {
    struct Person {
        std::string name;
        float height;
        const bool hasChild;
        const Person* couple;
    };

    auto& p = nickel::refl::ClassFactory<Person>::Instance()
                  .Regist("Person")
                  .Property("name", &Person::name)
                  .Property("height", &Person::height)
                  .Property("hasChild", &Person::hasChild)
                  .Property("couple", &Person::couple)
                  .Info();

    REQUIRE(p.Name() == "Person");
    REQUIRE(p.Kind() == nickel::refl::ValueKind::Class);
    auto& props = p.Properties();

    Person inst { "VisualGMQ", 123.0, false, nullptr };
    auto ref = nickel::refl::AnyMakeConstRef(inst);

    SECTION("string property") {
        auto& prop = props[0];
        REQUIRE(prop->Kind() == nickel::refl::ValueKind::Property);
        REQUIRE(prop->ClassInfo() == &p);
        REQUIRE_FALSE(prop->IsConst());
        REQUIRE_FALSE(prop->IsRef());
        REQUIRE(prop->Name() == "name");
        REQUIRE(prop->TypeInfo() == nickel::refl::TypeInfo<std::string>());

        auto name = prop->Call(ref);
        REQUIRE(name.TypeInfo() == nickel::refl::TypeInfo<std::string>());
        REQUIRE(*nickel::refl::TryCastConst<std::string>(name) == "VisualGMQ");
    }

    SECTION("float property") {
        auto& prop = props[1];
        REQUIRE(prop->Kind() == nickel::refl::ValueKind::Property);
        REQUIRE(prop->ClassInfo() == &p);
        REQUIRE_FALSE(prop->IsConst());
        REQUIRE_FALSE(prop->IsRef());
        REQUIRE(prop->Name() == "height");
        REQUIRE(prop->TypeInfo() == nickel::refl::TypeInfo<float>());

        auto height = prop->Call(ref);
        REQUIRE(height.TypeInfo() == nickel::refl::TypeInfo<float>());
        REQUIRE(*nickel::refl::TryCastConst<float>(height) == 123);
    }

    SECTION("bool property") {
        auto& prop = props[2];
        REQUIRE(prop->Kind() == nickel::refl::ValueKind::Property);
        REQUIRE(prop->ClassInfo() == &p);
        REQUIRE(prop->IsConst());
        REQUIRE_FALSE(prop->IsRef());
        REQUIRE(prop->Name() == "hasChild");
        REQUIRE(prop->TypeInfo() == nickel::refl::TypeInfo<bool>());

        auto hasChild = prop->Call(ref);
        REQUIRE(hasChild.TypeInfo() == nickel::refl::TypeInfo<bool>());
        REQUIRE(*nickel::refl::TryCastConst<bool>(hasChild) == false);
    }

    SECTION("couple property") {
        auto& prop = props[3];
        REQUIRE(prop->Kind() == nickel::refl::ValueKind::Property);
        REQUIRE(prop->ClassInfo() == &p);
        REQUIRE_FALSE(prop->IsConst());
        REQUIRE_FALSE(prop->IsRef());
        REQUIRE(prop->Name() == "couple");

        auto couple = prop->CallConst(ref);


        auto q = static_cast<const Person*>(ref.Payload());
        auto& value = q->*(&Person::couple);
        auto q2 = static_cast<const Person* const>(couple.Payload());

        REQUIRE(couple.TypeInfo() == nickel::refl::TypeInfo<const Person*>());
        REQUIRE(*nickel::refl::TryCastConst<const Person*>(couple) == nullptr);
    }
}
