/********************************************************************
Copyright 2018 Intel Corp. All Rights Reserved.
Description : vulkan implement
Author      : Wenyi Tang
Email       : wenyi.tang@intel.com
Created     : April 23rd, 2018
********************************************************************/
#ifndef IXR_ENGINE_LOW_LEVEL_VULKAN_SWAPCHAIN_VK_H_
#define IXR_ENGINE_LOW_LEVEL_VULKAN_SWAPCHAIN_VK_H_
#include <vulkan/vulkan.h>
#include "core/swapchain/swapchain.h"
#include "core/texture/texture.h"
#include "low_level/vulkan/env_vk.h"

namespace ixr {
namespace engine {
class Env;

namespace vk {

class VulkanSwapChain : public core::SwapChain {
 public:
  VulkanSwapChain(Env* e);
  ~VulkanSwapChain();

  virtual void CreateSwapChain(const SwapChainDesc &sd) override;
  virtual core::Texture *GetBuffer(uint32_t index) override;
  virtual void Present(bool vsync = true) override;
  virtual void Resize(uint32_t w, uint32_t h) override;
  virtual void GetSize(uint32_t *w, uint32_t *h, float *ratio) override;
 private:
  Env *env_;
  VulkanEnvironment vk_env_;
  VkSwapchainKHR swapchain_;
  VkExtent2D vk_size_;
  std::vector<VkImage> buffers_;
};
}  // namespace vk
}  // namespace engine
}  // namespace ixr
#endif  // IXR_ENGINE_LOW_LEVEL_VULKAN_SWAPCHAIN_VK_H_
