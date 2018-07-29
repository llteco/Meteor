/********************************************************************
Copyright 2018 Intel Corp. All Rights Reserved.
Description : scenes
Author      : Wenyi Tang
Email       : wenyi.tang@intel.com
Created     : Feb. 28th, 2018
********************************************************************/
#include "model/scenes.h"
#include "engine/engine.h"
#include "errors/error.h"
#include "utils/names.h"

namespace ixr {
namespace engine {
namespace model {
using namespace core;
using namespace utils;

Scene::Scene(Env *e) : env_(e) {}

Scene::~Scene() {}

void Scene::AddShape(Model *shape, const char *name) {
  Names shape_name(name ? name : "Shape", 0);
  while (shapes_.find(shape_name.str()) != shapes_.end()) {
    shape_name.AdvancePostfix();
  }
  shapes_[shape_name.str()] = shape;
  auto bf = VertexBuffer(env_);
  bf.SetVertex(shape->GetVertex());
  bf.SetIndex(shape->GetIndice());
  shapebuffers_[shape] = bf;
}

void Scene::RemoveShape(const char *name) noexcept {
  if (shapes_.find(name) != shapes_.end()) {
    shapes_.erase(name);
  }
}

void Scene::Draw(Renderer *renderer) {
  THROW_IF_FAIL(!renderer, "Renderer is null!");
  for (auto &sb : shapebuffers_) {
    auto &shape = sb.first;
    auto &bf = sb.second;
    bf.SetVertex(shape->GetVertex());
    renderer->SetVertexAndIndex(bf.GetVertexBuffer(),
                                bf.GetVertexStrideAs<uint32_t>(),
                                bf.GetIndexBuffer());
    int32_t index_count = static_cast<int32_t>(shape->GetIndice().size());
    renderer->Draw(index_count);
  }
}
}  // namespace model
}  // namespace engine
}  // namespace ixr
