#pragma once

namespace nickel::rhi {

class Buffer {
public:
    virtual ~Buffer() = default;
};

class DeviceMemory {
public:
    virtual ~DeviceMemory() = default;
};

}