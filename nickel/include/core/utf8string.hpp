#pragma once

#include "pch.hpp"

namespace nickel {

class utf8string final {
public:
    using OneUTF8 = std::array<char, 4>;
    using UTF8Container = std::vector<OneUTF8>;
    using iterator = UTF8Container::iterator;
    using const_iterator = UTF8Container::const_iterator;

    utf8string() = default;
    utf8string(const utf8string&);
    explicit utf8string(const std::string& s);
    explicit utf8string(const char* s);

    inline void clear() { data_.clear(); }

    inline size_t size() const { return data_.size(); }

    inline OneUTF8* data() { return data_.data(); }

    void insert(const const_iterator& it, const OneUTF8& c);
    void insert(const const_iterator& it, const char* s);
    void insert(const const_iterator& it, const std::string& s);
    void insert(const const_iterator& it, const utf8string& s);

    void erase(const const_iterator& it);

    void push_back(OneUTF8 c) { data_.push_back(c); }

    /* you must ensure that s contains complete UTF8 codes */
    void push_back(const std::string& s);
    void push_back(const char* s);
    void push_back(const utf8string& s);

    inline void pop_back() { data_.pop_back(); }

    inline OneUTF8& back() { return data_.back(); }

    inline const OneUTF8& at(size_t idx) const { return data_.at(idx); }

    inline auto begin() const { return data_.begin(); }

    inline auto end() const { return data_.end(); }

    inline const OneUTF8& operator[](size_t idx) const { return data_[idx]; }

    inline bool empty() const { return data_.empty(); }

    inline utf8string operator+(const utf8string& s) const {
        utf8string result(*this);
        result.push_back(s);
        return result;
    }

    inline utf8string operator+(const char* s) const {
        utf8string result(*this);
        result.push_back(s);
        return result;
    }

    inline utf8string operator+(const std::string& s) const {
        utf8string result(*this);
        result.push_back(s);
        return result;
    }

    inline utf8string& operator+=(const char* s) {
        this->push_back(s);
        return *this;
    }

    inline utf8string& operator+=(const std::string& s) {
        this->push_back(s);
        return *this;
    }

    inline utf8string& operator+=(const utf8string& s) {
        this->push_back(s);
        return *this;
    }

    inline utf8string& operator=(const char* s) {
        clear();
        push_back(s);
        return *this;
    }

    inline utf8string& operator=(const std::string& s) {
        clear();
        push_back(s);
        return *this;
    }

    std::string to_string() const;

private:
    UTF8Container data_;
};

std::string UTF8ToString(const utf8string::OneUTF8& c);

}  // namespace nickel