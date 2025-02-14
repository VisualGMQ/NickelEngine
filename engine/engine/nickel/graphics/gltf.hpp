#pragma once
#include "nickel/common/memory/memory.hpp"
#include "nickel/fs/path.hpp"

namespace nickel::graphics {

class GLTFModelImpl;

struct GLTFModel final {
    GLTFModel() = default;
    GLTFModel(GLTFModelImpl* impl);
    GLTFModel(const GLTFModel&) noexcept;
    GLTFModel(GLTFModel&&) noexcept;
    GLTFModel& operator=(const GLTFModel&) noexcept;
    GLTFModel& operator=(GLTFModel&&) noexcept;
    ~GLTFModel();
    
    operator bool() const;

    const GLTFModelImpl* GetImpl() const;
    GLTFModelImpl* GetImpl();

private:
    GLTFModelImpl* m_impl{};
};

class GLTFManager {
public:
    GLTFManager();
    ~GLTFManager();
    
    GLTFModel Load(const Path&);
    void GC();

private:
    class Impl;
    std::unique_ptr<Impl> m_impl;
};

}
