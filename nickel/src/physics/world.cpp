#include "physics/world.hpp"
#include "gecs/entity/querier.hpp"

namespace nickel {

namespace physics {

void World::collide(gecs::querier<gecs::mut<Body>, CollideShape> bodies,
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
    for (auto& contactList : contacts) {
        for (auto& contact : contactList) {
            auto& manifold = contact->GetManifold();
            if (manifold.pointCount == 0) {
                continue;
            }

            // handle contacts
            auto b1 = contact->GetBody1();
            auto b2 = contact->GetBody2();
            Vec2 mtv;
            mtv = manifold.normal * manifold.depth;
            dealContact(mtv, manifold.tangent, *b1, *b2);

            renderer.DrawCircle(manifold.points[0], 5, {1, 0, 1, 1}, 10);
            renderer.DrawLine(
                manifold.points[0],
                manifold.points[0] + manifold.normal * manifold.depth,
                {0, 1, 1, 1});
        }
    }
}

void World::dealContact(const Vec2& mtv, const Vec2& tangent, Body& b1,
                        Body& b2, bool shouldRecurse) {
    if (b1.type == Body::Type::Dynamic) {
        switch (b2.type) {
            case Body::Type::Static:
                b1.pos += mtv;
                b1.vel = b1.vel.Dot(tangent) * tangent;
                break;
            case Body::Type::Dynamic:
                b1.pos += mtv * 0.5;
                b2.pos -= mtv * 0.5;
                b1.vel = b1.vel.Dot(tangent) * tangent;
                b2.vel = b2.vel.Dot(tangent) * tangent;
                break;
            case Body::Type::Kinematic:
                b1.pos += mtv;
                break;
        }
    } else if (shouldRecurse) {
        dealContact(-mtv, tangent, b2, b1, false);
    }
}

void World::Step(Real interval,
                 gecs::querier<gecs::mut<Body>, CollideShape> querier,
                 gecs::resource<gecs::mut<Renderer2D>> renderer) {
    for (auto&& [_, body, shape] : querier) {
        physicSolver_.Step(interval, body);
    }
    collide(querier, renderer.get());
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