/********************************************************************
Copyright 2018 Intel Corp. All Rights Reserved.
Description : camera helper
Author      : Wenyi Tang
Email       : wenyi.tang@intel.com
Created     : Feb. 28th, 2018
********************************************************************/
#include "model/camera.h"

namespace ixr {
namespace engine {
namespace model {
using namespace math;

Camera::Camera() {}

Camera::~Camera(){};

void Camera::LookAt(Position point) {
  Position direction = point - p_;
  float angle = Angular(normal_, direction);
  Position axis = normal_ * direction;
  Quaternion r(axis, angle);
  Rotate(r);
}

Matrix Camera::GetProjectionMatrix() { return LookToLH(p_, normal_, up_); }
}  // namespace model
}  // namespace engine
}  // namespace ixr
