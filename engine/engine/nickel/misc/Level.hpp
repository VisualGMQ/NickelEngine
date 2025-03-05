#pragma once
#include "nickel/misc/gameobject.hpp"

namespace nickel {

class Level {
public:
    GameObject& GetRootGO() { return m_root_go; }

    void Update();

private:
    GameObject m_root_go;

    void preorderGO(GameObject* parent, GameObject& go);
    void debugDrawRigidActor(const GameObject& go);
};

}  // namespace nickel