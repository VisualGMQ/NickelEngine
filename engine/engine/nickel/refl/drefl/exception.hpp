#pragma once

#include <exception>
#include <stdexcept>

namespace nickel::refl {

class BadAnyAccess: public std::logic_error {
public:
    using std::logic_error::logic_error;
};

class AnyNoncopyable: public std::logic_error {
public:
    using std::logic_error::logic_error;
};

}