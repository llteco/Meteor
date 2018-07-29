/********************************************************************
Copyright 2018 Intel Corp. All Rights Reserved.
Description : collect basic shapes
Author      : Wenyi Tang
Email       : wenyi.tang@intel.com
Created     : Feb. 28th, 2018
********************************************************************/
#include "model/ball.h"
#include "model/cube.h"
#include "model/cylinder.h"
#include "model/plain.h"

namespace ixr {
namespace engine {
namespace model {
using namespace math;

Plain::Plain(float width, float height) {
  float hw = width / 2.0f;
  float hh = height / 2.0f;
  vert_.emplace_back(Vertex{-hw, +hh, 0.0f, 0.0f, 0.f});
  vert_.emplace_back(Vertex{+hw, +hh, 0.0f, 1.0f, 0.f});
  vert_.emplace_back(Vertex{-hw, -hh, 0.0f, 0.0f, 1.f});
  vert_.emplace_back(Vertex{+hw, -hh, 0.0f, 1.0f, 1.f});
  indx_ = indices{0, 1, 2, 3, 2, 1};
}

Cube::Cube(float length, float width, float height) {
  float x = width / 2.f;
  float y = height / 2.f;
  float z = length / 2.f;
  Plain p0(width, height);
  p0.SetPosition(0, 0, -z);
  std::generate_n(std::back_inserter(vert_), p0.GetVertex().size(),
                  [it = p0.GetVertex().begin()]() mutable { return *it++; });
  std::generate_n(std::back_inserter(indx_), p0.GetIndice().size(),
                  [it = p0.GetIndice(0).begin()]() mutable { return *it++; });
  Plain p1(width, height);
  p1.SetPosition(0, 0, +z);
  p1.SetRotation(Quaternion(0, PI, 0));
  std::generate_n(std::back_inserter(vert_), p1.GetVertex().size(),
                  [it = p1.GetVertex().begin()]() mutable { return *it++; });
  std::generate_n(std::back_inserter(indx_), p1.GetIndice().size(),
                  [it = p1.GetIndice(4).begin()]() mutable { return *it++; });
  Plain p2(length, height);
  p2.SetPosition(-x, 0, 0);
  p2.SetRotation(Quaternion(0, +PI / 2, 0));
  std::generate_n(std::back_inserter(vert_), p2.GetVertex().size(),
                  [it = p2.GetVertex().begin()]() mutable { return *it++; });
  std::generate_n(std::back_inserter(indx_), p2.GetIndice().size(),
                  [it = p2.GetIndice(8).begin()]() mutable { return *it++; });
  Plain p3(length, height);
  p3.SetPosition(+x, 0, 0);
  p3.SetRotation(Quaternion(0, -PI / 2, 0));
  std::generate_n(std::back_inserter(vert_), p3.GetVertex().size(),
                  [it = p3.GetVertex().begin()]() mutable { return *it++; });
  std::generate_n(std::back_inserter(indx_), p3.GetIndice().size(),
                  [it = p3.GetIndice(12).begin()]() mutable { return *it++; });
  Plain p4(width, length);
  p4.SetPosition(0, +y, 0);
  p4.SetRotation(Quaternion(+PI / 2, 0, 0));
  std::generate_n(std::back_inserter(vert_), p4.GetVertex().size(),
                  [it = p4.GetVertex().begin()]() mutable { return *it++; });
  std::generate_n(std::back_inserter(indx_), p4.GetIndice().size(),
                  [it = p4.GetIndice(16).begin()]() mutable { return *it++; });
  Plain p5(width, length);
  p5.SetPosition(0, -y, 0);
  p5.SetRotation(Quaternion(-PI / 2, 0, 0));
  std::generate_n(std::back_inserter(vert_), p5.GetVertex().size(),
                  [it = p5.GetVertex().begin()]() mutable { return *it++; });
  std::generate_n(std::back_inserter(indx_), p5.GetIndice().size(),
                  [it = p5.GetIndice(20).begin()]() mutable { return *it++; });
}

Cylinder::Cylinder(float bottom_r, float top_r, float h, int stack, int slice) {
  vert_.emplace_back(Vertex{0, 0, 0, 0, 0});
  float diff_r = (bottom_r - top_r) / stack;
  float diff_h = h / stack;
  float theta = 2.0f * PI / slice;
  for (int i = 0; i <= stack; i++) {
    // for each stack
    float y = diff_h * i;
    float stack_r = bottom_r - diff_r * i;
    for (int j = 0; j <= slice; j++) {
      float _sin = sinf(theta * j);
      float _cos = cosf(theta * j);
      float x = stack_r * _cos;
      float z = stack_r * _sin;
      vert_.emplace_back(Vertex{x, y, z, 0, 0});
      if (i == 0 && j < slice) {
        indx_.push_back(0);
        indx_.push_back(j + 1);
        indx_.push_back(j + 2);
      }
      if (i > 0 && j < slice) {
        indx_.push_back((slice + 1) * i + j + 1);
        indx_.push_back((slice + 1) * i + j + 2);
        indx_.push_back((slice + 1) * (i - 1) + j + 1);
        indx_.push_back((slice + 1) * (i - 1) + j + 1);
        indx_.push_back((slice + 1) * i + j + 2);
        indx_.push_back((slice + 1) * (i - 1) + j + 2);
      }
      if (i == stack && j < slice) {
        indx_.push_back((slice + 1) * (i + 1) + 1);
        indx_.push_back((slice + 1) * i + j + 2);
        indx_.push_back((slice + 1) * i + j + 1);
      }
    }
  }
  vert_.emplace_back(Vertex{0, h, 0, 0, 0});
}

Ball::Ball(float radius, int subdivide) {
  // Approximate a sphere by tessellating an icosahedron.
  const float f1 = 0.525731f;
  const float f2 = 0.850651f;
  // 20 faces
  vert_ = vertices{
      {-f1, 0.0f, f2, 0, 0}, {f1, 0.0f, f2, 0, 0},   {-f1, 0.0f, -f2, 0, 0},
      {f1, 0.0f, -f2, 0, 0}, {0.0f, f2, f1, 0, 0},   {0.0f, f2, -f1, 0, 0},
      {0.0f, -f2, f1, 0, 0}, {0.0f, -f2, -f1, 0, 0}, {f2, f1, 0.0f, 0, 0},
      {-f2, f1, 0.0f, 0, 0}, {f2, -f1, 0.0f, 0, 0},  {-f2, -f1, 0.0f, 0, 0}};
  indx_ = indices{1,  4,  0, 4,  9, 0, 4, 5,  9, 8, 5, 4,  1,  8, 4,
                  1,  10, 8, 10, 3, 8, 8, 3,  5, 3, 2, 5,  3,  7, 2,
                  3,  10, 7, 10, 6, 7, 6, 11, 7, 6, 0, 11, 6,  1, 0,
                  10, 1,  6, 11, 0, 9, 2, 11, 9, 5, 2, 9,  11, 2, 7};
  // 8 faces
  // vert_ = vertices {
  //  {0,  1,  0, 0, 0}, // 0 top
  //  {0,  0, -1, 0, 0}, // 1 front
  //  {1,  0,  0, 0, 0}, // 2 right
  //  {0,  0,  1, 0, 0}, // 3 back
  //  {-1,  0, 0, 0, 0}, // 4 left
  //  {0, -1,  0, 0, 0}, // 5 bottom
  //};
  // indx_ = indices {
  //  0, 1, 2, // top front-right face
  //  0, 2, 3, // top back-right face
  //  0, 3, 4, // top back-left face
  //  0, 4, 1, // top front-left face
  //  5, 1, 4, // bottom front-left face
  //  5, 4, 3, // bottom back-left face
  //  5, 3, 2, // bottom back-right face
  //  5, 2, 1, // bottom front-right face
  //};
  for (int i = 0; i < subdivide; i++) {
    for (auto &v : vert_) {
      v.pos = v.pos / v.pos.Length() * radius;
    }
    Subdivide();
  }
  for (auto &v : vert_) {
    float longitude = atan2f(v.pos.X(), -v.pos.Z());
    float latitude = acos(v.pos.Y());
    v.texcoord.x = 0.5f - (longitude / 2.f / PI);
    v.texcoord.y = latitude / PI;
  }
}

Ball::Ball(float radius, int stack, int slice) {
  // Create rings of vertices at progressively higher latitudes.
  for (size_t i = 0; i <= stack; i++) {
    float v = 1 - (float)i / stack;

    float latitude = (i * PI / stack) - PI / 2.f;
    float dy, dxz;
    dy = sinf(latitude);
    dxz = cosf(latitude);

    // Create a single ring of vertices at this latitude.
    for (size_t j = 0; j <= slice; j++) {
      float u = (float)j / slice;

      float longitude = j * PI * 2.f / slice;
      float dx, dz;
      dx = sinf(longitude);
      dz = cosf(longitude);

      dx *= dxz;
      dz *= dxz;

      Position p(dx, dy, dz);
      p = p / p.Length();
      vert_.push_back({p * radius, {u, v}});
    }
  }

  // Fill the index buffer with triangles joining each pair of latitude rings.
  uint32_t stride = slice + 1;

  for (int i = 0; i < stack; i++) {
    for (int j = 0; j <= slice; j++) {
      int next_i = i + 1;
      int next_j = (j + 1) % stride;

      indx_.push_back(i * stride + j);
      indx_.push_back(next_i * stride + j);
      indx_.push_back(i * stride + next_j);

      indx_.push_back(i * stride + next_j);
      indx_.push_back(next_i * stride + j);
      indx_.push_back(next_i * stride + next_j);
    }
  }
}

void Ball::Subdivide() {
  auto make_edge = [](uint32_t a, uint32_t b) -> uint32_t {
    auto x = std::min(a, b);
    auto y = std::max(a, b);
    return (y << 16) | x;
  };
  // map edge to middle point index
  std::map<uint32_t, uint32_t> edges;

  indices new_index;
  for (auto i = 0U; i < indx_.size(); i += 3) {
    auto p0 = vert_[indx_[i]];
    auto p1 = vert_[indx_[i + 1]];
    auto p2 = vert_[indx_[i + 2]];
    auto e0 = make_edge(indx_[i], indx_[i + 1]);
    auto e1 = make_edge(indx_[i + 2], indx_[i + 1]);
    auto e2 = make_edge(indx_[i], indx_[i + 2]);
    uint32_t i0, i1, i2;
    if (edges.find(e0) != edges.cend()) {
      i0 = edges[e0];
    } else {
      auto m0 = Vertex((p0.pos + p1.pos) * 0.5f, {0, 0});
      vert_.push_back(m0);
      i0 = static_cast<uint32_t>(vert_.size() - 1);
      edges[e0] = i0;
    }
    if (edges.find(e1) != edges.cend()) {
      i1 = edges[e1];
    } else {
      auto m1 = Vertex((p1.pos + p2.pos) * 0.5f, {0, 0});
      vert_.push_back(m1);
      i1 = static_cast<uint32_t>(vert_.size() - 1);
      edges[e1] = i1;
    }
    if (edges.find(e2) != edges.cend()) {
      i2 = edges[e2];
    } else {
      auto m2 = Vertex((p2.pos + p0.pos) * 0.5f, {0, 0});
      vert_.push_back(m2);
      i2 = static_cast<uint32_t>(vert_.size() - 1);
      edges[e2] = i2;
    }

    new_index.push_back(indx_[i]);
    new_index.push_back(i0);
    new_index.push_back(i2);

    new_index.push_back(i0);
    new_index.push_back(indx_[i + 1]);
    new_index.push_back(i1);

    new_index.push_back(indx_[i + 2]);
    new_index.push_back(i2);
    new_index.push_back(i1);

    new_index.push_back(i0);
    new_index.push_back(i1);
    new_index.push_back(i2);
  }
  indx_ = new_index;
}
}  // namespace model
}  // namespace engine
}  // namespace ixr