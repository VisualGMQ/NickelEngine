#include "physics/world.hpp"
#include "gecs/entity/querier.hpp"

namespace nickel {

namespace physics {

void World::collide(Real interval,
                    gecs::querier<gecs::mut<Body>, CollideShape> bodies,
                    Renderer2D& renderer) {
    std::vector<std::vector<std::unique_ptr<Contact>>> contacts;

    for (auto it1 = bodies.begin(); it1 != bodies.end(); it1++) {
        auto& b1 = std::get<1>(*it1);
        auto& s1 = std::get<2>(*it1);
        contacts.emplace_back();
        for (auto it2 = it1 + 1; it2 != bodies.end(); it2++) {
            auto& b2 = std::get<1>(*it2);
            auto& s2 = std::get<2>(*it2);

            auto contact = manifoldSolver_.GetContact(s1, s2);
            contact->Evaluate(s1, s2, &b1, &b2);
            contacts.back().push_back(std::move(contact));
        }
    }

    // handle contacts
    for (int i = 0; i < 5; i++) {
        for (auto& contactList : contacts) {
            for (auto& contact : contactList) {
                auto& manifold = contact->GetManifold();
                if (manifold.pointCount == 0) {
                    continue;
                }
                contact->ApplyImpulse();

                auto b1 = contact->GetBody1();
                auto b2 = contact->GetBody1();

                renderer.DrawCircle(manifold.points[0], 5, {1, 0, 1, 1}, 10);
                renderer.DrawLine(
                    manifold.points[0],
                    manifold.points[0] + manifold.normal * manifold.depth,
                    {0, 1, 1, 1});
            }
        }
    }
}

void World::Step(Real interval,
                 gecs::querier<gecs::mut<Body>, CollideShape> querier,
                 gecs::resource<gecs::mut<Renderer2D>> renderer) {
    for (auto&& [_, body, shape] : querier) {
        body.acc = body.force * interval;
        body.vel += body.acc * interval;

        if (body.vel.LengthSqrd() > 100* 100) {
            body.vel = cgmath::Normalize(body.vel) * 100;
        }

        body.force = Vec2{0, 0};
    }

    collide(interval, querier, renderer.get());

    for (auto&& [_, body, shape] : querier) {
        body.pos += body.vel * interval;
    }
}

void PhysicsInit(gecs::commands cmds) {
    cmds.emplace_resource<World>();
}

void PhysicsUpdate(gecs::resource<gecs::mut<World>> world,
                   gecs::querier<gecs::mut<Body>, CollideShape> querier,
                   gecs::resource<gecs::mut<Renderer2D>> renderer) {
    for (auto&& [_, body, shape] : querier) {
        for (auto& forceGen : world->forceGenerators) {
            if (body.type == Body::Type::Dynamic) {
                forceGen(body);
            }
        }
    }

    world->Step(0.3, querier, renderer);
}

}  // namespace physics

}  // namespace nickel