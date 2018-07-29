/********************************************************************
Copyright 2018 Intel Corp. All Rights Reserved.
Description : camera helper
Author      : Wenyi Tang
Email       : wenyi.tang@intel.com
Created     : Feb. 28th, 2018
********************************************************************/
#ifndef IXR_ENGINE_MODEL_CAMERA_H_
#define IXR_ENGINE_MODEL_CAMERA_H_
#include "math/math.h"
#include "model/spatial.h"

namespace ixr {
namespace engine {
namespace model {
class Camera : public Spatial {
 public:
  Camera();

  virtual ~Camera();

  void LookAt(math::Position point);

  math::Matrix GetProjectionMatrix();
};
}  // namespace model
}  // namespace engine
}  // namespace ixr
#endif  // IXR_ENGINE_MODEL_CAMERA_H_
