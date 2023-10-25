#pragma once

#include "physics/manifold.hpp"

namespace nickel {

namespace physics {

class ManifoldSolver final {
public:
    std::unique_ptr<Contact> GetContact(const CollideShape&, const CollideShape&);

private:
};

}

}