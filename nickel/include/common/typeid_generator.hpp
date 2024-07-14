#pragma once
#include "stdpch.hpp"

namespace nickel {

struct TypeIDGenerator {
public:
    template <typename T>
    static uint32_t GetID() {
        static uint32_t id = id_++;
        return id;
    }

private:
    static uint32_t id_;
};

}  // namespace nickel