#include "physics/world.hpp"
#include "gecs/entity/querier.hpp"

namespace nickel {

namespace physics {

void World::collide(gecs::querier<Body, CollideShape> bodies) {
    std::vector<std::vector<std::unique_ptr<Contact>>> contacts;

    for (auto it1 = bodies.begin(); it1 != bodies.end(); it1++) {
        auto& b1 = std::get<1>(*it1);
        auto& s1 = std::get<2>(*it1);
        contacts.emplace_back();
        for (auto it2 = it1 + 1; it2 != bodies.end(); it2++) {
            auto& b2 = std::get<1>(*it2);
            auto& s2 = std::get<2>(*it2);

            auto contact = manifoldSolver_.GetContact(s1, s2);
            contact->Evaluate(s1, s2, Transform::FromTranslation(b1.pos), Transform::FromTranslation(b2.pos));
            contacts.back().push_back(std::move(contact));
        }
    }

    // handle contacts
    for (auto& contactList : contacts) {
        for (auto& contact : contactList) {
            // handle contacts
        }
    }
}

void World::Step(Real interval, Body& body) {
    physicSolver_.Step(interval, body);
}

void PhysicsInit(gecs::commands cmds) {
    cmds.emplace_resource<World>();
}

void PhysicsUpdate(gecs::resource<gecs::mut<World>> world,
                   gecs::querier<gecs::mut<Body>> bodies) {
    for (auto&& [_, body] : bodies) {
        for (auto& forceGen : world->forceGenerators) {
            forceGen(body);
        }
        world->Step(0.3, body);
    }
}

}  // namespace physics

}  // namespace nickel