/********************************************************************
Copyright 2018 Intel Corp. All Rights Reserved.
Description : dx11 implement
Author      : Wenyi Tang
Email       : wenyi.tang@intel.com
Created     : Feb. 28th, 2018
********************************************************************/

#ifndef IXR_ENGINE_LOW_LEVEL_DX11_RENDERER_DX11_H_
#define IXR_ENGINE_LOW_LEVEL_DX11_RENDERER_DX11_H_
#include <d3d11_2.h>
#include <dxgi1_2.h>
#include <wrl/client.h>
#include <numeric>
#include "core/renderer/renderer.h"
#include "core/vertex/vertex_buffer.h"

namespace ixr {
namespace engine {
enum {
  COM_TYPE_INPUT_LAYOUT,
  COM_TYPE_VERTEX_SHADER,
  COM_TYPE_PIXEL_SHADER,
  COM_TYPE_SAMPLER,
  COM_TYPE_RASTERIZER,
  COM_TYPE_BLEND,
};
class Env;

namespace dx11 {

class Dx11Renderer : public core::Renderer {
 public:
  template <typename T>
  using CPtr = Microsoft::WRL::ComPtr<T>;
  using CacheCOM = std::unordered_map<uint64_t, CPtr<IUnknown>>;
  using SlotNum = uint32_t;
  using SlotVertex = std::map<SlotNum, core::VertexBuffer *>;

  Dx11Renderer(engine::Env *e);
  virtual ~Dx11Renderer();

  virtual void CreateDevice(uint32_t id = 0) override;

  virtual void SetInputLayout(const VertexElements &v,
                              core::Shader *vertex_shader) override;

  virtual void SetVertexBySlot(const SlotVertex &slots);

  virtual void SetVertexAndIndex(core::GpuBuffer *vertice, uint32_t stride,
                                 core::GpuBuffer *indice) override;

  virtual void SetShader(core::Shader *shader) override;

  virtual void SetSampler() override;

  virtual void SetRasterizer(uint32_t rasterizer_desc) override;

  virtual void SetViewport(float w, float h, float x, float y, float min,
                           float max) override;

  virtual void SetScissor(int32_t left, int32_t top, int32_t right,
                          int32_t bottom) override;

  virtual void ClearRenderTarget(core::Texture *tex,
                                 const float color[4]) override;

  virtual void ClearDepthStencil(core::Texture *depth) override;

  virtual void SetRenderTargets(const RenderTargets &rt,
                                core::Texture *depth) override;

  virtual void SetDepthStencil() override;

  virtual void SetShaderResources(core::Texture *tex) override;

  virtual void SetBlendState(const float (&factor)[4], uint32_t mask) override;

  virtual void SetModelViewPerspectiveProjection(float mvp[16]) override;

  virtual void Draw(int32_t count, int32_t offset, int32_t base) override;

  virtual void SetBarrier() override;

  virtual void SyncBarrier() override;

 private:
  bool IsCached(const CPtr<IUnknown> &p);

  bool IsCached(const uint64_t &key);

  void Cache(const CPtr<IUnknown> &p,
             uint64_t key = std::numeric_limits<uint64_t>::max());

 private:
  CPtr<ID3D11Device> device_;
  CPtr<ID3D11DeviceContext> context_;
  CPtr<ID3D11Query> query_;
  CacheCOM cached_coms_;
  core::GpuBuffer *mvp_cbuffer_;
  AdapterVendorId vid_;
  engine::Env *env_;
};
}  // namespace dx11
}  // namespace engine
}  // namespace ixr

#endif  // IXR_ENGINE_LOW_LEVEL_DX11_RENDERER_DX11_H_
