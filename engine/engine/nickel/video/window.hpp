#pragma once
#include "nickel/common/math/smatrix.hpp"

#include <memory>
#include <string>

namespace nickel::video {

class Window {
public:
    class Impl;
    
    Window(const std::string& title, int w, int h);
    ~Window();

    Impl& GetImpl();
    const Impl& GetImpl() const;
    SVector<uint32_t, 2> GetSize() const noexcept;

    bool IsMinimize() const noexcept;
    
private:
    std::unique_ptr<Impl> m_impl;
};

}