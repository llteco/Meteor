/****************************************
 * Copyright (c) 2019 Wenyi Tang
 * Author: Wenyi Tang
 * E-mail: wenyitang@outlook.com
 * Description:
 ****************************************/
#include "meteor/core/tex_pool.h"

using namespace ll::engine;

namespace mt {
TexPool::TexPool(Env *env, core::Renderer *renderer, int width, int height,
                 int cmode) {
  env_ = env;
  rdr_ = renderer;
  for (int i = 0; i < 1; i++) {
    TextureDesc td;
    td.width = width;
    td.height = height;
    td.usage = TU_CPUWRITABLE | TU_STAGING;
    td.format = PF_RGBA32_UNORM;
    stg_.push_back(env_->NewTexture(td));
  }
  TextureDesc td;
  td.width = width;
  td.height = height;
  td.usage = TU_SHADERRESOURCE;
  td.format = PF_RGBA32_UNORM;
  tex_ = env_->NewTexture(td);
}

TexPool::~TexPool() {}

void *TexPool::GetTexID() { return tex_->AsShaderResource(); }

void TexPool::Update(char *data) {
  { stg_[0]->Map(TEX_MAP_WRITE)->UpdateFromRawData(data); }
  stg_[0]->CopyTo(tex_);
}
}  // namespace mt
