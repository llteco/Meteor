/********************************************************************
Copyright 2018 Intel Corp. All Rights Reserved.
Description : vertex buffer
Author      : Wenyi Tang
Email       : wenyi.tang@intel.com
Created     : Feb. 28th, 2018
********************************************************************/
#ifndef IXR_ENGINE_CORE_VERTEX_VERTEX_BUFFER_H_
#define IXR_ENGINE_CORE_VERTEX_VERTEX_BUFFER_H_
#include "core/gpu_buffer/gpu_buffer.h"
#include "core/vertex/vertex_element.h"
#include "engine/types.h"
#include "export/export.h"

namespace ixr {
namespace engine {
class Env;
namespace core {

class IXR_ENGINE_API VertexBuffer {
 public:
  explicit VertexBuffer(engine::Env *e);
  VertexBuffer();
  VertexBuffer(const VertexBuffer &b);
  VertexBuffer &operator=(const VertexBuffer &rhs);
  ~VertexBuffer();

  template <class T>
  void SetVertex(const std::vector<T> &vertex) {
    if (!vertex_buffer_) {
      vertex_stride_ = sizeof T;
      SetVertexInternal(vertex.data(), vertex.size(), vertex_stride_);
    } else {
      vertex_buffer_->Update(vertex.data());
    }
  }

  void SetIndex(const std::vector<uint32_t> &index);

  template <class T>
  T GetVertexStrideAs() const {
    return static_cast<T>(vertex_stride_);
  }

  GpuBuffer *GetVertexBuffer();

  GpuBuffer *GetIndexBuffer();

 private:
  void SetVertexInternal(const void *head, size_t vertex_number, size_t stride);

 private:
  std::vector<uint32_t> index_internal_;
  GpuBuffer *index_buffer_;
  GpuBuffer *vertex_buffer_;
  size_t vertex_stride_;
  engine::Env *env_;
};
}  // namespace core
}  // namespace engine
}  // namespace ixr
#endif  // IXR_ENGINE_CORE_VERTEX_VERTEX_BUFFER_H_
