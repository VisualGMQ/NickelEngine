#pragma once

#ifdef _NICKEL_REFLECTION_
#define NICKEL_REFL_ATTR(...)  __attribute__((annotate("nickel(" #__VA_ARGS__ ")")))
#else
#define NICKEL_REFL_ATTR(...)
#endif
