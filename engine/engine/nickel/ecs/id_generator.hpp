#pragma once

namespace nickel::ecs {

class IDGenerator {
public:
    template <typename>
    static auto GenID() noexcept {
        static auto id = m_id++;
        return id;
    }
    
private:
    static uint32_t m_id;
};

}