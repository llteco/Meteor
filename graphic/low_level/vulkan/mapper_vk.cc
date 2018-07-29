/********************************************************************
Copyright 2018 Intel Corp. All Rights Reserved.
Description : vulkan implement
Author      : Wenyi Tang
Email       : wenyi.tang@intel.com
Created     : April 23rd, 2018
********************************************************************/
#include "low_level/vulkan/mapper_vk.h"

namespace ixr {
namespace engine {
namespace vk {
VkSurfaceFormatKHR ToSurfaceFormat(PixelFormat fmt) {
  VkSurfaceFormatKHR vk_format;
  vk_format.colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
  vk_format.format = ToVkFormat(fmt);
  return vk_format;
}

VkSurfaceKHR ToSurface(void *window_handle) {
  
}

VkFormat ToVkFormat(PixelFormat fmt) {
  switch (fmt) {
    case PF_RGBA32_UNORM:
      return VK_FORMAT_R8G8B8_UNORM;
    case PF_BGRA32_UNORM:
      return VK_FORMAT_B8G8R8_UNORM;
    case PF_R8_UNORM:
      return VK_FORMAT_R8_UNORM;
    case PF_R8G8_UNORM:
      return VK_FORMAT_R8G8_UNORM;
    case PF_FLOAT32:
      return VK_FORMAT_D32_SFLOAT;
    case PF_DEPTHSTENCIL:
      return VK_FORMAT_D24_UNORM_S8_UINT;
    default:
      ETHROW("Unsupported format!");
  };
}

VkImageType ToImageType(TextureType type) {
  switch(type) {
    case TEXTURE_TYPE_1D:
      return VK_IMAGE_TYPE_1D;
    case TEXTURE_TYPE_2D:
      return VK_IMAGE_TYPE_2D;    
    case TEXTURE_TYPE_3D:
      return VK_IMAGE_TYPE_3D;
    case TEXTURE_TYPE_VIEW:
    default:
      ETHROW("Unsupported texture type!");
  };
}

VkImageUsageFlags ToImageUsage(uint32_t usage) {
  
}
}  // namespace vk
}  // namespace engine
}  // namespace ixr