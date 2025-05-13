#include "nickel/refl/drefl/exception.hpp"
#include "nickel/refl/util/misc.hpp"
#include "catch2/catch_test_macros.hpp"

#include "nickel/refl/drefl/Any.hpp"
#include "nickel/refl/drefl/cast_any.hpp"
#include "nickel/refl/drefl/make_any.hpp"


using namespace nickel::refl;

struct test_class {
    int* copy_constructor_counter;
    int* move_constructor_counter;
    int* copy_assignment_counter;
    int* move_assignment_counter;

    test_class(int* a, int* b, int* c, int* d)
        : copy_constructor_counter(a),
          move_constructor_counter(b),
          copy_assignment_counter(c),
          move_assignment_counter(d) {}

    test_class(const test_class& o) {
        copy_all(o);
        (*copy_constructor_counter)++;
    }

    test_class(test_class&& o) {
        copy_all(o);
        (*move_constructor_counter)++;
    }

    test_class& operator=(const test_class& o) {
        copy_all(o);
        (*copy_assignment_counter)++;
        return *this;
    }

    test_class& operator=(test_class&& o) {
        copy_all(o);
        (*move_assignment_counter)++;
        return *this;
    }

private:
    void copy_all(const test_class& o) {
        copy_constructor_counter = o.copy_constructor_counter;
        copy_assignment_counter = o.copy_assignment_counter;
        move_constructor_counter = o.move_constructor_counter;
        move_assignment_counter = o.move_assignment_counter;
    }
};

TEST_CASE("create any") {
    int ccc = 0, mcc = 0, cac = 0, mac = 0;
    test_class test{&ccc, &mcc, &cac, &mac};

    auto a = AnyMakeCopy(test);
    REQUIRE(a.AccessType() == nickel::refl::Any::AccessType::Copy);
    REQUIRE(a.TypeInfo() == TypeInfo<test_class>());

    test_class* copy = TryCast<test_class>(a);

    REQUIRE(copy != nullptr);
    REQUIRE(*copy->copy_constructor_counter == 1);
    REQUIRE(*copy->move_constructor_counter == 0);
    REQUIRE(*copy->copy_assignment_counter == 0);
    REQUIRE(*copy->move_assignment_counter == 0);
}

TEST_CASE("reference hold") {
    int ccc = 0, mcc = 0, cac = 0, mac = 0;
    test_class test{&ccc, &mcc, &cac, &mac};

    auto c1 = AnyMakeRef(test);
    REQUIRE(c1.AccessType() == Any::AccessType::Ref);
    REQUIRE(c1.TypeInfo() == TypeInfo<test_class>());

    REQUIRE(ccc == 0);
    REQUIRE(mcc == 0);
    REQUIRE(cac == 0);
    REQUIRE(mac == 0);

    *TryCast<test_class>(c1)->copy_assignment_counter += 1;
    REQUIRE(cac == 1);
}

TEST_CASE("const hold") {
    int ccc = 0, mcc = 0, cac = 0, mac = 0;
    test_class test{&ccc, &mcc, &cac, &mac};
    auto c1 = AnyMakeConstRef(test);

    REQUIRE(c1.AccessType() == Any::AccessType::ConstRef);
    REQUIRE(c1.TypeInfo() == TypeInfo<test_class>());
    REQUIRE(TryCastConst<test_class>(c1) != nullptr);

    REQUIRE(ccc == 0);
    REQUIRE(mcc == 0);
    REQUIRE(cac == 0);
    REQUIRE(mac == 0);

    try {
        // constref try mutable cast will cause error
        auto elem = TryCast<test_class>(c1);
    } catch (const BadAnyAccess&) {
        REQUIRE(true);
    }
}

TEST_CASE("with enum") {
    enum class MyEnum {
        A = 1,
        B = 2,
        C = 3,
    };

    auto c1 = AnyMakeCopy(MyEnum::A);
    REQUIRE(c1.TypeInfo() == TypeInfo<MyEnum>());
    REQUIRE(c1.TypeInfo()->AsEnum()->GetValue(c1) == 1);
}

TEST_CASE("transform between any") {
    int ccc = 0, mcc = 0, cac = 0, mac = 0;
    test_class test{&ccc, &mcc, &cac, &mac};
    auto c1 = AnyMakeConstRef(test);

    SECTION("copy") {
        auto c2 = c1.Copy();

        REQUIRE(c2.AccessType() == Any::AccessType::Copy);
        REQUIRE(c2.TypeInfo() == TypeInfo<test_class>());
        REQUIRE(TryCast<test_class>(c2) != nullptr);
        REQUIRE(*test.copy_constructor_counter == 1);
        REQUIRE(*test.copy_assignment_counter == 0);
        REQUIRE(*test.move_constructor_counter == 0);
        REQUIRE(*test.move_assignment_counter == 0);
    }

    SECTION("steal") {
        auto c2 = c1.Steal();

        REQUIRE(c1.AccessType() == Any::AccessType::Null);
        REQUIRE(c2.AccessType() == Any::AccessType::ConstRef);
        REQUIRE(*test.copy_constructor_counter == 0);
        REQUIRE(*test.copy_assignment_counter == 0);
        REQUIRE(*test.move_constructor_counter == 1);
        REQUIRE(*test.move_assignment_counter == 0);
    }

    SECTION("const ref") {
        auto c2 = c1.ConstRef();

        REQUIRE(c2.AccessType() == Any::AccessType::ConstRef);
        REQUIRE(*test.copy_constructor_counter == 0);
        REQUIRE(*test.copy_assignment_counter == 0);
        REQUIRE(*test.move_constructor_counter == 0);
        REQUIRE(*test.move_assignment_counter == 0);
    }

    SECTION("ref from constref") {
        // make ref from const-ref will cause error
        try {
            auto c2 = c1.Ref();
        } catch (const BadAnyAccess&) {
            REQUIRE(true);
        }
    }

    SECTION("ref from ref") {
        int ccc = 0, mcc = 0, cac = 0, mac = 0;
        test_class test{&ccc, &mcc, &cac, &mac};
        auto c2 = AnyMakeRef(test);

        auto c3 = c2.Ref();

        REQUIRE(c3.AccessType() == Any::AccessType::Ref);
        REQUIRE(c3.TypeInfo() == c2.TypeInfo());
        *TryCast<test_class>(c3)->copy_assignment_counter += 1;

        REQUIRE(*TryCast<test_class>(c3)->copy_assignment_counter == 1);
    }
}