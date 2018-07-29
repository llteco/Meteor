/********************************************************************
Copyright 2018 Intel Corp. All Rights Reserved.
Description : model object
Author      : Wenyi Tang
Email       : wenyi.tang@intel.com
Created     : Feb. 28th, 2018
********************************************************************/
#ifndef IXR_ENGINE_MODEL_MODEL_H_
#define IXR_ENGINE_MODEL_MODEL_H_
#include "model/spatial.h"
#include "model/vertex/vertex.h"

namespace ixr {
namespace engine {
namespace model {
class Model : public Spatial {
 public:
  using vertices = std::vector<Vertex>;
  using indices = std::vector<uint32_t>;

  Model();
  Model(const vertices &vert, const indices &indx);
  virtual ~Model();

  virtual vertices &GetVertex();

  virtual indices &GetIndice(uint32_t offset = 0);

  virtual math::Position SetPosition(float x, float y, float z);

  virtual math::Quaternion SetRotation(math::Quaternion q);

  virtual void Rotate(math::Quaternion q);

 protected:
  vertices vert_;
  indices indx_;
  indices indx_offset_;
};
}  // namespace model
}  // namespace engine
}  // namespace ixr

#endif  // IXR_ENGINE_MODEL_MODEL_H_
