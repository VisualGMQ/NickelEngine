#pragma once
#include "nickel/common/macro.hpp"

#include <concepts>
#include <numeric>
#include <utility>

namespace nickel::ecs {

template <typename T>
concept is_unsigned_integral =
    (std::is_integral_v<T> && std::is_unsigned_v<T>) ||
    (std::is_enum_v<T> && std::is_integral_v<std::underlying_type_t<T>> &&
     std::is_unsigned_v<std::underlying_type_t<T>>);

template <typename KeyT, typename ValueT, typename Policy>
concept is_sparse_set_policy = requires(Policy t, Policy& policy_ref, ValueT& value_ref, KeyT& key_ref)
{
    {t.GetIndexFromKey(KeyT{})} -> is_unsigned_integral;
    {t.GetIndexFromValue(ValueT{})} -> is_unsigned_integral;
    {t.GetInvalidKey()} -> is_unsigned_integral;
    {t.GetInvalidValue()} -> std::same_as<ValueT>;
    t.RecordDenseIndex(value_ref, size_t{});
    t.ReuseKey(key_ref);
};

template <typename KeyT, typename ValueT,
          typename Policy, size_t PageSize = 1024>
requires is_sparse_set_policy<KeyT, ValueT, Policy>
class SparseSet {
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
        ValueT& new_value = EnsurePage(k)[k % page_size];
        new_value = value;
        m_policy.RecordDenseIndex(new_value, m_alive_count++);
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

    const KeyT* Reuse(ValueT&& value) noexcept {
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

        return &m_dense[m_alive_count];
    }

    const ValueT* Get(const KeyT& key) const noexcept {
        auto k = m_policy.GetIndexFromKey(key);
        size_t page_idx = k / page_size;
        if (page_idx >= m_sparse.size()) {
            return nullptr;
        }

        auto& value = m_sparse[page_idx][k % page_size];
        if (value == m_policy.GetInvalidValue()) {
            return nullptr;
        }
        return &value;
    }

    ValueT* Get(const KeyT& key) noexcept {
        return const_cast<ValueT*>(std::as_const(*this).Get(key));
    }

    void Remove(const KeyT& key) noexcept {
        NICKEL_RETURN_IF_FALSE(m_alive_count != 0);

        auto k = m_policy.GetIndexFromKey(key);

        size_t page_idx = k / page_size;
        if (page_idx >= m_sparse.size()) {
            return;
        }

        size_t page_slot_idx = k % page_size;
        ValueT& value = m_sparse[page_idx][page_slot_idx];
        if (value == m_policy.GetInvalidValue()) {
            return;
        }

        size_t last_key_idx = m_policy.GetIndexFromKey(m_dense[--m_alive_count]);
        size_t idx = m_policy.GetIndexFromValue(value);
        ValueT& last_value =
            m_sparse[last_key_idx / page_size][last_key_idx % page_size];

        value = m_policy.GetInvalidValue();
        m_policy.RecordDenseIndex(last_value, idx);

        using std::swap;
        swap(m_dense[m_alive_count], m_dense[idx]);
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

    auto& Dense() const noexcept { return m_dense; }

    auto& Sparse() const noexcept { return m_sparse; }

    size_t AliveCount() const noexcept { return m_alive_count; }

protected:
    std::vector<KeyT> m_dense;
    std::vector<page_type> m_sparse;
    policy_type m_policy;
    size_t m_alive_count{};

    page_type& EnsurePage(size_t num) noexcept {
        size_t page_idx = num / page_size;

        size_t old_size = m_sparse.size();
        m_sparse.resize(std::max(m_sparse.size(), page_idx + 1));

        for (size_t i = old_size; i < m_sparse.size(); i++) {
            m_sparse[i].fill(m_policy.GetInvalidValue());
        }
        return m_sparse[page_idx];
    }
};

}  // namespace nickel::ecs