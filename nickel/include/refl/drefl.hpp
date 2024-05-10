#pragma once

#include "common/ecs.hpp"

namespace nickel {

enum Attribute {
    AttrColor,
    AttrRange01,
    EditorNodisplay,
};

void RegistReflectInfos();

}