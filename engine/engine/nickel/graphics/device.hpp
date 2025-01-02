#pragma once

#include "nickel/common/math/smatrix.hpp"

namespace nickel::graphics {

class DeviceImpl;
class AdapterImpl;

class Device {
public:
    explicit Device(const AdapterImpl& adapter_impl,
                    const SVector<uint32_t, 2>& window_size);
    ~Device();

    DeviceImpl& Impl();
    const DeviceImpl& Impl() const;
    
private:
    std::unique_ptr<DeviceImpl> m_impl;
};

}