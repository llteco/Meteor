/********************************************************************
Copyright 2018 Intel Corp. All Rights Reserved.
Description : renderer interface
Author      : Wenyi Tang
Email       : wenyi.tang@intel.com
Created     : Feb. 28th, 2018
********************************************************************/
#ifndef IXR_ENGINE_CORE_RENDERER_RENDERER_H_
#define IXR_ENGINE_CORE_RENDERER_RENDERER_H_
#include "core/gpu_buffer/gpu_buffer.h"
#include "core/shader/shader.h"
#include "core/vertex/vertex_element.h"
#include "engine/types.h"
#include "platform/native_object.h"

namespace ixr {
namespace engine {
/* Enumerate rasterizer state mode */
enum RasterizerDesc {
  RS_CULL_BACK = 0x01,
  RS_CULL_FRONT = 0x02,
  RS_CULL_NONE = 0x04,
  RS_FILL_SOLID = 0x100,
  RS_FILL_WIRE = 0x200,
  RS_SCISSOR_ENABLE = 0x1000,
};

namespace core {
class Texture;
/** Abstract class for render device context.
 * A Renderer represents for a graphic device on the machine,
 * and each renderer maintain a device context inside itself.
 */
class Renderer : public Object {
 public:
  using VertexElements = std::vector<VertexElement>;
  using RenderTargets = std::vector<Texture *>;

  /** Create the device and its context.
   * \param [in] id: specify the adapter vendor id, 0 to create on primary
   * device. \note the available vendor id are enumerated in AdapterVendorId.
   */
  virtual void CreateDevice(uint32_t id = 0) = 0;

  /** Set input vertex layout */
  virtual void SetInputLayout(const VertexElements &v,
                              Shader *vertex_shader) = 0;

  /** Set the vertex and index buffer */
  virtual void SetVertexAndIndex(GpuBuffer *vertice, uint32_t stride,
                                 GpuBuffer *indice) = 0;

  /** Set the shader.
   * The specific shader type is determined inside Shader class.
   */
  virtual void SetShader(Shader *shader) = 0;

  /** Set the sampler state */
  virtual void SetSampler() = 0;

  /** Set the rasterizer state.
   * Default value is fill solid and cull back.
   */
  virtual void SetRasterizer(uint32_t rasterizer_desc = 0x101) = 0;

  virtual void SetBlendState(const float (&factor)[4], uint32_t mask) = 0;

  /** Set viewport */
  virtual void SetViewport(float w, float h, float x, float y, float min,
                           float max) = 0;

  virtual void SetScissor(int32_t left, int32_t top, int32_t right,
                          int32_t bottom) = 0;

  /** Set the render targets and depth buffer
   * \param [in] rt: a list of render targets
   * \param [in] depth: a depth buffer
   */
  virtual void SetRenderTargets(const RenderTargets &rt, Texture *depth) = 0;

  virtual void SetDepthStencil() = 0;

  /** Clear render target to the background color */
  virtual void ClearRenderTarget(Texture *tex, const float color[4]) = 0;

  /** Clear depth stencil buffer */
  virtual void ClearDepthStencil(Texture *depth) = 0;

  /** Set a texture as shader resource */
  virtual void SetShaderResources(Texture *tex) = 0;

  /** Set the 3D projection matrix */
  virtual void SetModelViewPerspectiveProjection(float mvp[16]) = 0;

  /** Draw primitives.
   * \param [in] count: the vertex count to draw.
   * \param [in] offset: always 0
   * \param [in] base: always 0
   */
  virtual void Draw(int32_t count = -1, int32_t offset = 0,
                    int32_t base = 0) = 0;

  virtual void SetBarrier() = 0;

  virtual void SyncBarrier() = 0;
};
}  // namespace core
}  // namespace engine
}  // namespace ixr
#endif  // IXR_ENGINE_CORE_RENDERER_RENDERER_H_
