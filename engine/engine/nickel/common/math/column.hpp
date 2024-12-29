#pragma once
#include <type_traits>

namespace nickel {

template <typename T, bool IsConst = true>
class Column {
public:
    using ElemType = T;

    Column(ElemType& datas, size_t len)
    requires(!IsConst)
        : datas_{&datas} {
#ifdef NICKEL_DEBUG
        len_ = len;
#endif
    }

    Column(const ElemType& datas, size_t len)
    requires(IsConst)
        : datas_{&datas} {
#ifdef NICKEL_DEBUG
        len_ = len;
#endif
    }

    auto operator[](size_t idx) const noexcept {
        NICKEL_ASSERT(idx < len_, "Column index out of range");
        return datas_[idx];
    }

    auto& operator[](size_t idx) noexcept
    requires(!IsConst)
    {
        NICKEL_ASSERT(idx < len_, "Column index out of range");
        return datas_[idx];
    }

private:
    std::conditional_t<IsConst, const ElemType*, ElemType*> datas_{};

#ifdef NICKEL_DEBUG
    size_t len_{};
#endif
};

}  // namespace nickel