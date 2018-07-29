/********************************************************************
Copyright 2018 Intel Corp. All Rights Reserved.
Description : texture
Author      : Wenyi Tang
Email       : wenyi.tang@intel.com
Created     : Feb. 28th, 2018
********************************************************************/
#ifndef IXR_ENGINE_CORE_TEXTURE_TEXTURE_H_
#define IXR_ENGINE_CORE_TEXTURE_TEXTURE_H_
#include "core/renderer/renderer.h"
#include "engine/types.h"
#include "platform/native_object.h"

namespace ixr {
namespace engine {

enum PixelFormat {
  PF_FLOAT32,
  PF_RGBA32_UNORM,
  PF_BGRA32_UNORM,
  PF_R8_UNORM,
  PF_R8G8_UNORM,
  PF_DEPTHSTENCIL,
};

enum TextureUsage {
  /** A regular texture that is not often or ever updated from the CPU. */
  TU_STATIC = 0x01,
  /** A regular texture that is often updated by the CPU. */
  TU_DYNAMIC = 0x02,
  TU_STAGING = 0x04,
  TU_SHADERRESOURCE = 0x100,
  /** Texture that can be rendered to by the GPU. */
  TU_RENDERTARGET = 0x200,
  /** Texture used as a depth/stencil buffer by the GPU. */
  TU_DEPTHSTENCIL = 0x400,
  /** Texture that allows load/store operations from the GPU program. */
  TU_LOADSTORE = 0x800,
  /** All mesh data will also be cached in CPU memory,
   *  making it available for fast read access from the CPU. */
  TU_CPUCACHED = 0x1000,
  /** Allows the CPU to directly read the texture data buffers from the GPU. */
  TU_CPUREADABLE = 0x2000,
  TU_CPUWRITABLE = 0x4000,
  /** Default (most common) texture usage. */
  TU_DEFAULT = TU_STATIC
};

enum TextureType {
  TEXTURE_TYPE_1D,
  TEXTURE_TYPE_2D,
  TEXTURE_TYPE_3D,
  TEXTURE_TYPE_VIEW,
};

enum TexMapFlag {
  TEX_MAP_READ = 1,
  TEX_MAP_WRITE = 2,
  TEX_MAP_READ_WRITE = TEX_MAP_READ | TEX_MAP_WRITE,
  TEX_MAP_WRITE_DISCARD = 4,
  TEX_MAP_WRITE_NO_OVERWRITE = 5,
  TEX_MAP_DO_NOT_WAIT = 0x100000
};

struct TextureDesc {
  /** Format of pixels in the texture. */
  PixelFormat format;
  /** Width of the texture in pixels. */
  uint32_t width = 1;
  /** Height of the texture in pixels. */
  uint32_t height = 1;
  /** Depth of the texture in pixels (Must be 1 for 2D textures). */
  uint32_t depth = 1;
  /** Number of mip-maps the texture has. This number includes the full
   * resolution map. */
  uint32_t mips = 1;
  /** Describes how the caller plans on using the texture in the pipeline. */
  uint32_t usage = TU_DEFAULT;
  /** Type of the texture. */
  TextureType type = TEXTURE_TYPE_2D;
  /** Number of samples per pixel.
   * Set to 1 or 0 to use the default of a single sample per pixel.
   */
  uint32_t sample_count = 1;
  /** Number of texture slices to create if creating a texture array.
   * Ignored for 3D textures.
   */
  uint32_t array_slices = 1;
  /** If ref is not null, the texture make a referene of this handle,
   * and do not create a new one.
   */
  Handle ref = nullptr;
  /** Data of initialized pixels.
   * Ignored if is null. 
   */
  void *data = nullptr;
  uint32_t data_size = 0;
};

namespace core {
class TextureMapData {
 public:
  virtual ~TextureMapData() {}

  virtual void UpdateFromRawData(const void *src, uint32_t pitch = 0) = 0;

  virtual void ReadIntoMemory(void *dst, uint32_t pitch = 0) = 0;

  virtual bool IsValid() const = 0;
};

/** Abstract class for texture resources */
class Texture : public Object {
 public:
  virtual void CreateTexture(TextureDesc) = 0;

  virtual void CreateReferenceFrom(Handle, TextureType = TEXTURE_TYPE_2D) = 0;

  virtual void CopyTo(Texture *dst) = 0;

  virtual void Map(void **data, uint32_t *pitch, uint32_t flag) = 0;

  virtual void Unmap() = 0;

  virtual SRef<TextureMapData> Map(uint32_t flag) = 0;

  virtual Handle GetHandle() = 0;

  virtual TextureDesc GetDesc() const = 0;

  virtual Handle AsShaderResource() = 0;

  virtual Handle AsRenderTarget() = 0;
};
}  // namespace core
}  // namespace engine
}  // namespace ixr

#endif  // IXR_ENGINE_CORE_TEXTURE_TEXTURE_H_
