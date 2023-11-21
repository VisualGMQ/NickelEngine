#pragma once

#include "pch.hpp"

namespace nickel {

toml::table SaveAsPrefab(gecs::entity, gecs::registry);
gecs::entity CreateFromPrefab(std::string_view filename, gecs::registry);

}
