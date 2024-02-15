#include "graphics/rhi/null/adapter.hpp"
#include "graphics/rhi/device.hpp"

namespace nickel::rhi::null {

GPUSupportFeatures AdapterImpl::Features() {
    return {};
}

GPUSupportLimits AdapterImpl::Limits() {
    return {};
}

Device AdapterImpl::RequestDevice() {
    return Device{*this};
}

Adapter::Info AdapterImpl::RequestAdapterInfo() {
    Adapter::Info info;
    info.device = "null";
    info.vendorID = 0;
    info.api = APIPreference::Null;
    return info;
}

}