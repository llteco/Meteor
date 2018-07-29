/********************************************************************
Copyright 2018 Intel Corp. All Rights Reserved.
Description : cube
Author      : Wenyi Tang
Email       : wenyi.tang@intel.com
Created     : Feb. 28th, 2018
********************************************************************/
#ifndef IXR_ENGINE_MODEL_CUBE_H_
#define IXR_ENGINE_MODEL_CUBE_H_
#include <algorithm>
#include <iterator>
#include "model/model.h"
#include "model/plain.h"

namespace ixr {
namespace engine {
namespace model {
/* A cube */
class Cube : public Model {
 public:
  Cube(float length, float width, float height);

  virtual ~Cube(){};
};
}  // namespace model
}  // namespace engine
}  // namespace ixr

#endif  // IXR_ENGINE_MODEL_CUBE_H_
