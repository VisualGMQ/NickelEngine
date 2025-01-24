#include "catch2/catch_test_macros.hpp"
#include "nickel/common/memory/memory.hpp"

using namespace nickel;

uint32_t gDestructCount = 0;

struct Num {
    int num;

    Num(int num): num{num} {}
    ~Num() { gDestructCount++; }
};

struct ThrowException {
    ThrowException() {
        throw std::out_of_range("custom exception");
    }
};

TEST_CASE("block memory") {
    SECTION("allocate") {
        BlockMemoryAllocator<uint32_t> allocator(4);

        uint32_t* value1 = allocator.Allocate(1);
        REQUIRE(*value1 == 1);
        REQUIRE(allocator.UnuseCount(0) == 3);
        REQUIRE(allocator.InuseCount(0) == 1);
        uint32_t* value2 = allocator.Allocate(2);
        REQUIRE(*value2 == 2);
        REQUIRE(allocator.UnuseCount(0) == 2);
        REQUIRE(allocator.InuseCount(0) == 2);
        uint32_t* value3 = allocator.Allocate(3);
        REQUIRE(*value3 == 3);
        REQUIRE(allocator.UnuseCount(0) == 1);
        REQUIRE(allocator.InuseCount(0) == 3);
        uint32_t* value4 = allocator.Allocate(4);
        REQUIRE(*value4 == 4);
        REQUIRE(allocator.UnuseCount(0) == 0);
        REQUIRE(allocator.InuseCount(0) == 4);
        REQUIRE(allocator.BlockCount() == 1);

        uint32_t* value5 = allocator.Allocate(5);
        REQUIRE(*value5 == 5);
        REQUIRE(allocator.BlockCount() == 2);
        REQUIRE(allocator.UnuseCount(1) == 3);
        REQUIRE(allocator.InuseCount(1) == 1);
    }

    SECTION("deallocate") {
        BlockMemoryAllocator<Num> allocator(4);

        Num* value1 = allocator.Allocate(1);
        Num* value2 = allocator.Allocate(2);
        Num* value3 = allocator.Allocate(3);
        REQUIRE(allocator.UnuseCount(0) == 1);
        REQUIRE(gDestructCount == 0);
        allocator.Deallocate(value1);
        REQUIRE(allocator.UnuseCount(0) == 2);
        REQUIRE(gDestructCount == 1);
        allocator.Deallocate(value2);
        REQUIRE(allocator.UnuseCount(0) == 3);
        REQUIRE(gDestructCount == 2);
        allocator.Deallocate(value3);
        REQUIRE(allocator.UnuseCount(0) == 4);
        REQUIRE(gDestructCount == 3);
    }

    SECTION("strong exception guarantee") {
        BlockMemoryAllocator<ThrowException> allocator(4);

        ThrowException* elem{};
        elem = allocator.Allocate();
        REQUIRE(elem == nullptr);
        REQUIRE(allocator.InuseCount(0) == 0);
        REQUIRE(allocator.UnuseCount(0) == 4);
    }
}