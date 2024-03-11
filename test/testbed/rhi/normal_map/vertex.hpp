#pragma once
#include "nickel.hpp"

struct Vertex {
    nickel::cgmath::Vec3 position;
    nickel::cgmath::Vec2 uv;
    nickel::cgmath::Vec3 normal;
    nickel::cgmath::Vec3 tangent;
};

// clang-format off
static std::array<Vertex, 36> gVertices = {
  Vertex{nickel::cgmath::Vec3{  1, -1, 1, }, nickel::cgmath::Vec2{ 0, 1,}},
  Vertex{nickel::cgmath::Vec3{  -1, -1, 1,  },  nickel::cgmath::Vec2{ 1, 1,}},
  Vertex{nickel::cgmath::Vec3{  -1, -1, -1, },  nickel::cgmath::Vec2{ 1, 0,}},
  Vertex{nickel::cgmath::Vec3{  1, -1, -1,  },  nickel::cgmath::Vec2{ 0, 0,}},
  Vertex{nickel::cgmath::Vec3{  1, -1, 1,   },  nickel::cgmath::Vec2{ 0, 1,}},
  Vertex{nickel::cgmath::Vec3{  -1, -1, -1, },  nickel::cgmath::Vec2{ 1, 0,}},

  Vertex{nickel::cgmath::Vec3{  1, 1, 1,    }, nickel::cgmath::Vec2{ 0, 1,}},
  Vertex{nickel::cgmath::Vec3{  1, -1, 1,   }, nickel::cgmath::Vec2{ 1, 1,}},
  Vertex{nickel::cgmath::Vec3{  1, -1, -1,},  nickel::cgmath::Vec2{ 1, 0,}},
  Vertex{nickel::cgmath::Vec3{  1, 1, -1, },  nickel::cgmath::Vec2{ 0, 0,}},
  Vertex{nickel::cgmath::Vec3{  1, 1, 1, },  nickel::cgmath::Vec2{ 0, 1,}},
  Vertex{nickel::cgmath::Vec3{  1, -1, -1,}, nickel::cgmath::Vec2{ 1, 0,}},

  Vertex{nickel::cgmath::Vec3{  -1, 1, 1, }, nickel::cgmath::Vec2{ 0, 1,}},
  Vertex{nickel::cgmath::Vec3{  1, 1, 1,},   nickel::cgmath::Vec2{ 1, 1,}},
  Vertex{nickel::cgmath::Vec3{  1, 1, -1, }, nickel::cgmath::Vec2{ 1, 0,}},
  Vertex{nickel::cgmath::Vec3{  -1, 1, -1,}, nickel::cgmath::Vec2{ 0, 0,}},
  Vertex{nickel::cgmath::Vec3{  -1, 1, 1, }, nickel::cgmath::Vec2{ 0, 1,}},
  Vertex{nickel::cgmath::Vec3{  1, 1, -1, }, nickel::cgmath::Vec2{ 1, 0,}},

  Vertex{nickel::cgmath::Vec3{  -1, -1, 1,}, nickel::cgmath::Vec2{ 0, 1,}},
  Vertex{nickel::cgmath::Vec3{  -1, 1, 1, }, nickel::cgmath::Vec2{ 1, 1,}},
  Vertex{nickel::cgmath::Vec3{  -1, 1, -1,}, nickel::cgmath::Vec2{ 1, 0,}},
  Vertex{nickel::cgmath::Vec3{  -1, -1, -1}, nickel::cgmath::Vec2{ 0, 0,}},
  Vertex{nickel::cgmath::Vec3{  -1, -1, 1,}, nickel::cgmath::Vec2{ 0, 1,}},
  Vertex{nickel::cgmath::Vec3{  -1, 1, -1,}, nickel::cgmath::Vec2{ 1, 0,}},

  Vertex{nickel::cgmath::Vec3{  1, 1, 1,}, nickel::cgmath::Vec2{ 0, 1,}},
  Vertex{nickel::cgmath::Vec3{  -1, 1, 1, }, nickel::cgmath::Vec2{ 1, 1,}},
  Vertex{nickel::cgmath::Vec3{  -1, -1, 1,}, nickel::cgmath::Vec2{ 1, 0,}},
  Vertex{nickel::cgmath::Vec3{  -1, -1, 1,}, nickel::cgmath::Vec2{ 1, 0,}},
  Vertex{nickel::cgmath::Vec3{  1, -1, 1, }, nickel::cgmath::Vec2{ 0, 0,}},
  Vertex{nickel::cgmath::Vec3{  1, 1, 1,}, nickel::cgmath::Vec2{ 0, 1,}},

  Vertex{nickel::cgmath::Vec3{  1, -1, -1,}, nickel::cgmath::Vec2{ 0, 1,}},
  Vertex{nickel::cgmath::Vec3{  -1, -1, -1}, nickel::cgmath::Vec2{ 1, 1,}},
  Vertex{nickel::cgmath::Vec3{  -1, 1, -1,}, nickel::cgmath::Vec2{ 1, 0,}},
  Vertex{nickel::cgmath::Vec3{  1, 1, -1, }, nickel::cgmath::Vec2{ 0, 0,}},
  Vertex{nickel::cgmath::Vec3{  1, -1, -1,}, nickel::cgmath::Vec2{ 0, 1,}},
  Vertex{nickel::cgmath::Vec3{-1, 1, -1,}, nickel::cgmath::Vec2{ 1, 0,}},
};

// clang-format on
