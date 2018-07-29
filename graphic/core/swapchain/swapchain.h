/********************************************************************
Copyright 2018 Intel Corp. All Rights Reserved.
Description : swap-chain interface
Author      : Wenyi Tang
Email       : wenyi.tang@intel.com
Created     : Mar. 6th, 2018
********************************************************************/
#ifndef IXR_ENGINE_CORE_SWAPCHAIN_SWAPCHAIN_H_
#define IXR_ENGINE_CORE_SWAPCHAIN_SWAPCHAIN_H_
#include "core/texture/texture.h"
#include "engine/types.h"
#include "platform/native_object.h"

namespace ixr {
namespace engine {

/* Swapchain description */
struct SwapChainDesc {
  /** The window on which to display the surface */
  Handle window_handle;
  /** The pixel format of the surface */
  PixelFormat format;
  /** The surface width, set 0 to derive from the window */
  uint32_t width = 0;
  /** The surface height, set 0 to derive from the window */
  uint32_t height = 0;
  /** Whether to be full screen or windowed */
  uint8_t windowed = true;
  /** Multi-sample count */
  uint32_t sample_count = 1;
  /** The total buffers in the swapchain, always 2 */
  uint32_t buffer_count = 2;
  /** The refresh rate of the output */
  uint32_t refresh_rate = 60;
};

namespace core {
/** Abstract class for swapchain.
 * A swapchain is used to swap back/front buffers of the surface
 */
class SwapChain : public Object {
 public:
  /** Create the swapchain.
   * You should create an instance before calling any other API
   */
  virtual void CreateSwapChain(const SwapChainDesc &sd) = 0;

  /** Get the back buffer of the surface.
   * \return: back buffer pointer
   */
  virtual Texture *GetBuffer(uint32_t index) = 0;

  /** Swap the buffer and display on the surface */
  virtual void Present(bool vsync = true) = 0;

  /** Resize the surface buffers */
  virtual void Resize(uint32_t w, uint32_t h) = 0;

  /** Get the width and height and w/h of the buffer */
  virtual void GetSize(uint32_t *w, uint32_t *h, float *ratio) = 0;
};
}  // namespace core
}  // namespace engine
}  // namespace ixr
#endif  // IXR_ENGINE_CORE_SWAPCHAIN_SWAPCHAIN_H_
