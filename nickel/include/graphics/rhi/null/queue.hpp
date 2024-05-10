#pragma once

#include "graphics/rhi/impl/queue.hpp"

namespace nickel::rhi::null {

class QueueImpl: public rhi::QueueImpl {
public:
    void Submit(const std::vector<CommandBuffer>&) override;
};

}