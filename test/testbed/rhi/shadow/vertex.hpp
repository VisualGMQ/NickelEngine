#pragma once
#include "nickel.hpp"

struct Vertex {
    nickel::cgmath::Vec3 position;
    nickel::cgmath::Vec3 normal;
};

// clang-format off
static std::array<Vertex, 36> gVertices = {
    Vertex{nickel::cgmath::Vec3{-0.5f, -0.5f, -0.5f, }, nickel::cgmath::Vec3{ 0.0f,  0.0f, -1.0f,}},
    Vertex{nickel::cgmath::Vec3{0.5f, -0.5f, -0.5f,  }, nickel::cgmath::Vec3{0.0f,  0.0f, -1.0f, }},
    Vertex{nickel::cgmath::Vec3{0.5f,  0.5f, -0.5f,  }, nickel::cgmath::Vec3{0.0f,  0.0f, -1.0f, }},
    Vertex{nickel::cgmath::Vec3{0.5f,  0.5f, -0.5f,  }, nickel::cgmath::Vec3{0.0f,  0.0f, -1.0f, }},
    Vertex{nickel::cgmath::Vec3{-0.5f,  0.5f, -0.5f, }, nickel::cgmath::Vec3{ 0.0f,  0.0f, -1.0f,}},
    Vertex{nickel::cgmath::Vec3{-0.5f, -0.5f, -0.5f, }, nickel::cgmath::Vec3{ 0.0f,  0.0f, -1.0f,}},

    Vertex{nickel::cgmath::Vec3{-0.5f, -0.5f,  0.5f, }, nickel::cgmath::Vec3{ 0.0f,  0.0f,  1.0f,}},
    Vertex{nickel::cgmath::Vec3{0.5f, -0.5f,  0.5f,  }, nickel::cgmath::Vec3{0.0f,  0.0f,  1.0f, }},
    Vertex{nickel::cgmath::Vec3{0.5f,  0.5f,  0.5f,  }, nickel::cgmath::Vec3{0.0f,  0.0f,  1.0f, }},
    Vertex{nickel::cgmath::Vec3{0.5f,  0.5f,  0.5f,  }, nickel::cgmath::Vec3{0.0f,  0.0f,  1.0f, }},
    Vertex{nickel::cgmath::Vec3{-0.5f,  0.5f,  0.5f, }, nickel::cgmath::Vec3{ 0.0f,  0.0f,  1.0f,}},
    Vertex{nickel::cgmath::Vec3{-0.5f, -0.5f,  0.5f, }, nickel::cgmath::Vec3{ 0.0f,  0.0f,  1.0f,}},

    Vertex{nickel::cgmath::Vec3{-0.5f,  0.5f,  0.5f, }, nickel::cgmath::Vec3{-1.0f,  0.0f,  0.0f,}},
    Vertex{nickel::cgmath::Vec3{-0.5f,  0.5f, -0.5f, }, nickel::cgmath::Vec3{-1.0f,  0.0f,  0.0f,}},
    Vertex{nickel::cgmath::Vec3{-0.5f, -0.5f, -0.5f, }, nickel::cgmath::Vec3{-1.0f,  0.0f,  0.0f,}},
    Vertex{nickel::cgmath::Vec3{-0.5f, -0.5f, -0.5f, }, nickel::cgmath::Vec3{-1.0f,  0.0f,  0.0f,}},
    Vertex{nickel::cgmath::Vec3{-0.5f, -0.5f,  0.5f, }, nickel::cgmath::Vec3{-1.0f,  0.0f,  0.0f,}},
    Vertex{nickel::cgmath::Vec3{-0.5f,  0.5f,  0.5f, }, nickel::cgmath::Vec3{-1.0f,  0.0f,  0.0f,}},

    Vertex{nickel::cgmath::Vec3{0.5f,  0.5f,  0.5f,  }, nickel::cgmath::Vec3{1.0f,  0.0f,  0.0f, }},
    Vertex{nickel::cgmath::Vec3{0.5f,  0.5f, -0.5f,  }, nickel::cgmath::Vec3{1.0f,  0.0f,  0.0f, }},
    Vertex{nickel::cgmath::Vec3{0.5f, -0.5f, -0.5f,  }, nickel::cgmath::Vec3{1.0f,  0.0f,  0.0f, }},
    Vertex{nickel::cgmath::Vec3{0.5f, -0.5f, -0.5f,  }, nickel::cgmath::Vec3{1.0f,  0.0f,  0.0f, }},
    Vertex{nickel::cgmath::Vec3{0.5f, -0.5f,  0.5f,  }, nickel::cgmath::Vec3{1.0f,  0.0f,  0.0f, }},
    Vertex{nickel::cgmath::Vec3{0.5f,  0.5f,  0.5f,  }, nickel::cgmath::Vec3{1.0f,  0.0f,  0.0f, }},

    Vertex{nickel::cgmath::Vec3{-0.5f, -0.5f, -0.5f, }, nickel::cgmath::Vec3{ 0.0f, -1.0f,  0.0f,}},
    Vertex{nickel::cgmath::Vec3{0.5f, -0.5f, -0.5f,  }, nickel::cgmath::Vec3{0.0f, -1.0f,  0.0f, }},
    Vertex{nickel::cgmath::Vec3{0.5f, -0.5f,  0.5f,  }, nickel::cgmath::Vec3{0.0f, -1.0f,  0.0f, }},
    Vertex{nickel::cgmath::Vec3{0.5f, -0.5f,  0.5f,  }, nickel::cgmath::Vec3{0.0f, -1.0f,  0.0f, }},
    Vertex{nickel::cgmath::Vec3{-0.5f, -0.5f,  0.5f, }, nickel::cgmath::Vec3{ 0.0f, -1.0f,  0.0f,}},
    Vertex{nickel::cgmath::Vec3{-0.5f, -0.5f, -0.5f, }, nickel::cgmath::Vec3{ 0.0f, -1.0f,  0.0f,}},

    Vertex{nickel::cgmath::Vec3{-0.5f,  0.5f, -0.5f, }, nickel::cgmath::Vec3{ 0.0f,  1.0f,  0.0f,}},
    Vertex{nickel::cgmath::Vec3{0.5f,  0.5f, -0.5f,  }, nickel::cgmath::Vec3{0.0f,  1.0f,  0.0f, }},
    Vertex{nickel::cgmath::Vec3{0.5f,  0.5f,  0.5f,  }, nickel::cgmath::Vec3{0.0f,  1.0f,  0.0f, }},
    Vertex{nickel::cgmath::Vec3{0.5f,  0.5f,  0.5f,  }, nickel::cgmath::Vec3{0.0f,  1.0f,  0.0f, }},
    Vertex{nickel::cgmath::Vec3{-0.5f,  0.5f,  0.5f, }, nickel::cgmath::Vec3{ 0.0f,  1.0f,  0.0f,}},
    Vertex{nickel::cgmath::Vec3{-0.5f,  0.5f, -0.5f, }, nickel::cgmath::Vec3{ 0.0f,  1.0f,  0.0f }},
};
// clang-format on
