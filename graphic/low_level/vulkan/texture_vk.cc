/********************************************************************
Copyright 2018 Intel Corp. All Rights Reserved.
Description : vulkan implement
Author      : Wenyi Tang
Email       : wenyi.tang@intel.com
Created     : April 25th, 2018
********************************************************************/
#include "low_level/vulkan/texture_vk.h"
#include "engine/engine.h"
#include "low_level/vulkan/mapper_vk.h"

namespace ixr {
namespace engine {
using namespace core;

namespace vk {
VulkanTexture::VulkanTexture(Env *e) : env_(e) {
  vk_env_ = *static_cast<VulkanEnvironment *>(e->GetHandle().pthis);
}

VulkanTexture::~VulkanTexture() {
  vkDestroyImageView(vk_env_.dev, view_, nullptr);
  vkDestroyImage(vk_env_.dev, image_, nullptr);
}

Handle VulkanTexture::GetHandle() { return image_; }

TextureDesc VulkanTexture::GetDesc() const {
  TextureDesc td;
  td.width = image_info_.extent.width;
  td.height = image_info_.extent.height;
  td.depth = image_info_.extent.depth;
  td.array_slices = image_info_.arrayLayers;
  td.mips = image_info_.mipLevels;
  td.sample_count = image_info_.samples;
  return td;
}

void VulkanTexture::CreateTexture(TextureDesc desc) {
  VkImageCreateInfo info;
  info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
  info.pNext = nullptr;
  info.arrayLayers = desc.array_slices;
  info.extent = VkExtent3D{desc.width, desc.height, desc.depth};
  info.format = ToVkFormat(desc.format);
  info.imageType = ToImageType(desc.type);
  info.mipLevels = desc.mips;
  info.flags = 0;
  info.samples = VkSampleCountFlagBits(desc.sample_count);
  info.initialLayout = ToImageLayout(desc.usage);
  info.pQueueFamilyIndices = 0;
  info.queueFamilyIndexCount = 0;
  info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
  info.tiling = VK_IMAGE_TILING_LINEAR;
  info.usage = ToImageUsage(desc.usage);
  VkResult result = vkCreateImage(vk_env_.dev, &info, nullptr, &image_);
  THROW_IF_FAIL(result != VK_SUCCESS, "Fail to create vkImage");
  image_info_ = info;
}

void VulkanTexture::CreateReferenceFrom(Handle texture, TextureType type) {
  VkImage image = static_cast<VkImage>(texture);
  image_ = image;
}

void VulkanTexture::CopyTo(Texture *dst) {}

void VulkanTexture::Map(void **data, uint32_t *pitch, uint32_t flag) {}

void VulkanTexture::Unmap() {}

Handle VulkanTexture::AsShaderResource() {
  VkImageViewCreateInfo info;
  info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
  info.pNext = nullptr;
  info.image = image_;
  info.flags = 0;
  info.format = image_info_.format;
  switch (image_info_.imageType) {
    case VK_IMAGE_TYPE_1D:
      info.viewType = VK_IMAGE_VIEW_TYPE_1D;
      break;
    case VK_IMAGE_TYPE_2D:
      info.viewType = VK_IMAGE_VIEW_TYPE_2D;
      break;
    case VK_IMAGE_TYPE_3D:
      info.viewType = VK_IMAGE_VIEW_TYPE_3D;
      break;
    default:
      ETHROW("invalid image type");
  };
  info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
  info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
  info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
  info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
  info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  info.subresourceRange.baseArrayLayer = 0;
  info.subresourceRange.layerCount = 1;
  info.subresourceRange.baseMipLevel = 0;
  info.subresourceRange.levelCount = image_info_.mipLevels;
  VkResult result = vkCreateImageView(vk_env_.dev, &info, nullptr, &view_);
  THROW_IF_FAIL(result != VK_SUCCESS, "Fail to create image view");
  return view_;
}

Handle VulkanTexture::AsRenderTarget() {}

Handle VulkanTexture::AsDepthStencil() {}
}  // namespace vk
}  // namespace engine
}  // namespace ixr
