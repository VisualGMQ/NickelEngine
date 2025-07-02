#include "catch2/catch_test_macros.hpp"
#include "nickel/common/handle.hpp"
#include "nickel/common/memory/memory.hpp"

using namespace nickel;

struct TestClass: public RefCountable {
    bool Foo() const {
        return true;
    }
};

TEST_CASE("handle") {
    BlockMemoryAllocator<TestClass> allocator;

    using TestHandle = Handle<TestClass, BlockMemoryAllocator<TestClass>>;

    SECTION("null handle") {
        TestHandle handle;
        REQUIRE_FALSE(handle);
    }

    TestHandle handle(allocator.Allocate(), allocator);
    REQUIRE(handle);
    REQUIRE(handle->Foo());

    REQUIRE(handle.Payload()->Refcount() == 1);

    SECTION("copy") {
        TestHandle handle2 = handle;
        REQUIRE(handle.Payload()->Refcount() == 2);
        REQUIRE(handle2.Payload()->Refcount() == 2);
    }

    SECTION("move") {
        TestHandle handle2 = std::move(handle);
        REQUIRE_FALSE(handle);
        REQUIRE(handle2.Payload()->Refcount() == 1);       
    }
}
