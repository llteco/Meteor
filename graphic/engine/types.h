/********************************************************************
Copyright 2018 Intel Corp. All Rights Reserved.
Description : claim types
Author      : Wenyi Tang
Email       : wenyi.tang@intel.com
Created     : Feb. 28th, 2018
********************************************************************/
#ifndef IXR_ENGINE_ENGINE_TYPES_H_
#define IXR_ENGINE_ENGINE_TYPES_H_
#include <stdint.h>
#include <functional>
#include <map>
#include <memory>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

namespace ixr {
namespace engine {
using String = std::string;
using WString = std::wstring;

template <typename T>
using Ref = std::unique_ptr<T>;
template <typename T>
using SRef = std::shared_ptr<T>;
template <typename T>
using WRef = std::weak_ptr<T>;
template <typename T, class... U>
Ref<T> MakeRef(U &&... u) {
  return std::make_unique<T>(std::forward<U>(u)...);
}
template <typename T, class... U>
SRef<T> MakeSRef(U &&... u) {
  return std::make_shared<T>(std::forward<U>(u)...);
}

using Handle = void *;

/* Enumerate supported graphic apis */
enum GraphicApi {
  GRAPHIC_API_DX11,
  GRAPHIC_API_DX12,
  GRAPHIC_API_GL,
  GRAPHIC_API_VULKAN,
};

/* Enumerate adapter vendor id */
enum AdapterVendorId {
  ADAPTER_VENDOR_ANY = 0,
  ADAPTER_VENDOR_INTEL = 0x8086,
  ADAPTER_VENDOR_NVIDIA = 0x10DE,
  ADAPTER_VENDOR_AMD = 0x1002,
  ADAPTER_VENDOR_MSVC_DEBUGGER = 0x1414,
};

/* Enumerate shader languages */
enum ShaderType {
  SHADER_LANG_UNKNOWN,
  SHADER_LANG_HLSL,
  SHADER_LANG_GLSL,
};

struct NativeDeviceHandle {
  Handle pthis = nullptr;
  AdapterVendorId vid = ADAPTER_VENDOR_ANY;
  GraphicApi api = GRAPHIC_API_DX11;
  ShaderType shader = SHADER_LANG_HLSL;
};
}  // namespace engine
}  // namespace ixr

#endif  // IXR_ENGINE_ENGINE_TYPES_H_
