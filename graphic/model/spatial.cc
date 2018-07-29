/********************************************************************
Copyright 2018 Intel Corp. All Rights Reserved.
Description : spatial object
Author      : Wenyi Tang
Email       : wenyi.tang@intel.com
Created     : Feb. 28th, 2018
********************************************************************/
#include "model/spatial.h"

namespace ixr {
namespace engine {
namespace model {
using namespace math;

Spatial::Spatial()
    : p_(),
      anchor_(),
      up_(0, 1.0f, 0),
      normal_(IdentityNormal),
      rotation_(Quaternion::Identity()) {}

Spatial::Spatial(Position p, Quaternion r)
    : p_(p),
      anchor_(p),
      rotation_(r),
      up_(0, 1.0f, 0),
      normal_(IdentityNormal) {
  normal_.Rotate(r);
  up_.Rotate(r);
}

Spatial::~Spatial() {}

void Spatial::Reset() {
  p_ = {};
  anchor_ = {};
  normal_ = IdentityNormal;
  rotation_ = Quaternion::Identity();
}

Position Spatial::SetPosition(float x, float y, float z) {
  auto dest = Position(x, y, z);
  auto diff = dest - p_;
  p_ = dest;
  anchor_ += diff;
  return diff;
}

Quaternion Spatial::SetRotation(Quaternion q) {
  normal_ = IdentityNormal;
  normal_.Rotate(q);
  up_ = Position(0, 1.0f, 0);
  up_.Rotate(q);
  Quaternion diff = q * rotation_.Conj();
  rotation_ = q;
  return diff;
}

void Spatial::Rotate(Quaternion q) {
  normal_.Rotate(q);
  up_.Rotate(q);
  rotation_ = q * rotation_;
}

void Spatial::SetAnchor(float x, float y, float z) {
  anchor_ = Position(x, y, z);
}

const Position &Spatial::GetPosition() const { return p_; }

const Position &Spatial::GetAnchor() const { return anchor_; }
}  // namespace model
}  // namespace engine
}  // namespace ixr