/********************************************************************
Copyright 2018 Intel Corp. All Rights Reserved.
Description : vertex buffer
Author      : Wenyi Tang
Email       : wenyi.tang@intel.com
Created     : Feb. 28th, 2018
********************************************************************/
#include "core/vertex/vertex_buffer.h"
#include <algorithm>
#include <iterator>
#include "engine/engine.h"
#include "platform/platform.h"

namespace ixr {
namespace engine {
namespace core {
VertexBuffer::VertexBuffer(engine::Env *e) : env_(e) {
  index_buffer_ = vertex_buffer_ = nullptr;
}

VertexBuffer::VertexBuffer() {}

VertexBuffer::VertexBuffer(const VertexBuffer &b) {
  env_ = b.env_;
  index_buffer_ = b.index_buffer_;
  index_buffer_ = b.index_buffer_;
  vertex_buffer_ = b.vertex_buffer_;
  vertex_stride_ = b.vertex_stride_;
}

VertexBuffer &VertexBuffer::operator=(const VertexBuffer &rhs) {
  if (this != &rhs) {
    env_ = rhs.env_;
    index_buffer_ = rhs.index_buffer_;
    index_buffer_ = rhs.index_buffer_;
    vertex_buffer_ = rhs.vertex_buffer_;
    vertex_stride_ = rhs.vertex_stride_;
  }
  return *this;
}

VertexBuffer::~VertexBuffer() {}

void VertexBuffer::SetIndex(const std::vector<uint32_t> &index) {
  BufferDesc bd{BUFFER_USAGE_INDICE, const_cast<uint32_t *>(index.data()),
                index.size() * sizeof(uint32_t), 0};
  index_buffer_ = env_->NewGpuBuffer(bd);
}

GpuBuffer *VertexBuffer::GetVertexBuffer() { return vertex_buffer_; }

GpuBuffer *VertexBuffer::GetIndexBuffer() {
  if (!index_buffer_) SetIndex(index_internal_);
  return index_buffer_;
}

void VertexBuffer::SetVertexInternal(const void *head, size_t vertex_number,
                                     size_t stride) {
  BufferDesc bd{BUFFER_USAGE_VERTEX, const_cast<void *>(head),
                vertex_number * stride, 0};
  vertex_buffer_ = env_->NewGpuBuffer(bd);
  std::generate_n(
      std::back_inserter(index_internal_), vertex_number,
      [n = uint32_t(index_internal_.size())]() mutable { return n++; });
}
}  // namespace core
}  // namespace engine
}  // namespace ixr
