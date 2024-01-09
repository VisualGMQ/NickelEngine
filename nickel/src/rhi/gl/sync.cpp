#include "rhi/gl/sync.hpp"
#include "rhi/gl/glcall.hpp"

namespace nickel::rhi::gl {

Fence::Fence() {
    fence_ = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
}

Fence::~Fence() {
    GL_CALL(glDeleteSync(fence_));
}

}  // namespace nickel::rhi::gl