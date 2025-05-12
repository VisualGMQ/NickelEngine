#include "catch2/catch_test_macros.hpp"
#include "nickel/refl/drefl/any.hpp"
#include "nickel/refl/drefl/cast_any.hpp"
#include "nickel/refl/drefl/make_any.hpp"
#include "nickel/refl/internal/serd_backends/tomlplusplus.hpp"
#include <array>
#include <iostream>

#define TOML_EXCEPTIONS 0
#include "toml++/toml.hpp"

class Person {
public:
    Person() = default;

    Person(const std::string& name, float height, bool male,
           const std::array<int, 5>& ids, std::optional<int> opt)
        : name(name), height(height), male(male), ids(ids), opt(opt) {}

    std::string name;
    float height;
    bool male;
    std::array<int, 5> ids;
    std::optional<int> opt;

    bool operator==(const Person& o) const {
        return o.name == name && o.height == height && male == o.male &&
               ids == o.ids && opt == o.opt;
    }
};

TEST_CASE("serialization & deserialization") {
    nickel::refl::ClassFactory<Person>::Instance()
        .Regist("Person")
        .Property("name", &Person::name)
        .Property("height", &Person::height)
        .Property("male", &Person::male)
        .Property("ids", &Person::ids)
        .Property("opt", &Person::opt);

    auto value = nickel::refl::AnyMakeCopy(Person{
        "VisualGMQ", 123.0, true, {1, 2, 3, 4, 5}, 3
    });

    toml::table tbl;
    nickel::refl::serialize(tbl, value, "Person");

    std::cout << toml::toml_formatter{tbl} << std::endl;

    value = nickel::refl::AnyMakeCopy(Person{
        "", 0.0, false, {0, 0, 0, 0, 0}, std::nullopt
    });
    nickel::refl::deserialize(value, tbl);

    Person* person = nickel::refl::TryCast<Person>(value);
    REQUIRE(person);
    REQUIRE(*person == *nickel::refl::TryCastConst<Person>(value));
}