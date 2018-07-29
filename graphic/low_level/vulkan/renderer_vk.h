/********************************************************************
Copyright 2018 Intel Corp. All Rights Reserved.
Description : vulkan implement
Author      : Wenyi Tang
Email       : wenyi.tang@intel.com
Created     : April 2nd, 2018
********************************************************************/
#ifndef IXR_ENGINE_LOW_LEVEL_VULKAN_RENDERER_VK_H_
#define IXR_ENGINE_LOW_LEVEL_VULKAN_RENDERER_VK_H_
#include <vulkan/vulkan.h>
#include <numeric>
#include "core/renderer/renderer.h"
#include "core/vertex/vertex_buffer.h"
#include "low_level/vulkan/env_vk.h"

namespace ixr {
namespace engine {
class Env;

namespace vk {
class VulkanRenderer : public core::Renderer {
 public:
  using SlotNum = uint32_t;
  using SlotVertex = std::map<SlotNum, core::VertexBuffer *>;

  VulkanRenderer(Env *e);
  virtual ~VulkanRenderer();

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

  virtual void SetShaderResources(core::Texture *tex) override;

  virtual void SetBlendState(const float (&factor)[4], uint32_t mask) override;

  virtual void SetModelViewPerspectiveProjection(float mvp[16]) override;

  virtual void Draw(int32_t count, int32_t offset, int32_t base) override;

  virtual void SetBarrier() override;

  virtual void SyncBarrier() override;

 private:
  std::vector<VkLayerProperties> enumLayers();
  std::vector<VkExtensionProperties> enumInstanceExtensions();
  std::vector<VkExtensionProperties> enumDeviceExtensions();
#ifdef _DEBUG
  VkResult createDebugCallback();
  static VKAPI_ATTR VkBool32 VKAPI_CALL
  DebugCallback(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objType,
                uint64_t obj, size_t location, int32_t code,
                const char *layerPrefix, const char *msg, void *userData);
  VkDebugReportCallbackEXT debug_report_fn_;
#endif
 private:
  VulkanEnvironment vk_env_;
  GpuBuffer *mvp_cbuffer_;
  AdapterVendorId vid_;
  engine::Env *env_;
};
}  // namespace vk
}  // namespace engine
}  // namespace ixr
#endif  // IXR_ENGINE_LOW_LEVEL_VULKAN_RENDERER_VK_H_
