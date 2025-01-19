#include "nickel/physics/context.hpp"
#include "nickel/physics/internal/context_impl.hpp"

namespace nickel::physics {

Context::Context() : m_impl{std::make_unique<ContextImpl>()} {}

Context::~Context() {}

}  // namespace nickel::physics