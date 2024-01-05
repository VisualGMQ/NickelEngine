#pragma once

#include "core/log.hpp"
#include <fstream>
#include <optional>
#include <string>

#include "core/singlton.hpp"

namespace nickel {

/**
 * @addtogroup utilities
 * @{
 */

/**
 * @brief read whole file content
 * @param filename
 * @return file content. Read file failed will return `std::nullopt`
 */
template <typename T = std::string, typename PathT>
inline std::optional<T> ReadWholeFile(const PathT& filename) {
    std::ifstream file(filename);
    if (file.fail()) {
        LOGE("file %s open failed", filename);
        return std::nullopt;
    }
    T content(std::istreambuf_iterator<char>(file),
                        (std::istreambuf_iterator<char>()));
    return content;
}

/**
 * @brief eazy use random utility
 */
class Random final : public Singlton<Random, true> {
public:
    Random() : seed_(time(nullptr)) { num_ = seed_; }

    explicit Random(int seed) : seed_(seed) { num_ = seed_; }

    std::uint32_t Rand() {
        num_ = (a_ * num_ + b_) % m_;
        return num_;
    }

    std::uint32_t RandRange(int low, int high) {
        return Rand() % (high - low) + low;
    }

    std::uint32_t Seed() const { return seed_; }

private:
    std::uint32_t a_ = 48271, b_ = 2312,
                  m_ = std::numeric_limits<uint32_t>::max();
    std::uint32_t seed_;
    std::uint32_t num_;
};

inline bool IsWhiteSpace(char c) {
    return c == ' ' || c == '\t' || c == '\n';
}

/**
 * @brief CSV iterator, use to get elements in CSV format
 */
template <typename CharList>
class CSVIterator final {
public:
    explicit CSVIterator(const CharList& data): data_{&data} {
        nextToken();
    }
    CSVIterator() = default;

    bool operator==(const CSVIterator& o) const {
        if (!o.data_ && !data_) {
            return true;
        }

        if (data_) {
            if (o.data_) {
                return o.data_ == data_ && begin_ == o.begin_ && end_ == o.end_;
            } else {
                return begin_ >= data_->size();
            }
        } else {
            return o == *this;
        }
    }

    bool operator!=(const CSVIterator& o) const {
        return !(*this == o);
    }

    auto& operator++() {
        nextToken();
        return *this;
    }

    auto operator++(int) {
        auto backup = *this;
        nextToken();
        return backup;
    }

    std::string_view operator*() const {
        return std::string_view{data_->data() + begin_, end_ - begin_};
    }

private:
    size_t begin_ = -1, end_ = -1;
    const CharList* data_ = nullptr;

    void nextToken() {
        if (!data_) {
            return;
        }

        begin_ = end_ + 1;        
        while (begin_ < data_->size() && IsWhiteSpace((*data_)[begin_])) {
            begin_ ++;
        }
        end_ = begin_ + 1;
        while (end_ < data_->size() && (*data_)[end_] != ',') {
            end_ ++;
        }
    }
};


/**
 * @}
 */

}  // namespace nickel