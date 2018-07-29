/********************************************************************
Copyright 2018 Intel Corp. All Rights Reserved.
Description : top instance of ixr engine
Author      : Wenyi Tang
Email       : wenyi.tang@intel.com
Created     : Feb. 28th, 2018
********************************************************************/
#include "engine/engine.h"
#include "core/shader/shader_hlsl.h"  // @Todo
#include "errors/error.h"
#include "low_level/dx11/prerequisite_dx11.h"
#include "low_level/vulkan/prerequisite_vk.h"
#include "platform/window/window_win32.h"

namespace ixr {
namespace engine {

using namespace core;
using namespace window;
using namespace errors;

class EnvInternal : public Env {
 public:
  virtual NativeDeviceHandle GetHandle() const override;
  virtual void ChangeEnv(NativeDeviceHandle h) override;
  virtual Window *NewWindow(const WindowDesc &wd) override;
  virtual GpuBuffer *NewGpuBuffer(const BufferDesc &bd) override;
  virtual Renderer *NewRenderer() override;
  virtual Shader *NewShader() override;
  virtual SwapChain *NewSwapChain(const SwapChainDesc &scd) override;
  virtual Texture *NewTexture(const TextureDesc &td) override;
  virtual void ReleaseObject(void *) override;

 public:
  NativeDeviceHandle Handle;
  std::list<Object *> Objects;
};

Env *Env::NewEnvironment(GraphicApi api, AdapterVendorId vid) {
  auto p = new EnvInternal();
  p->Handle.api = api;
  p->Handle.vid = vid;
  switch (api) {
    case GRAPHIC_API_DX11:
    case GRAPHIC_API_DX12:
      p->Handle.shader = SHADER_LANG_HLSL;
      break;
    case GRAPHIC_API_GL:
      p->Handle.shader = SHADER_LANG_GLSL;
      break;
    case GRAPHIC_API_VULKAN:
    default:
      p->Handle.shader = SHADER_LANG_UNKNOWN;
      break;
  }
  return p;
}

void Env::CleanupEnvironment(Env *env) {
  EnvInternal *p = static_cast<EnvInternal *>(env);
  for (auto &o : p->Objects) {
    o->Release();
  }
  delete p;
}

void Env::Advanced::SetLogLevel(int level) {
#ifdef SPDLOG_VERSION
  auto logger = spdlog::stdout_color_mt("IXRengine");
  logger->set_level(spdlog::level::level_enum(level));
#endif
}

NativeDeviceHandle EnvInternal::GetHandle() const { return Handle; }

void EnvInternal::ChangeEnv(NativeDeviceHandle h) { Handle = h; }

Window *EnvInternal::NewWindow(const WindowDesc &wd) {
#ifdef WIN32
  auto p = new Win32Window();
#endif
  p->Create(wd);
  Objects.push_back(p);
  return p;
}

GpuBuffer *EnvInternal::NewGpuBuffer(const BufferDesc &bd) {
  GpuBuffer *p = nullptr;
  if (!Handle.pthis) return p;
  switch (Handle.api) {
    case GRAPHIC_API_DX11:
#ifdef IXR_ENGINE_INCLUDE_DIRECTX11
      p = new dx11::Dx11GpuBuffer(Handle);
      break;
#endif
    case GRAPHIC_API_DX12:
#ifdef IXR_ENGINE_INCLUDE_DIRECTX12
      p = new Dx12GpuBuffer(Handle);
      break;
#endif
    case GRAPHIC_API_GL:
#ifdef IXR_ENGINE_INCLUDE_OPENGL
      p = new GlGpuBuffer(Handle);
      break;
#endif
    case GRAPHIC_API_VULKAN:
#ifdef IXR_ENGINE_INCLUDE_VULKAN
      p = new VulkanGpuBuffer(Handle);
      break;
#endif
    default:
      break;
  }
  if (p) {
    p->CreateBuffer(bd);
    Objects.push_back(p);
  }
  return p;
}

Renderer *EnvInternal::NewRenderer() {
  Renderer *p = nullptr;
  switch (Handle.api) {
    case GRAPHIC_API_DX11:
#ifdef IXR_ENGINE_INCLUDE_DIRECTX11
      p = new dx11::Dx11Renderer(this);
      break;
#endif
    case GRAPHIC_API_DX12:
#ifdef IXR_ENGINE_INCLUDE_DIRECTX12
      p = new Dx12Renderer(this);
      break;
#endif
    case GRAPHIC_API_GL:
#ifdef IXR_ENGINE_INCLUDE_OPENGL
      p = new GlRenderer(this);
      break;
#endif
    case GRAPHIC_API_VULKAN:
#ifdef IXR_ENGINE_INCLUDE_VULKAN
      p = new VulkanRenderer(this);
      break;
#endif
    default:
      break;
  }
  if (p) {
    p->CreateDevice(Handle.vid);
    Objects.push_back(p);
  }
  return p;
}

Shader *EnvInternal::NewShader() {
  Shader *p = nullptr;
  switch (Handle.shader) {
    case SHADER_LANG_HLSL:
      p = new ShaderHLSL();
      break;
    case SHADER_LANG_GLSL:
      break;
    default:
      break;
  }
  if (p) {
    Objects.push_back(p);
  }
  return p;
}

SwapChain *EnvInternal::NewSwapChain(const SwapChainDesc &scd) {
  SwapChain *p = nullptr;
  if (!Handle.pthis) return p;
  switch (Handle.api) {
    case GRAPHIC_API_DX11:
#ifdef IXR_ENGINE_INCLUDE_DIRECTX11
      p = new dx11::Dx11SwapChain(this);
      break;
#endif
    case GRAPHIC_API_DX12:
#ifdef IXR_ENGINE_INCLUDE_DIRECTX12
      p = new Dx12SwapChain(this);
      break;
#endif
    case GRAPHIC_API_GL:
#ifdef IXR_ENGINE_INCLUDE_OPENGL
      p = new GlSwapChain(this);
      break;
#endif
    case GRAPHIC_API_VULKAN:
#ifdef IXR_ENGINE_INCLUDE_VULKAN
      p = new VulkanSwapChain(this);
      break;
#endif
    default:
      break;
  }
  if (p) {
    p->CreateSwapChain(scd);
    Objects.push_back(p);
  }
  return p;
}

Texture *EnvInternal::NewTexture(const TextureDesc &td) {
  Texture *p = nullptr;
  if (!Handle.pthis) return p;
  switch (Handle.api) {
    case GRAPHIC_API_DX11:
#ifdef IXR_ENGINE_INCLUDE_DIRECTX11
      p = new dx11::Dx11Texture(this);
      break;
#endif
    case GRAPHIC_API_DX12:
#ifdef IXR_ENGINE_INCLUDE_DIRECTX12
      p = new Dx12Texture(this);
      break;
#endif
    case GRAPHIC_API_GL:
#ifdef IXR_ENGINE_INCLUDE_OPENGL
      p = new GlTexture(this);
      break;
#endif
    case GRAPHIC_API_VULKAN:
#ifdef IXR_ENGINE_INCLUDE_VULKAN
      p = new VulkanTexture(this);
      break;
#endif
    default:
      break;
  }
  if (p) {
    if (td.ref) {
      p->CreateReferenceFrom(td.ref, td.type);
    } else {
      p->CreateTexture(td);
    }
    Objects.push_back(p);
  }
  return p;
}

void EnvInternal::ReleaseObject(void *obj) {
  if (obj) {
    Object *p = static_cast<Object *>(obj);
    auto index = std::find(Objects.begin(), Objects.end(), p);
    if (index != Objects.end()) {
      Objects.erase(index);
      p->Release();
    }
  }
}
}  // namespace engine
}  // namespace ixr
