#pragma once
#include "nickel/video/window.hpp"

#include <memory>

namespace nickel::graphics {

class Adapter {
public:
    class Impl;

    Adapter(const video::Window::Impl& window);
    ~Adapter();

    const Impl& GetImpl() const;
    
private:
    std::unique_ptr<Impl> m_impl;
};

}