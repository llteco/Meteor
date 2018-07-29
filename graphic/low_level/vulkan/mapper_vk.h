/********************************************************************
Copyright 2018 Intel Corp. All Rights Reserved.
Description : vulkan implement
Author      : Wenyi Tang
Email       : wenyi.tang@intel.com
Created     : April 23rd, 2018
********************************************************************/
#ifndef IXR_ENGINE_LOW_LEVEL_VULKAN_MAPPER_VK_H_
#define IXR_ENGINE_LOW_LEVEL_VULKAN_MAPPER_VK_H_
#include <vulkan/vulkan.h>
#include "engine/engine.h"
#include "low_level/vulkan/prerequisite_vk.h"


namespace ixr {
namespace engine {
namespace vk {

VkSurfaceFormatKHR ToSurfaceFormat(PixelFormat fmt);

VkSurfaceKHR ToSurface(void *window_handle);

VkFormat ToVkFormat(PixelFormat fmt);

VkImageType ToImageType(TextureType type);

VkImageUsageFlags ToImageUsage(uint32_t usage);

VkImageLayout ToImageLayout(uint32_t usage);
}
}  // namespace engine
}  // namespace ixr
#endif  // IXR_ENGINE_LOW_LEVEL_VULKAN_MAPPER_VK_H_
