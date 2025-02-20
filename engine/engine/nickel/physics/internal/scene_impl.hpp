#pragma once
#include "nickel/common/memory/memory.hpp"
#include "nickel/common/memory/refcountable.hpp"
#include "nickel/physics/internal/pch.hpp"
#include "nickel/physics/rigidbody.hpp"

namespace nickel::physics {

class ContextImpl;

class SceneImpl : public RefCountable {
public:
    SceneImpl(const std::string& name, ContextImpl* ctx, physx::PxScene*);
    ~SceneImpl();

    void DecRefcount() override;

    void AddRigidActor(RigidActor&);
    void Simulate(float delta_time) const;

    physx::PxScene* m_scene;
    
private:
    ContextImpl* m_ctx;
};

}  // namespace nickel::physics