#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "common/data_pool.hpp"

TEST_CASE("data storage") {
    nickel::DataStorage<int> storage;

    SECTION("allocate") {
        auto [value1, id1] = storage.Allocate();
        REQUIRE(id1 == nickel::Num2DataID(0));
        auto [value2, id2] = storage.Allocate();
        REQUIRE(id2 == nickel::Num2DataID(1));
    }

    auto [value1, id1] = storage.Allocate();
    auto [value2, id2] = storage.Allocate();
    auto [value3, id3] = storage.Allocate();
    *(int*)value1.data = 1;
    *(int*)value2.data = 2;
    *(int*)value3.data = 3;

    SECTION("get") {
        REQUIRE(*storage.Get(id1) == 1);
        REQUIRE(*storage.Get(id2) == 2);
        REQUIRE(*storage.Get(id3) == 3);
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
    Data1(int value): value{value} {}
    ~Data1() { gDestructCount ++; }

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