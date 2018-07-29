/********************************************************************
Copyright 2018 Intel Corp. All Rights Reserved.
Description : vulkan implement
Author      : Wenyi Tang
Email       : wenyi.tang@intel.com
Created     : April 25th, 2018
********************************************************************/
#ifndef IXR_ENGINE_LOW_LEVEL_VULKAN_TEXTURE_VK_H_
#define IXR_ENGINE_LOW_LEVEL_VULKAN_TEXTURE_VK_H_
#include <vulkan/vulkan.h>
#include "core/texture/texture.h"
#include "low_level/vulkan/env_vk.h"

namespace ixr {
namespace engine {
class Env;

namespace vk {
/** vulkan texture implementation */
class VulkanTexture : public core::Texture {
 public:
  VulkanTexture(engine::Env *e);
  virtual ~VulkanTexture();

  virtual Handle GetHandle() override;

  virtual TextureDesc GetDesc() const override;

  virtual void CreateTexture(TextureDesc desc) override;

  virtual void CreateReferenceFrom(Handle texture, TextureType type) override;

  virtual void CopyTo(Texture *dst) override;

  virtual void Map(void **data, uint32_t *pitch, uint32_t flag) override;

  virtual void Unmap() override;

  virtual Handle AsShaderResource() override;

  virtual Handle AsRenderTarget() override;

  virtual Handle AsDepthStencil();

 private:
 private:
  engine::Env *env_;
  VulkanEnvironment vk_env_;
  VkImageCreateInfo image_info_;
  VkImage image_;
  VkImageView view_;
};
}  // namespace vk
}  // namespace engine
}  // namespace ixr
#endif  // IXR_ENGINE_LOW_LEVEL_VULKAN_TEXTURE_VK_H_
