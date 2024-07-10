#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "common/sparse_set.hpp"

TEST_CASE("Sparse Set") {
    nickel::SparseSet set;

    auto id1 = set.Create();
    auto id2 = set.Create();
    auto id3 = set.Create();

    auto& sparse = set.Sparse();
    REQUIRE(id1 == nickel::Num2DataID(0x00));
    REQUIRE(id2 == nickel::Num2DataID(0x01));
    REQUIRE(id3 == nickel::Num2DataID(0x02));

    SECTION("create") {
        auto id4 = set.Create();
        REQUIRE(id4 == nickel::Num2DataID(0x03));

        auto id5 = set.Create();
        REQUIRE(id5 == nickel::Num2DataID(0x04));
    }

    SECTION("index") {
        REQUIRE(set.Index(id1) == 0);
        REQUIRE(set.Index(id2) == 1);
        REQUIRE(set.Index(id3) == 2);
    }

    SECTION("destroy") {
        auto dst = set.Destroy(id3);
        REQUIRE(dst == 2);
        REQUIRE(set.Dense().size() == 3);
        REQUIRE(set.ElemCount() == 2);
        REQUIRE(set.Index(id1) == 0);
        REQUIRE(set.Index(id2) == 1);
        REQUIRE(nickel::Num2DataID(set.Dense()[2]) == nickel::MakeupDataID(0x02, 1));

        dst = set.Destroy(id1);
        REQUIRE(dst == 1);
        REQUIRE(set.Dense().size() == 3);
        REQUIRE(set.ElemCount() == 1);
        REQUIRE(set.Index(id1) == 1);
        REQUIRE(set.Index(id2) == 0);
        REQUIRE(nickel::Num2DataID(set.Dense()[1]) == nickel::MakeupDataID(0x00, 1));
    }

    SECTION("create with cache") {
        set.Destroy(id2);
        set.Destroy(id3);
        auto id4 = set.Create();
        auto id5 = set.Create();
        REQUIRE(id4 == nickel::MakeupDataID(0x02, 1));
        REQUIRE(id5 == nickel::MakeupDataID(0x01, 1));
        REQUIRE(set.Index(id4) == 1);
        REQUIRE(set.Index(id5) == 2);
    }

    SECTION("create-destroy-create") {
        set.Destroy(id2);
        set.Destroy(id3);
        auto id4 = set.Create();
        auto id5 = set.Create();
        set.Destroy(id4);
        REQUIRE(set.Dense().size() == 3);
        REQUIRE(set.ElemCount() == 2);
        auto id6 = set.Create();
        REQUIRE(id6 == nickel::MakeupDataID(2, 2));
    }

    SECTION("exists") {
        REQUIRE(set.Exists(id1));
        REQUIRE(set.Exists(id2));
        REQUIRE(set.Exists(id3));
        REQUIRE(!set.Exists(nickel::Num2DataID(0x04)));
    }
}