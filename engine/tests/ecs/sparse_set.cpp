#include "catch2/catch_test_macros.hpp"
#include "nickel/ecs/internal/sparse_set.hpp"

using namespace nickel::ecs;

struct NumericStorage {
    uint32_t value = std::numeric_limits<uint32_t>::max();
    uint32_t dense_idx = std::numeric_limits<uint32_t>::max();

    constexpr NumericStorage() = default;
    explicit constexpr NumericStorage(uint32_t value): value{value} {}

    NumericStorage(NumericStorage const&) = default;
    NumericStorage(NumericStorage&&) = default;
    NumericStorage& operator=(NumericStorage const&) = default;
    NumericStorage& operator=(NumericStorage&&) = default;
    
    bool operator==(const NumericStorage& o)const noexcept {
        return value == o.value && dense_idx == o.dense_idx; 
    }
    
    bool operator!=(const NumericStorage& o)const noexcept {
        return !(*this == o);
    }

    friend void swap(NumericStorage& a, NumericStorage& b) noexcept {
        using std::swap;
        swap(a.value, b.value);
        swap(a.dense_idx, b.dense_idx);
    }
};


struct  NumericSparseSetPolicy {
    using key_type = uint32_t;
    using value_type = NumericStorage;

    uint32_t GetIndexFromKey(key_type key) const { return key; }

    uint32_t GetIndexFromValue(value_type value) const { return value.dense_idx; }

    constexpr uint32_t GetInvalidKey() const noexcept {
        return std::numeric_limits<uint32_t>::max();
    }

    constexpr NumericStorage GetInvalidValue() const noexcept {
        return {};
    }

    void RecordDenseIndex(value_type& value, size_t idx) noexcept { value.dense_idx = idx; }

    void ReuseKey(key_type& key) const noexcept {}
};


TEST_CASE("numeric sparse set") {
    using SparseSet = SparseSet<uint32_t, NumericStorage, NumericSparseSetPolicy>;
    SparseSet sparse_set;

    REQUIRE(sparse_set.Dense().empty());
    REQUIRE(sparse_set.Sparse().empty());

    SECTION("insert") {
        sparse_set.Insert(1, NumericStorage{101});
        REQUIRE(sparse_set.Dense().size() == 1);
        REQUIRE(sparse_set.Sparse().size() == 1);
        REQUIRE(sparse_set.Dense()[0] == 1);
        REQUIRE(sparse_set.Sparse()[0][1].dense_idx == 0);
        REQUIRE(sparse_set.Sparse()[0][1].value == 101);

        sparse_set.Insert(2, NumericStorage{102});
        REQUIRE(sparse_set.Dense().size() == 2);
        REQUIRE(sparse_set.Sparse().size() == 1);
        REQUIRE(sparse_set.Dense()[1] == 2);
        REQUIRE(sparse_set.Sparse()[0][2].dense_idx == 1);
        REQUIRE(sparse_set.Sparse()[0][2].value == 102);

        sparse_set.Insert(SparseSet::page_size, NumericStorage{103});
        REQUIRE(sparse_set.Dense().size() == 3);
        REQUIRE(sparse_set.Sparse().size() == 2);
        REQUIRE(sparse_set.Dense()[2] == SparseSet::page_size);
        REQUIRE(sparse_set.Sparse()[1][0].dense_idx == 2);
        REQUIRE(sparse_set.Sparse()[1][0].value == 103);
    }

    SECTION("emplace") {
        sparse_set.Emplace(1, NumericStorage{101});
        REQUIRE(sparse_set.Dense().size() == 1);
        REQUIRE(sparse_set.Sparse().size() == 1);
        REQUIRE(sparse_set.Dense()[0] == 1);
        REQUIRE(sparse_set.Sparse()[0][1].dense_idx == 0);
        REQUIRE(sparse_set.Sparse()[0][1].value == 101);

        sparse_set.Emplace(2, NumericStorage{102});
        REQUIRE(sparse_set.Dense().size() == 2);
        REQUIRE(sparse_set.Sparse().size() == 1);
        REQUIRE(sparse_set.Dense()[1] == 2);
        REQUIRE(sparse_set.Sparse()[0][2].dense_idx == 1);
        REQUIRE(sparse_set.Sparse()[0][2].value == 102);

        sparse_set.Emplace(SparseSet::page_size, NumericStorage{103});
        REQUIRE(sparse_set.Dense().size() == 3);
        REQUIRE(sparse_set.Sparse().size() == 2);
        REQUIRE(sparse_set.Dense()[2] == SparseSet::page_size);
        REQUIRE(sparse_set.Sparse()[1][0].dense_idx == 2);
        REQUIRE(sparse_set.Sparse()[1][0].value == 103);
    }

    sparse_set.Insert(1, NumericStorage{101});
    sparse_set.Insert(9, NumericStorage{102});
    sparse_set.Insert(6, NumericStorage{103});
    sparse_set.Insert(SparseSet::page_size + 2, NumericStorage{104});

    SECTION("get") {
        REQUIRE(sparse_set.Get(1)->dense_idx == 0);
        REQUIRE(sparse_set.Get(1)->value == 101);
        
        REQUIRE(sparse_set.Get(9)->dense_idx == 1);
        REQUIRE(sparse_set.Get(9)->value == 102);
        
        REQUIRE(sparse_set.Get(6)->dense_idx == 2);
        REQUIRE(sparse_set.Get(6)->value == 103);
        
        REQUIRE(sparse_set.Get(SparseSet::page_size + 2)->dense_idx == 3);
        REQUIRE(sparse_set.Get(SparseSet::page_size + 2)->value == 104);
    }
    
    SECTION("contains") {
        REQUIRE(sparse_set.Contains(1));
        REQUIRE(sparse_set.Contains(9));
        REQUIRE(sparse_set.Contains(6));
        REQUIRE(sparse_set.Contains(SparseSet::page_size + 2));
        REQUIRE(sparse_set.Dense().size() == 4);
        REQUIRE(sparse_set.Sparse().size() == 2);
        REQUIRE_FALSE(sparse_set.Contains(2));
        REQUIRE_FALSE(sparse_set.Contains(0));
    }

    SECTION("remove") {
        sparse_set.Remove(1);
        REQUIRE_FALSE(sparse_set.Contains(1));
        REQUIRE(sparse_set.Dense().size() == 4);
        REQUIRE(sparse_set.AliveCount() == 3);

        // remove non-exists value
        REQUIRE_FALSE(sparse_set.Contains(3));
        sparse_set.Remove(3);
        REQUIRE_FALSE(sparse_set.Contains(3));
        REQUIRE(sparse_set.Dense().size() == 4);
        REQUIRE(sparse_set.AliveCount() == 3);

        sparse_set.Remove(9);
        REQUIRE_FALSE(sparse_set.Contains(1));
        REQUIRE(sparse_set.Dense().size() == 4);
        REQUIRE(sparse_set.AliveCount() == 2);
    }

    SECTION("reuse") {
        REQUIRE_FALSE(sparse_set.HasKeyCached());
        // no cached key, can't reuse
        REQUIRE_FALSE(sparse_set.Reuse(NumericStorage{22}));
        sparse_set.Remove(1);
        sparse_set.Remove(6);

        {
            const uint32_t* value = sparse_set.Reuse(NumericStorage{106});
            REQUIRE(value);
            REQUIRE(*value == 6);
            REQUIRE(sparse_set.Contains(6));
            REQUIRE(sparse_set.Dense()[2] == 6);
            REQUIRE(sparse_set.Sparse()[0][6].value == 106);
        }

        {
            const uint32_t* value = sparse_set.Reuse(NumericStorage{107});
            REQUIRE(value);
            REQUIRE(*value == 1);
            REQUIRE(sparse_set.Contains(1));
            REQUIRE(sparse_set.Dense()[3] == 1);
            REQUIRE(sparse_set.Sparse()[0][1].value == 107);
        }
    }
}
