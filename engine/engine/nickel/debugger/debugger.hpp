#pragma once
#include "nickel/animation/skeleton.hpp"
#include "nickel/misc/gameobject.hpp"

namespace nickel::debugger {

class DebugManager {
public:
    void EnabelPhysicsDebugDraw(bool);
    bool IsPhysicsDebugDrawEnabled() const;
    
    void EnabelSkeletonDebugDraw(bool);
    bool IsSkeletonDebugDrawEnabled() const;

    void Update();

private:
    bool m_enable_physics_debug_draw{false};
    bool m_enable_skeleton_debug_draw{false};
    bool m_imgui_window_open{true};
    bool m_show_demo_window{false};
    bool m_enable_model_render{true};

    void drawPhysicActors();
    void updateImGui();
    
    void drawSkeleton(const Skeleton&);
    void drawSkeletonRecurse(const Skeleton&, uint32_t bone_idx);
    void drawGOSkeletonRecurse(const GameObject&);
    void enableModelRenderRecurse(GameObject&, bool);
};

}  // namespace nickel::debugger