#pragma once
#include "nickel.hpp"

struct Vertex {
    nickel::cgmath::Vec3 position;
    nickel::cgmath::Vec3 normal;
    nickel::cgmath::Vec2 uv;
};

// clang-format off
static std::array gCubeVertices = {
    // Back face
    Vertex{{-1.0f, -1.0f, -1.0f}, {0.0f, 0.0f, -1.0f}, {0.0f, 0.0f}}, // Bottom-left
    Vertex{{1.0f, 1.0f, -1.0f}, {0.0f, 0.0f, -1.0f}, {1.0f, 1.0f}}, // top-right
    Vertex{{1.0f, -1.0f, -1.0f}, {0.0f, 0.0f, -1.0f}, {1.0f, 0.0f}}, // bottom-right         
    Vertex{{1.0f, 1.0f, -1.0f}, {0.0f, 0.0f, -1.0f}, {1.0f, 1.0f}},  // top-right
    Vertex{{-1.0f, -1.0f, -1.0f}, {0.0f, 0.0f, -1.0f}, {0.0f, 0.0f}},  // bottom-left
    Vertex{{-1.0f, 1.0f, -1.0f}, {0.0f, 0.0f, -1.0f}, {0.0f, 1.0f}},// top-left
    // Front face
    Vertex{{-1.0f, -1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}}, // bottom-left
    Vertex{{1.0f, -1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}},  // bottom-right
    Vertex{{1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},  // top-right
    Vertex{{1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}}, // top-right
    Vertex{{-1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},  // top-left
    Vertex{{-1.0f, -1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}},  // bottom-left
    // Left face
    Vertex{{-1.0f, 1.0f, 1.0f}, {-1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}}, // top-right
    Vertex{{-1.0f, 1.0f, -1.0f}, {-1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}}, // top-left
    Vertex{{-1.0f, -1.0f, -1.0f}, {-1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}},  // bottom-left
    Vertex{{-1.0f, -1.0f, -1.0f}, {-1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}}, // bottom-left
    Vertex{{-1.0f, -1.0f, 1.0f}, {-1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},  // bottom-right
    Vertex{{-1.0f, 1.0f, 1.0f}, {-1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}}, // top-right
    // Right face
    Vertex{{1.0f, 1.0f, 1.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}}, // top-left
    Vertex{{1.0f, -1.0f, -1.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}}, // bottom-right
    Vertex{{1.0f, 1.0f, -1.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}}, // top-right         
    Vertex{{1.0f, -1.0f, -1.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}},  // bottom-right
    Vertex{{1.0f, 1.0f, 1.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},  // top-left
    Vertex{{1.0f, -1.0f, 1.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}}, // bottom-left     
    // Bottom face
    Vertex{{-1.0f, -1.0f, -1.0f}, {0.0f, -1.0f, 0.0f}, {0.0f, 1.0f}}, // top-right
    Vertex{{1.0f, -1.0f, -1.0f}, {0.0f, -1.0f, 0.0f}, {1.0f, 1.0f}}, // top-left
    Vertex{{1.0f, -1.0f, 1.0f}, {0.0f, -1.0f, 0.0f}, {1.0f, 0.0f}},// bottom-left
    Vertex{{1.0f, -1.0f, 1.0f}, {0.0f, -1.0f, 0.0f}, {1.0f, 0.0f}}, // bottom-left
    Vertex{{-1.0f, -1.0f, 1.0f}, {0.0f, -1.0f, 0.0f}, {0.0f, 0.0f}}, // bottom-right
    Vertex{{-1.0f, -1.0f, -1.0f}, {0.0f, -1.0f, 0.0f}, {0.0f, 1.0f}}, // top-right
    // Top face
    Vertex{{-1.0f, 1.0f, -1.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}},// top-left
    Vertex{{1.0f, 1.0f, 1.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}}, // bottom-right
    Vertex{{1.0f, 1.0f, -1.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f}}, // top-right     
    Vertex{{1.0f, 1.0f, 1.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}}, // bottom-right
    Vertex{{-1.0f, 1.0f, -1.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}},// top-left
    Vertex{{-1.0f, 1.0f, 1.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}} // bottom-left        
};

static std::array gPlaneVertices = {
    Vertex{{-5.0f, -1.f, -5.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}},// top-left
    Vertex{{5.0f, -1.f, 5.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}}, // bottom-right
    Vertex{{5.0f, -1.f, -5.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f}}, // top-right     
    Vertex{{5.0f, -1.f, 5.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}}, // bottom-right
    Vertex{{-5.0f, -1.f, -5.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}},// top-left
    Vertex{{-5.0f, -1.f, 5.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}} // bottom-left        
};

// clang-format on
