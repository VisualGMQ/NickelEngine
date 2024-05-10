#pragma once
#include "graphics/rhi/impl/queue.hpp"

namespace nickel::rhi::gl4 {

class DeviceImpl;

class QueueImpl: public rhi::QueueImpl {
public:
    explicit QueueImpl(DeviceImpl&);

    void Submit(const std::vector<CommandBuffer>&) override;

private:
    DeviceImpl& device_;
};

}