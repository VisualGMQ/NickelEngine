#include "physics/manifold_solver.hpp"
#include "physics/manifold.hpp"
#include "physics/shape.hpp"

namespace nickel {

namespace physics {


std::unique_ptr<Contact> ManifoldSolver::GetContact(const CollideShape& shape1, const CollideShape& shape2, bool shouldRecurse) {
    auto& s1 = *shape1.shape;
    auto& s2 = *shape2.shape;
    auto type1 = s1.GetType();
    auto type2 = s2.GetType();

    if (type1 == Shape::Type::Circle) {
        if (type2 == Shape::Type::Circle) {
            return std::make_unique<CirclesContact>();
        } else if (type2 == Shape::Type::OBB) {
            return std::make_unique<CircleAABBContact>();
        }
        // TODO: not finish
    }

    if (shouldRecurse) {
        return GetContact(shape2, shape1, false);
    }

    return nullptr;
}

}

}