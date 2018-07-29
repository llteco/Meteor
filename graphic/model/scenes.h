/********************************************************************
Copyright 2018 Intel Corp. All Rights Reserved.
Description : scenes
Author      : Wenyi Tang
Email       : wenyi.tang@intel.com
Created     : Feb. 28th, 2018
********************************************************************/
#ifndef IXR_ENGINE_MODEL_SCENES_H_
#define IXR_ENGINE_MODEL_SCENES_H_
#include "core/renderer/renderer.h"
#include "core/vertex/vertex_buffer.h"
#include "engine/types.h"
#include "export/export.h"
#include "model/camera.h"
#include "model/model.h"

namespace ixr {
namespace engine {
class Env;
namespace model {
/* A scene is a container for a world */
class IXR_ENGINE_API Scene {
 public:
  using ShapeContainer = std::map<String, Model *>;
  using ShapeVertexBuffer = std::map<Model *, core::VertexBuffer>;

  Scene(Env *e);

  ~Scene();

  void AddShape(Model *shape, const char *name = nullptr);

  void RemoveShape(const char *name) noexcept;

  void Draw(core::Renderer *renderer);

 protected:
  ShapeContainer shapes_;
  ShapeVertexBuffer shapebuffers_;
  engine::Env *env_;
};
}  // namespace model
}  // namespace engine
}  // namespace ixr

#endif  // IXR_ENGINE_MODEL_SCENES_H_
