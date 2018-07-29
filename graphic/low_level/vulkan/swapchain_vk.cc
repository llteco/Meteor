/********************************************************************
Copyright 2018 Intel Corp. All Rights Reserved.
Description : vulkan implement
Author      : Wenyi Tang
Email       : wenyi.tang@intel.com
Created     : April 23rd, 2018
********************************************************************/
#include "low_level/vulkan/swapchain_vk.h"
#include "engine/engine.h"
#include "low_level/vulkan/mapper_vk.h"

namespace ixr {
namespace engine {
using namespace core;

namespace vk {
VulkanSwapChain::VulkanSwapChain(Env* e) : env_(e) {
  vk_env_ = *static_cast<VulkanEnvironment*>(e->GetHandle().pthis);
  uint32_t count = 0;
  vkEnumerateDeviceExtensionProperties(vk_env_.phy_dev, nullptr, &count,
                                       nullptr);
  std::vector<VkExtensionProperties> ext(count);
  vkEnumerateDeviceExtensionProperties(vk_env_.phy_dev, nullptr, &count,
                                       ext.data());
  bool is_swapchain_support = false;
  for (auto& e : ext) {
    if (strcmp(e.extensionName, VK_KHR_SWAPCHAIN_EXTENSION_NAME) == 0) {
      is_swapchain_support = true;
      break;
    }
  }
  THROW_IF_FAIL(is_swapchain_support,
                "This device doesn't support swapchain feature!");
}

VulkanSwapChain::~VulkanSwapChain() {
  // @Todo: thread noooooot safe!
  vkDestroySwapchainKHR(vk_env_.dev, swapchain_, nullptr);
}

void VulkanSwapChain::CreateSwapChain(const SwapChainDesc& sd) {
  VkSurfaceKHR surface = ToSurface(sd.window_handle);
  VkSurfaceCapabilitiesKHR cap;
  VkResult res =
      vkGetPhysicalDeviceSurfaceCapabilitiesKHR(vk_env_.phy_dev, surface, &cap);
  // @Todo: Do some check
  vk_size_ = VkExtent2D{sd.width, sd.height};
  if (!vk_size_.width || !vk_size_.height) {
    vk_size_ = cap.currentExtent;
  }
  VkSwapchainCreateInfoKHR info;
  info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
  info.flags = 0;
  info.surface = surface;
  info.imageFormat = ToSurfaceFormat(sd.format).format;
  info.imageColorSpace = ToSurfaceFormat(sd.format).colorSpace;
  info.presentMode = VK_PRESENT_MODE_FIFO_KHR;
  info.imageExtent = vk_size_;
  info.minImageCount = cap.minImageCount + 1;
  info.imageArrayLayers = 1;
  info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
  info.queueFamilyIndexCount = 0;
  info.pQueueFamilyIndices = nullptr;
  info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
  info.preTransform = cap.currentTransform;
  info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
  info.clipped = VK_TRUE;
  info.oldSwapchain = VK_NULL_HANDLE;
  info.pNext = nullptr;

  res = vkCreateSwapchainKHR(vk_env_.dev, &info, nullptr, &swapchain_);
  THROW_IF_FAIL(res != VK_SUCCESS, "Fail to create vkSwapchainKHR");
  uint32_t count;
  res = vkGetSwapchainImagesKHR(vk_env_.dev, swapchain_, &count, nullptr);
  THROW_IF_FAIL(res != VK_SUCCESS, "Fail to get swapchain images");
  buffers_.resize(count);
  res = vkGetSwapchainImagesKHR(vk_env_.dev, swapchain_, &count, buffers_.data());
  THROW_IF_FAIL(res != VK_SUCCESS, "Fail to get swapchain images");
}

Texture *VulkanSwapChain::GetBuffer(uint32_t index) {
  TextureDesc td;
  td.ref = buffers_[index];
  return env_->NewTexture(td);
}

void VulkanSwapChain::Present(bool vsync) {
  
}

void VulkanSwapChain::Resize(uint32_t w, uint32_t h) {

}

void VulkanSwapChain::GetSize(uint32_t *w, uint32_t *h, float *ratio) {
  *w = vk_size_.width;
  *h = vk_size_.height;
  if (ratio) {
    *ratio = static_cast<float>(*w) / static_cast<float>(*h);
  }
}
}  // namespace vk
}  // namespace engine
}  // namespace ixr
