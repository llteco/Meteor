#ifndef METEOR_CORE_TEX_POOL_H_
#define METEOR_CORE_TEX_POOL_H_
#include <deque>
#include <memory>
#include "graphic/engine/engine.h"
#include "meteor/core/loader.h"


class TexPool {
 public:
  TexPool(ixr::engine::Env *env, ixr::engine::core::Renderer *renderer,
          int width, int height, int cmode);
  ~TexPool();

  void *GetTexID();

  void Update(char *data);

 private:
  ixr::engine::Env *env_;
  ixr::engine::core::Renderer *rdr_;
  ixr::engine::core::Texture *tex_;
  std::deque<ixr::engine::core::Texture *> stg_;
};

#endif  // METEOR_CORE_TEX_POOL_H_
