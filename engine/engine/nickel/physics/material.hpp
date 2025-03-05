#pragma once
#include "nickel/common/impl_wrapper.hpp"
#include "nickel/physics/enums.hpp"

namespace nickel::physics {

class MaterialImpl;

class Material {
public:
    Material() = default;
    Material(MaterialImpl* impl);
    Material(const Material& o);
    Material(Material&& o) noexcept;
    Material& operator=(const Material& other);
    Material& operator=(Material&& other) noexcept;
    ~Material();

    void SetDynamicFriction(float friction);
    void SetStaticFriction(float friction);
    void SetRestitution(float);
    void SetDamping(float);

    float GetDamping() const;
    float GetStaticFriction() const;
    float GetDynamicFriction() const;
    float GetRestitution() const;

    void SetFrictionCombineMode(CombineMode);
    void SetRestitutionCombineMode(CombineMode);
    void SetDampingCombineMode(CombineMode);

    MaterialImpl* GetImpl();
    const MaterialImpl* GetImpl() const;

private:
    MaterialImpl* m_impl{};
};

}  // namespace nickel::physics