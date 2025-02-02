#pragma once
#include "nickel/common/memory/memory.hpp"
#include "nickel/fs/path.hpp"

namespace nickel::graphics {
struct GLTFModel final {
    class Impl;

    GLTFModel() = default;
    GLTFModel(Impl* impl);
    GLTFModel(const GLTFModel&) noexcept;
    GLTFModel(GLTFModel&&) noexcept;
    GLTFModel& operator=(const GLTFModel&) noexcept;
    GLTFModel& operator=(GLTFModel&&) noexcept;
    ~GLTFModel();
    
    operator bool() const;

private:
    Impl* m_impl{};
};

class GLTFManager {
public:
    GLTFModel Load(const Path&);

private:
    BlockMemoryAllocator<GLTFModel::Impl> m_model_allocator;
};

}
