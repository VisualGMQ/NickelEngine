#pragma once
#include <memory>
#include <string>

namespace nickel::video {

class Window {
public:
    class Impl;
    
    Window(const std::string& title, int w, int h);
    Impl& GetImpl();
    const Impl& GetImpl() const;
    ~Window();
    
private:
    std::unique_ptr<Impl> m_impl;
};

}