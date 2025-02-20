#pragma once

namespace nickel::physics {

class ContextImpl;
class SceneImpl;

class Scene {
public:
    Scene(SceneImpl*);
    Scene(const Scene&);
    Scene(Scene&&) noexcept;
    Scene& operator=(const Scene&);
    Scene& operator=(Scene&&) noexcept;
    ~Scene();

    void Simulate(float delta_time) const;

    operator bool() const;

    const SceneImpl* GetImpl() const;
    SceneImpl* GetImpl();

private:
    SceneImpl* m_impl{};
};

}  // namespace nickel::physics