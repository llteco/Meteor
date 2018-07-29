/********************************************************************
Copyright 2018 Intel Corp. All Rights Reserved.
Description : shape plain
Author      : Wenyi Tang
Email       : wenyi.tang@intel.com
Created     : Feb. 28th, 2018
********************************************************************/
#ifndef IXR_ENGINE_MODEL_PLAIN_H_
#define IXR_ENGINE_MODEL_PLAIN_H_
#include "model/model.h"

namespace ixr {
namespace engine {
namespace model {
/* A plain */
class Plain : public Model {
 public:
  Plain(float width, float height);

  virtual ~Plain(){};
};
}  // namespace model
}  // namespace engine
}  // namespace ixr
#endif  // IXR_ENGINE_MODEL_PLAIN_H_
