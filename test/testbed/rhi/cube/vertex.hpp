#pragma once
#include "nickel.hpp"

struct Vertex {
    nickel::cgmath::Vec3 position;
    nickel::cgmath::Vec3 color;
};

// clang-format off
static std::array<Vertex, 36> gVertices = {
       Vertex{nickel::cgmath::Vec3{-0.5f, -0.5f, -0.5f, }, nickel::cgmath::Vec3{ 0.0f, 0.0f, 1.0f}}, // A 0
       Vertex{nickel::cgmath::Vec3{ 0.5f, -0.5f, -0.5f, }, nickel::cgmath::Vec3{ 1.0f, 0.0f, 1.0f}}, // B 1
       Vertex{nickel::cgmath::Vec3{ 0.5f,  0.5f, -0.5f, }, nickel::cgmath::Vec3{ 1.0f, 1.0f, 1.0f}}, // C 2
       Vertex{nickel::cgmath::Vec3{ -0.5f,  0.5f, -0.5f,}, nickel::cgmath::Vec3{  0.0f, 1.0f,1.0f}},  // D 3
       Vertex{nickel::cgmath::Vec3{ -0.5f, -0.5f,  0.5f,}, nickel::cgmath::Vec3{  0.0f, 0.0f,1.0f}},  // E 4
       Vertex{nickel::cgmath::Vec3{ 0.5f, -0.5f,  0.5f, }, nickel::cgmath::Vec3{ 1.0f, 0.0f, 1.0f}},  // F 5
       Vertex{nickel::cgmath::Vec3{ 0.5f,  0.5f,  0.5f, }, nickel::cgmath::Vec3{ 1.0f, 1.0f, 1.0f}},  // G 6
       Vertex{nickel::cgmath::Vec3{ -0.5f,  0.5f,  0.5f,}, nickel::cgmath::Vec3{  0.0f, 1.0f,1.0f}},   // H 7
 
       Vertex{nickel::cgmath::Vec3{ -0.5f,  0.5f, -0.5f,}, nickel::cgmath::Vec3{  0.0f, 0.0f,1.0f}},  // D 8
       Vertex{nickel::cgmath::Vec3{ -0.5f, -0.5f, -0.5f,}, nickel::cgmath::Vec3{  1.0f, 0.0f,1.0f}},  // A 9
       Vertex{nickel::cgmath::Vec3{ -0.5f, -0.5f,  0.5f,}, nickel::cgmath::Vec3{  1.0f, 1.0f,1.0f}},  // E 10
       Vertex{nickel::cgmath::Vec3{ -0.5f,  0.5f,  0.5f,}, nickel::cgmath::Vec3{  0.0f, 1.0f,1.0f}},  // H 11
       Vertex{nickel::cgmath::Vec3{ 0.5f, -0.5f, -0.5f, }, nickel::cgmath::Vec3{ 0.0f, 0.0f, 1.0f}},  // B 12
       Vertex{nickel::cgmath::Vec3{ 0.5f,  0.5f, -0.5f, }, nickel::cgmath::Vec3{ 1.0f, 0.0f, 1.0f}},  // C 13
       Vertex{nickel::cgmath::Vec3{ 0.5f,  0.5f,  0.5f, }, nickel::cgmath::Vec3{ 1.0f, 1.0f, 1.0f}},  // G 14
       Vertex{nickel::cgmath::Vec3{ 0.5f, -0.5f,  0.5f, }, nickel::cgmath::Vec3{ 0.0f, 1.0f, 1.0f}},  // F 15
 
       Vertex{nickel::cgmath::Vec3{ -0.5f, -0.5f, -0.5f,}, nickel::cgmath::Vec3{  0.0f, 0.0f,1.0f}},  // A 16
       Vertex{nickel::cgmath::Vec3{ 0.5f, -0.5f, -0.5f, }, nickel::cgmath::Vec3{ 1.0f, 0.0f, 1.0f}},  // B 17
       Vertex{nickel::cgmath::Vec3{ 0.5f, -0.5f,  0.5f, }, nickel::cgmath::Vec3{ 1.0f, 1.0f, 1.0f}},  // F 18
       Vertex{nickel::cgmath::Vec3{ -0.5f, -0.5f,  0.5f,}, nickel::cgmath::Vec3{  0.0f, 1.0f,1.0f}},  // E 19
       Vertex{nickel::cgmath::Vec3{ 0.5f,  0.5f, -0.5f, }, nickel::cgmath::Vec3{  0.0f, 0.0f,1.0f}},  // C 20
       Vertex{nickel::cgmath::Vec3{ -0.5f,  0.5f, -0.5f,}, nickel::cgmath::Vec3{  1.0f, 0.0f,1.0f}},  // D 21
       Vertex{nickel::cgmath::Vec3{ -0.5f,  0.5f,  0.5f,}, nickel::cgmath::Vec3{  1.0f, 1.0f,1.0f}},  // H 22
       Vertex{nickel::cgmath::Vec3{ 0.5f,  0.5f,  0.5f, }, nickel::cgmath::Vec3{  0.0f, 1.0f,1.0f}},  // G 23
};

static std::array<uint32_t, 36> gIndices = {
      // front and back
      0, 3, 2,
      2, 1, 0,
      4, 5, 6,
      6, 7 ,4,
      // left and right
      11, 8, 9,
      9, 10, 11,
      12, 13, 14,
      14, 15, 12,
      // bottom and top
      16, 17, 18,
      18, 19, 16,
      20, 21, 22,
      22, 23, 20
  };

// clang-format on
