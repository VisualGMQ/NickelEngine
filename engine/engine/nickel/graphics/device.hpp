#pragma once

#include "adapter.hpp"

namespace nickel::graphics {

class Device {
public:
    class Impl;

    explicit Device(const Adapter::Impl& ctx_impl);
    ~Device();
    
private:
    std::unique_ptr<Impl> m_impl;
};

}