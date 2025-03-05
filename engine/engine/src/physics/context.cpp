#include "nickel/physics/context.hpp"
#include "nickel/physics/internal/context_impl.hpp"

namespace nickel::physics {

Context::Context() : m_impl{std::make_unique<ContextImpl>()} {}

Context::~Context() {}

Scene Context::CreateScene(const std::string& name, const Vec3& gravity) {
    return m_impl->CreateScene(name, gravity);
}

Material Context::CreateMaterial(float static_friction, float dynamic_friction,
                                 float restitution) {
    return m_impl->CreateMaterial(static_friction, dynamic_friction,
                                  restitution);
}

RigidStatic Context::CreateRigidStatic(const Vec3& p, const Quat& q) {
    return m_impl->CreateRigidStatic(p, q);
}

RigidDynamic Context::CreateRigidDynamic(const Vec3& p, const Quat& q) {
    return m_impl->CreateRigidDynamic(p, q);
}

TriangleMesh Context::CreateTriangleMesh(std::span<const Vec3> vertices,
                                         std::span<const uint32_t> indices) {
    return m_impl->CreateTriangleMesh(vertices, indices);
}

Shape Context::CreateShape(const SphereGeometry& g, const Material& mtl) {
    return m_impl->CreateShape(g, mtl);
}

Shape Context::CreateShape(const BoxGeometry& g, const Material& mtl) {
    return m_impl->CreateShape(g, mtl);
}

Shape Context::CreateShape(const CapsuleGeometry& g, const Material& mtl) {
    return m_impl->CreateShape(g, mtl);
}

Shape Context::CreateShape(const TriangleMeshGeometry& g, const Material& mtl) {
    return m_impl->CreateShape(g, mtl);
}

Shape Context::CreateShape(const PlaneGeometry& g, const Material& mtl) {
    return m_impl->CreateShape(g, mtl);
}

Scene Context::GetMainScene() {
    return m_impl->GetMainScene();
}

void Context::Update(float delta_time) {
    m_impl->Update(delta_time);
}

void Context::GC() {
    m_impl->GC();
}

const ContextImpl* Context::GetImpl() const {
    return m_impl.get();
}

ContextImpl* Context::GetImpl() {
    return m_impl.get();
}

}  // namespace nickel::physics