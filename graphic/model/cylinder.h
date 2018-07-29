/********************************************************************
Copyright 2018 Intel Corp. All Rights Reserved.
Description : shape cylinder
Author      : Wenyi Tang
Email       : wenyi.tang@intel.com
Created     : Mar. 9th, 2018
********************************************************************/
#ifndef IXR_ENGINE_MODEL_CYLINDER_H_
#define IXR_ENGINE_MODEL_CYLINDER_H_
#include "model/model.h"

namespace ixr {
namespace engine {
namespace model {
class Cylinder : public Model {
 public:
  Cylinder(float bottom_r, float top_r, float h, int stack, int slice);
  virtual ~Cylinder() {}
};
}  // namespace model
}  // namespace engine
}  // namespace ixr
#endif  // IXR_ENGINE_MODEL_CYLINDER_H_
