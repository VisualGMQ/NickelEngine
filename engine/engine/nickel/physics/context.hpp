#pragma once

namespace nickel::physics {

class ContextImpl;

class Context {
public:
    Context();
    ~Context();
    
private:
    std::unique_ptr<ContextImpl> m_impl;
};

}