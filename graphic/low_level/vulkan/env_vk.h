/********************************************************************
Copyright 2018 Intel Corp. All Rights Reserved.
Description : vulkan implement
Author      : Wenyi Tang
Email       : wenyi.tang@intel.com
Created     : April 2nd, 2018
********************************************************************/
#ifndef IXR_ENGINE_LOW_LEVEL_VULKAN_ENV_VK_H_
#define IXR_ENGINE_LOW_LEVEL_VULKAN_ENV_VK_H_
#include <vulkan/vulkan.h>
namespace ixr {
namespace engine {
namespace vk {
struct VulkanEnvironment {
  VkInstance ins;
  VkPhysicalDevice phy_dev;
  VkDevice dev;
  VkQueue queue[4];
};
}  // namespace vk
}  // namespace engine
}  // namespace ixr
#endif  // IXR_ENGINE_LOW_LEVEL_VULKAN_ENV_VK_H_
