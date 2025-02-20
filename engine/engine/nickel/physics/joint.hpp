#pragma once
#include "nickel/common/math/math.hpp"

namespace nickel::physics {

class D6JointImpl;

class D6Joint {
public:
    enum class Motion {
        Free,
        Locked,
        Limited,
    };

    struct Limit {
        float m_restitution{};
        float m_bounce_threshold{};
        float m_stiffness{};
        float m_damping{};
    };

    struct LinearLimit : public Limit {
        float m_extent;
    };

    struct AngularLimit : public Limit {
        float m_lower = -PI * 0.5;
        float m_upper = PI * 0.5;
    };

    struct LimitCone : public Limit {
        float m_y_angle = PI * 0.5;
        float m_z_angle = PI * 0.5;
    };

    struct PyramidLimit : public Limit {
        float m_y_limit_min = -PI * 0.5;
        float m_y_limit_max = PI * 0.5;
        float m_z_limit_min = -PI * 0.5;
        float m_z_limit_max = PI * 0.5;
    };

    struct Drive {
        float m_force_limit{};
        float m_stiffness{};
        float m_damping{};
        bool m_is_acceleration = false;
    };

private:
    D6JointImpl* m_impl{};
};

}  // namespace nickel::physics