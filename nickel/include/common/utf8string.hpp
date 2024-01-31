#pragma once

#include <array>
#include <vector>
#include <string>
#include <iostream>

namespace nickel {

class utf8char {
public:
    utf8char(char a, char b, char c, char d) : data_{a, b, c, d} {}

    utf8char() : data_{0} {}

    uint64_t to_uint64() const {
        auto l = len();
        if (l == 1) {
            return data_[0];
        } else if (l == 2) {
            return data_[0] << 8 | data_[1];
        } else if (l == 3) {
            return data_[0] << 16 | data_[1] << 8 | data_[2];
        } else {
            return data_[0] << 24 | data_[1] << 16 | data_[2] << 8 | data_[3];
        }
    }

    int len() const noexcept {
        if (!(data_[0] & 0x80)) {
            return 1;
        }
        if (data_[1] & 0xC0) {
            return 2;
        }
        if (data_[2] & 0xE0) {
            return 3;
        }
        return 4;
    }

    void set(char a, char b, char c, char d) {
        data_[0] = a;
        data_[1] = b;
        data_[2] = c;
        data_[3] = d;
    }

    bool is_white_space() const {
        return len() == 1 &&
               (data_[0] == ' ' || data_[0] == '\t' || data_[0] == '\n');
    }

    auto& operator[](size_t idx) { return data_[idx]; }

    auto& operator[](size_t idx) const { return data_[idx]; }

private:
    std::array<char, 4> data_;
};

class utf8string final {
public:
    using utf8_container = std::vector<utf8char>;
    using iterator = utf8_container::iterator;
    using const_iterator = utf8_container::const_iterator;

    utf8string() = default;
    utf8string(const utf8string&);
    explicit utf8string(const std::string& s);
    explicit utf8string(const char* s);

    inline void clear() { data_.clear(); }

    inline size_t size() const { return data_.size(); }

    inline utf8char* data() { return data_.data(); }

    void insert(const const_iterator& it, const utf8char& c);
    void insert(const const_iterator& it, const char* s);
    void insert(const const_iterator& it, const std::string& s);
    void insert(const const_iterator& it, const utf8string& s);

    void erase(const const_iterator& it);

    void push_back(utf8char c) { data_.push_back(c); }

    /* you must ensure that s contains complete UTF8 codes */
    void push_back(const std::string& s);
    void push_back(const char* s);
    void push_back(const utf8string& s);

    inline void pop_back() { data_.pop_back(); }

    inline utf8char& back() { return data_.back(); }

    inline const utf8char& at(size_t idx) const { return data_.at(idx); }

    inline auto begin() const { return data_.begin(); }

    inline auto end() const { return data_.end(); }

    inline const utf8char& operator[](size_t idx) const { return data_[idx]; }

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
    utf8_container data_;
};

std::string utf8char2string(const utf8char& c);

}  // namespace nickel