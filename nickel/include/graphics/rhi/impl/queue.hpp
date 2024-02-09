#pragma once

#include "graphics/rhi/command.hpp"
namespace nickel::rhi {

class QueueImpl {
public:
    virtual void Submit(const std::vector<CommandBuffer>&) = 0;
    virtual ~QueueImpl() = default;
};

}