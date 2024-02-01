#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "common/util.hpp"

TEST_CASE("csv iterator") {
    std::string csv = "12,32,44,99,10,5,\n8";
    nickel::CSVIterator it(csv);
    auto end = nickel::CSVIterator<std::string>();

    REQUIRE(*it == "12");
    REQUIRE(it != end);
    it++;
    REQUIRE(*it == "32");
    it++;
    REQUIRE(*it == "44");
    it++;
    REQUIRE(*it == "99");
    it++;
    REQUIRE(*it == "10");
    it++;
    REQUIRE(*it == "5");
    it++;
    REQUIRE(*it == "8");
    it++;
    REQUIRE(it == end);
}