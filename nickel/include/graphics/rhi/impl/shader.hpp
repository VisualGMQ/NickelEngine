#pragma once

namespace nickel::rhi {

class DeviceImpl;

class ShaderModuleImpl {
public:
    virtual void Destroy(DeviceImpl&) = 0;

    virtual ~ShaderModuleImpl() = default;
};

}