#include "graphics/rhi/null/adapter.hpp"
#include "graphics/rhi/device.hpp"

namespace nickel::rhi::null {

GPUSupportFeatures AdapterImpl::Features() {
    return {};
}

const GPUSupportLimits& AdapterImpl::Limits() const {
    return limits_;
}

Device AdapterImpl::RequestDevice() {
    return Device{*this};
}

Adapter::Info AdapterImpl::RequestAdapterInfo() const {
    Adapter::Info info;
    info.device = "null";
    info.vendorID = 0;
    info.api = APIPreference::Null;
    return info;
}

}