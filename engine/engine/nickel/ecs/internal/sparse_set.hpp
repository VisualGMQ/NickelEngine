#pragma once
#include "nickel/common/macro.hpp"

#include <concepts>
#include <numeric>

namespace nickel::ecs {
template <typename KeyT, typename ValueT>
requires std::is_integral_v<KeyT> && std::is_integral_v<ValueT> &&
         std::is_unsigned_v<ValueT> && std::is_unsigned_v<KeyT>
class NumericSparseSetPolicy {
    using numeric_type = std::common_type_t<KeyT, ValueT>;

    numeric_type GetIndexFromKey(KeyT key) const { return key; }

    numeric_type GetIndexFromValue(ValueT value) const { return value; }

    consteval numeric_type GetInvalidKey() const noexcept {
        return std::numeric_limits<numeric_type>::max();
    }

    consteval numeric_type GetInvalidValue() const noexcept {
        return std::numeric_limits<numeric_type>::max();
    }

    void RecordDenseIndex(ValueT& value, size_t idx) noexcept { value = idx; }

    void ReuseKey(KeyT& key) const noexcept {}
};

template <typename KeyT, typename ValueT,
          typename Policy = NumericSparseSetPolicy<KeyT, ValueT>,
          size_t PageSize = 1024>
class SparseSet<KeyT, ValueT, Policy, PageSize> {
public:
    using key_type = KeyT;
    using value_type = ValueT;
    using policy_type = Policy;
    using page_type = std::array<ValueT, PageSize>;
    static constexpr size_t page_size = PageSize;

    template <typename... Args>
    SparseSet(Args&&... args) : m_policy{std::forward<Args>(args)...} {}

    void Insert(const KeyT& key, const ValueT& value) noexcept {
        auto k = m_policy.GetIndexFromKey(key);

        m_dense.push_back(key);
        using std::swap;
        if (m_alive_count > 0) {
            swap(m_dense[m_alive_count], m_dense.back());
        }
        m_policy.RecordDenseIndex(value, m_alive_count++);
        EnsurePage(k)[k % page_size] = value;
    }

    void Emplace(KeyT&& key, ValueT&& value) noexcept {
        auto k = m_policy.GetIndexFromKey(key);

        m_dense.emplace_back(std::move(key));
        using std::swap;
        if (m_alive_count > 0) {
            swap(m_dense[m_alive_count], m_dense.back());
        }
        m_policy.RecordDenseIndex(value, m_alive_count++);
        EnsurePage(k)[k % page_size] = std::move(value);
    }

    const KeyT* Reuse(const ValueT&& value) noexcept {
        const KeyT* key = GetCachedKey();
        if (!key) {
            return nullptr;
        }

        auto k = m_policy.GetIndexFromKey(*key);
        m_policy.RecordDenseIndex(value, m_alive_count++);
        EnsurePage(k)[k % page_size] = std::move(value);

        return key;
    }

    bool HasKeyCached() const noexcept {
        return m_alive_count < m_dense.size();
    }

    const KeyT* GetCachedKey() const noexcept {
        if (m_alive_count >= m_dense.size()) {
            return nullptr;
        }

        return m_dense[m_alive_count];
    }

    void Remove(const KeyT& key) noexcept {
        NICKEL_RETURN_IF_FALSE(m_alive_count != 0);

        auto k = m_policy.GetIndexFromKey(key);

        size_t page_idx = k / page_size;
        if (page_idx >= m_sparse.size()) {
            return;
        }

        size_t page_slot_idx = k % page_size;
        const ValueT& value = m_sparse[page_idx][page_slot_idx];
        if (value == m_policy.GetInvalidValue()) {
            return;
        }

        size_t last_key_idx = --m_alive_count;
        size_t idx = m_policy.GetIndexFromValue(value);
        const KeyT& last_key = m_dense[last_key_idx];
        const ValueT& last_value =
            m_sparse[last_key_idx / page_size][last_key_idx % page_size];
        value = std::move(last_value);
        last_value = m_policy.GetInvalidValue();

        using std::swap;
        swap(m_dense.back(), m_dense[idx]);
    }

    bool Contains(const KeyT& key) const noexcept {
        uint32_t k = m_policy.GetIndexFromKey(key);
        size_t page_idx = k / page_size;
        size_t page_slot_idx = k % page_size;

        if (page_idx >= m_sparse.size()) {
            return false;
        }

        const ValueT& value = m_sparse[page_idx][page_slot_idx];
        return value != m_policy.GetInvalidValue();
    }

protected:
    std::vector<KeyT> m_dense;
    std::vector<page_type> m_sparse;
    policy_type m_policy;
    size_t m_alive_count{};

    page_type& EnsurePage(size_t num) noexcept {
        size_t page_idx = num / page_size;

        m_sparse.resize(std::max(m_sparse.size(), page_idx + 1));
        return m_sparse[page_idx];
    }
};

}  // namespace nickel::ecs