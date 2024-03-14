#pragma once
#include "nickel.hpp"

struct Vertex {
    nickel::cgmath::Vec3 position;
};

// clang-format off
static std::array<Vertex, 36> gVertices = {
  Vertex{nickel::cgmath::Vec3{  1, -1, 1, }},
  Vertex{nickel::cgmath::Vec3{  -1, -1, 1,  }},
  Vertex{nickel::cgmath::Vec3{  -1, -1, -1, }},
  Vertex{nickel::cgmath::Vec3{  1, -1, -1,  }},
  Vertex{nickel::cgmath::Vec3{  1, -1, 1,   }},
  Vertex{nickel::cgmath::Vec3{  -1, -1, -1, }},

  Vertex{nickel::cgmath::Vec3{  1, 1, 1,    }},
  Vertex{nickel::cgmath::Vec3{  1, -1, 1,   }},
  Vertex{nickel::cgmath::Vec3{  1, -1, -1,}},  
  Vertex{nickel::cgmath::Vec3{  1, 1, -1, }},  
  Vertex{nickel::cgmath::Vec3{  1, 1, 1, }},
  Vertex{nickel::cgmath::Vec3{  1, -1, -1,}},

  Vertex{nickel::cgmath::Vec3{  -1, 1, 1, }},
  Vertex{nickel::cgmath::Vec3{  1, 1, 1,}},
  Vertex{nickel::cgmath::Vec3{  1, 1, -1, }},
  Vertex{nickel::cgmath::Vec3{  -1, 1, -1,}},
  Vertex{nickel::cgmath::Vec3{  -1, 1, 1, }},
  Vertex{nickel::cgmath::Vec3{  1, 1, -1, }},

  Vertex{nickel::cgmath::Vec3{  -1, -1, 1,}},
  Vertex{nickel::cgmath::Vec3{  -1, 1, 1, }},
  Vertex{nickel::cgmath::Vec3{  -1, 1, -1,}},
  Vertex{nickel::cgmath::Vec3{  -1, -1, -1}},
  Vertex{nickel::cgmath::Vec3{  -1, -1, 1,}},
  Vertex{nickel::cgmath::Vec3{  -1, 1, -1,}},

  Vertex{nickel::cgmath::Vec3{  1, 1, 1,}},
  Vertex{nickel::cgmath::Vec3{  -1, 1, 1, }},
  Vertex{nickel::cgmath::Vec3{  -1, -1, 1,}},
  Vertex{nickel::cgmath::Vec3{  -1, -1, 1,}},
  Vertex{nickel::cgmath::Vec3{  1, -1, 1, }},
  Vertex{nickel::cgmath::Vec3{  1, 1, 1,}},

  Vertex{nickel::cgmath::Vec3{  1, -1, -1,}},
  Vertex{nickel::cgmath::Vec3{  -1, -1, -1}},
  Vertex{nickel::cgmath::Vec3{  -1, 1, -1,}},
  Vertex{nickel::cgmath::Vec3{  1, 1, -1, }},
  Vertex{nickel::cgmath::Vec3{  1, -1, -1,}},
  Vertex{nickel::cgmath::Vec3{-1, 1, -1,}},
};

// clang-format on
