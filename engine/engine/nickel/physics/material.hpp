#pragma once
#include "nickel/common/impl_wrapper.hpp"
#include "nickel/physics/enums.hpp"

namespace nickel::physics {

class MaterialImpl;

class Material: public ImplWrapper<MaterialImpl> {
public:
    using ImplWrapper::ImplWrapper;
    
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
};

}  // namespace nickel::physics