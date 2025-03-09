#pragma once
#include "nickel/graphics/lowlevel/device.hpp"
#include "nickel/video/window.hpp"
#include "nickel/common/dllexport.hpp"

#include <memory>

namespace nickel::graphics {

class AdapterImpl;

class NICKEL_API Adapter {
public:
    struct Limits {
        uint64_t min_uniform_buffer_offset_alignment{};
    };
    
    Adapter(const video::Window::Impl& window);
    ~Adapter();

    Adapter(const Adapter&) = delete;
    Adapter(Adapter&&) = delete;
    Adapter& operator=(const Adapter&) = delete;
    Adapter& operator=(Adapter&&) = delete;
    Device GetDevice() const;

    const AdapterImpl& GetImpl() const;
    AdapterImpl& GetImpl();
    const Limits& GetLimits() const;

private:
    std::unique_ptr<AdapterImpl> m_impl;
};

}  // namespace nickel::graphics