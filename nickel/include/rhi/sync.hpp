#pragma once

namespace nickel::rhi {

class Semaphore {
public:
    virtual ~Semaphore() = default;
};

class Fence {
public:
    virtual ~Fence() = default;
};

class Event {
public:
    virtual ~Event() = default;
};

}