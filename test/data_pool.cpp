#define CATCH_CONFIG_MAIN
#include "common/data_pool.hpp"
#include "catch.hpp"


TEST_CASE("data storage") {
    nickel::DataStorage<int> storage;

    SECTION("allocate") {
        auto [value1, refcount1, id1] = storage.Allocate();
        REQUIRE(id1 == nickel::Num2DataID(0));
        auto [value2, refcount2, id2] = storage.Allocate();
        REQUIRE(id2 == nickel::Num2DataID(1));
    }

    auto [valueBundle1, refcount1, id1] = storage.Allocate();
    auto [valueBundle2, refcount2, id2] = storage.Allocate();
    auto [valueBundle3, refcount3, id3] = storage.Allocate();
    *valueBundle1 = 1;
    *valueBundle2 = 2;
    *valueBundle3 = 3;

    SECTION("get") {
        REQUIRE(*(storage.Get(id1).value) == 1);
        REQUIRE(*(storage.Get(id2).value) == 2);
        REQUIRE(*(storage.Get(id3).value) == 3);
    }

    SECTION("get with ref") {
        storage.GetWithRef(id1);
        storage.GetWithRef(id1);
        REQUIRE(storage.GetRefCount(id1) == 2);
    }

    SECTION("Destroy") {
        storage.Destroy(id1);
        REQUIRE(!storage.Exists(id1));
    }
}

int gDestructCount = 0;

struct Data1 final {
    Data1(int value) : value{value} {}

    ~Data1() { gDestructCount++; }

    int value;
};

TEST_CASE("data pool") {
    nickel::DataPool::Init();

    auto& pool = nickel::DataPool::Instance();

    auto ref1 = pool.Emplace<Data1>(1);
    REQUIRE(ref1 != nickel::InvalidDataID);
    REQUIRE(pool.Get<Data1>(ref1)->value == 1);

    auto ref2 = pool.Emplace<Data1>(2);
    REQUIRE(ref2 != nickel::InvalidDataID);
    REQUIRE(pool.Get<Data1>(ref2)->value == 2);

    SECTION("destroy") {
        pool.Destroy<Data1>(ref1);
        REQUIRE(!pool.Exists<Data1>(ref1));
        pool.Destroy<Data1>(ref2);
        REQUIRE(!pool.Exists<Data1>(ref2));
        REQUIRE(ref1 != nickel::InvalidDataID);
        REQUIRE(ref2 != nickel::InvalidDataID);
        REQUIRE(gDestructCount == 2);
    }

    nickel::DataPool::Delete();
}