/********************************************************************
Copyright 2018 Intel Corp. All Rights Reserved.
Description : spatial object
Author      : Wenyi Tang
Email       : wenyi.tang@intel.com
Created     : Feb. 28th, 2018
********************************************************************/
#ifndef IXR_ENGINE_MODEL_SPATIAL_H_
#define IXR_ENGINE_MODEL_SPATIAL_H_
#include "engine/types.h"
#include "math/math.h"

namespace ixr {
namespace engine {
namespace model {

const math::Position IdentityNormal = math::Position(0, 0, 1);

/* A base class for spatial object */
class Spatial {
 public:
  Spatial();
  Spatial(math::Position p, math::Quaternion r);

  virtual ~Spatial();

  virtual void Reset();

  virtual math::Position SetPosition(float x, float y, float z);

  virtual math::Quaternion SetRotation(math::Quaternion q);

  virtual void Rotate(math::Quaternion q);

  virtual void SetAnchor(float x, float y, float z);

  virtual const math::Position &GetPosition() const;

  virtual const math::Position &GetAnchor() const;

 protected:
  math::Position p_;
  math::Position up_;
  math::Position normal_;
  math::Position anchor_;
  math::Quaternion rotation_;
};
}  // namespace model
}  // namespace engine
}  // namespace ixr
#endif  // IXR_ENGINE_MODEL_SPATIAL_H_
