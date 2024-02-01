#include "common/utf8string.hpp"

namespace nickel {

utf8string::utf8string(const utf8string& s) {
    push_back(s);
}

utf8string::utf8string(const char* s) {
    push_back(s);
}

utf8string::utf8string(const std::string& s) {
    push_back(s);
}

void utf8string::insert(const const_iterator& it, const utf8char& c) {
    data_.insert(it, c);
}

void utf8string::insert(const const_iterator& it, const char* s) {
    utf8string str(s);
    data_.insert(it, str.data_.begin(), str.data_.end());
}

void utf8string::insert(const const_iterator& it, const std::string& s) {
    utf8string str(s);
    data_.insert(it, str.data_.begin(), str.data_.end());
}

void utf8string::insert(const const_iterator& it, const utf8string& s) {
    data_.insert(it, s.begin(), s.end());
}

void utf8string::erase(const const_iterator& it) {
    data_.erase(it);
}

void utf8string::push_back(const std::string& s) {
    push_back(s.c_str());
}

void utf8string::push_back(const utf8string& s) {
    for (const auto& c : s) data_.push_back(c);
}

void utf8string::push_back(const char* s) {
    if (!s || strlen(s) == 0) return;

    int idx = 0;
    int len = strlen(s);
    while (idx < len) {
        utf8char c;
        if ((s[idx] & 0xF0) == 0xF0) {
            c[0] = s[idx++];
            c[1] = s[idx++];
            c[2] = s[idx++];
            c[3] = s[idx++];
            data_.push_back(c);
        } else if ((s[idx] & 0xE0) == 0xE0) {
            c[0] = s[idx++];
            c[1] = s[idx++];
            c[2] = s[idx++];
            data_.push_back(c);
        } else if ((s[idx] & 0xC0) == 0xC0) {
            c[0] = s[idx++];
            c[1] = s[idx++];
            data_.push_back(c);
        } else if (s[idx] >= 0) {
            c[0] = s[idx++];
            data_.push_back(c);
        } else {
            idx++;
        }
    }
}

std::string utf8string::to_string() const {
    std::string result;
    for (auto& c : data_) {
        if ((c[0] & 0xF0) == 0xF0) {
            result.push_back(c[0]);
            result.push_back(c[1]);
            result.push_back(c[2]);
            result.push_back(c[3]);
        } else if ((c[0] & 0xE0) == 0xE0) {
            result.push_back(c[0]);
            result.push_back(c[1]);
            result.push_back(c[2]);
        } else if ((c[0] & 0xC0) == 0xC0) {
            result.push_back(c[0]);
            result.push_back(c[1]);
        } else if (c[0] >= 0) {
            result.push_back(c[0]);
        }
    }
    return result;
}

std::string utf8char2string(const utf8char& c) {
    std::string result;
    if ((c[0] & 0xF0) == 0xF0) {
        result.push_back(c[0]);
        result.push_back(c[1]);
        result.push_back(c[2]);
        result.push_back(c[3]);
    } else if ((c[0] & 0xE0) == 0xE0) {
        result.push_back(c[0]);
        result.push_back(c[1]);
        result.push_back(c[2]);
    } else if ((c[0] & 0xC0) == 0xC0) {
        result.push_back(c[0]);
        result.push_back(c[1]);
    } else if (c[0] >= 0) {
        result.push_back(c[0]);
    }
    return result;
}

std::ostream& operator<<(std::ostream& o, const utf8char& c) {
    if ((c[0] & 0xF0) == 0xF0) {
        o << c[0] << c[1] << c[2] << c[3];
    } else if ((c[0] & 0xE0) == 0xE0) {
        o << c[0] << c[1] << c[2];
    } else if ((c[0] & 0xC0) == 0xC0) {
        o << c[0] << c[1];
    } else if (c[0] >= 0) {
        o << c[0];
    }
    return o;
}

std::ostream& operator<<(std::ostream& o, const utf8string& s) {
    for (const auto& c : s) o << c;
    return o;
}

}  // namespace nickel