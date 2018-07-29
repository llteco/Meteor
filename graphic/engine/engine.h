/********************************************************************
Copyright 2018 Intel Corp. All Rights Reserved.
Description : top instance of ixr engine
Author      : Wenyi Tang
Email       : wenyi.tang@intel.com
Created     : Feb. 28th, 2018
********************************************************************/
#ifndef IXR_ENGINE_ENGINE_ENGINE_H_
#define IXR_ENGINE_ENGINE_ENGINE_H_
#include "core/prerequisite_core.h"
#include "engine/types.h"
#include "export/export.h"
#include "platform/platform.h"

namespace ixr {
namespace engine {
class Env : public noncopyable {
 public:
  /**
   *  \brief Create an environment for the engine.
   *
   *  \param [in] GraphicApi \see ixr::GraphicApi, specify low level graphic api
   *  \param [in] AdapterVendorId \see ixr::AdapterVendorId, specify an adapter
   * by vid \return Return a pointer to Env instance
   *
   *  \details The engine implementation runs on an environment, AKA context.
   *  Engine instances should be created from Env, and CleanupEnvironment must
   * be called to avoid memory leak.
   */
  static IXR_ENGINE_API Env* NewEnvironment(
      GraphicApi, AdapterVendorId = ADAPTER_VENDOR_ANY);

  /**
   *  \brief Clean up an environment and destroy all resources allocated.
   *
   *  \param [in] env The pointer to Env which is returned by NewEnvironment.
   *
   *  \details All the instances created from this environment should not be
   * used any more.
   */
  static IXR_ENGINE_API void CleanupEnvironment(Env* env);

  class Advanced {
   public:
    /**
     *  \brief Specify the log level to be reported
     *
     *  \param [in] level from 0~5, where 0 is verbose and 5 is critical
     */
    static void IXR_ENGINE_API SetLogLevel(int level);
  };

  virtual NativeDeviceHandle GetHandle() const = 0;
  virtual void ChangeEnv(NativeDeviceHandle h) = 0;
  virtual window::Window* NewWindow(const WindowDesc&) = 0;
  virtual core::GpuBuffer* NewGpuBuffer(const BufferDesc&) = 0;
  virtual core::Renderer* NewRenderer() = 0;
  virtual core::Shader* NewShader() = 0;
  virtual core::SwapChain* NewSwapChain(const SwapChainDesc&) = 0;
  virtual core::Texture* NewTexture(const TextureDesc&) = 0;

  virtual void ReleaseObject(void*) = 0;
};
}  // namespace engine
}  // namespace ixr
#endif  // IXR_ENGINE_ENGINE_ENGINE_H_
