#pragma once
#include "nickel/graphics/internal/refcountable.hpp"

namespace nickel::graphics {

class CommandPoolImpl;

class CommandImpl : public RefCountable {
public:
    explicit CommandImpl(VkDevice device, CommandPoolImpl& pool,
                         VkCommandBuffer cmd);
    ~CommandImpl();

    VkCommandBuffer m_cmd;

private:
    VkDevice m_device;
    CommandPoolImpl& m_pool;
};

}  // namespace nickel::graphics