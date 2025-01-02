#pragma once
#include "nickel/video/window.hpp"

#include <memory>

namespace nickel::graphics {

class AdapterImpl;

class Adapter {
public:
    Adapter(const video::Window::Impl& window);
    ~Adapter();

    const AdapterImpl& GetImpl() const;
    
private:
    std::unique_ptr<AdapterImpl> m_impl;
};

}