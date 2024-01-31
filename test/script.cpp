#include "nickel.hpp"

void BootstrapSystem(gecs::world& world,
                     typename gecs::world::registry_type& reg) {
    nickel::ProjectInitInfo info;
    info.windowData.title = "lua script";
    info.windowData.size.Set(1024, 720);

    InitSystem(world, info, reg.commands());

    nickel::LuaScript script("test/test.luau");
    script.OnUpdate(gecs::null_entity);
}
