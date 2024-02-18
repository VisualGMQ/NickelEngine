#pragma once
#include "graphics/rhi/impl/queue.hpp"

namespace nickel::rhi::gl4 {

class QueueImpl: public rhi::QueueImpl {
public:
    void Submit(const std::vector<CommandBuffer>&) override;
};

}