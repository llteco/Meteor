/********************************************************************
Copyright 2018 Intel Corp. All Rights Reserved.
Description : model object
Author      : Wenyi Tang
Email       : wenyi.tang@intel.com
Created     : Feb. 28th, 2018
********************************************************************/
#include "model/model.h"

namespace ixr {
namespace engine {
namespace model {
using namespace math;

Model::Model() {}

Model::Model(const vertices &vert, const indices &indx)
    : vert_(vert), indx_(indx) {}

Model::~Model() {}

Model::vertices &Model::GetVertex() { return vert_; }

Model::indices &Model::GetIndice(uint32_t offset) {
  if (offset) {
    indx_offset_ = indx_;
    for (auto &i : indx_offset_) {
      i += offset;
    }
    return indx_offset_;
  }
  return indx_;
}

Position Model::SetPosition(float x, float y, float z) {
  auto diff = Spatial::SetPosition(x, y, z);
  for (auto &v : vert_) {
    // move
    v.pos += diff;
  }
  return diff;
}

Quaternion Model::SetRotation(Quaternion q) {
  auto diff = Spatial::SetRotation(q);
  for (auto &v : vert_) {
    // relative to anchor
    Position rp = v.pos - anchor_;
    // rorate
    rp.Rotate(diff);
    // relative to world
    v.pos = rp + anchor_;
  }
  return diff;
}

void Model::Rotate(Quaternion q) {
  Spatial::Rotate(q);
  for (auto &v : vert_) {
    // relative to anchor
    Position rp = v.pos - anchor_;
    // rorate
    rp.Rotate(q);
    // relative to world
    v.pos = rp + anchor_;
  }
}
}  // namespace model
}  // namespace engine
}  // namespace ixr