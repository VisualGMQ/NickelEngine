#include "rhi/context.hpp"
#include "rhi/gl/context.hpp"
#include "rhi/vulkan/context.hpp"

namespace nickel::rhi {

void SetAPIHint() {
#ifdef NICKEL_HAS_VULKAN
    vulkan::SetAPIHint();
#else
    gl::SetAPIHint();
#endif
}

}