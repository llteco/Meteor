/********************************************************************
Copyright 2018 Intel Corp. All Rights Reserved.
Description : Vertex in model
Author      : Wenyi Tang
Email       : wenyi.tang@intel.com
Created     : Feb. 28th, 2018
********************************************************************/
#ifndef IXR_ENGINE_MODEL_VERTEX_VERTEX_H_
#define IXR_ENGINE_MODEL_VERTEX_VERTEX_H_
#include "engine/types.h"
#include "math/math.h"

namespace ixr {
namespace engine {
namespace model {
struct float2 {
  float x, y;
};
struct Vertex {
  math::Position pos;
  float2 texcoord;

  Vertex() {}
  Vertex(math::Position p, float2 t) : pos(p), texcoord(t) {}
  Vertex(float x, float y, float z, float u, float v)
      : pos(x, y, z), texcoord{u, v} {}
};
}  // namespace model
}  // namespace engine
}  // namespace ixr
#endif  // IXR_ENGINE_MODEL_VERTEX_VERTEX_H_
