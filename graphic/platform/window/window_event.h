/********************************************************************
Copyright 2018 Intel Corp. All Rights Reserved.
Description : window events
Author      : Wenyi Tang
Email       : wenyi.tang@intel.com
Created     : Feb. 28th, 2018
********************************************************************/
#ifndef IXR_ENGINE_PLATFORM_WINDOW_WINDOW_EVENT_H_
#define IXR_ENGINE_PLATFORM_WINDOW_WINDOW_EVENT_H_
#include "engine/types.h"

namespace ixr {
namespace engine {

struct WindowEvent {
  uint32_t id;
  uint32_t type;
  uint64_t value[4];
  // A callback function f(id, type, value[4])
  std::function<void(uint32_t, uint32_t, uint64_t*)> callback;
};
}  // namespace engine
}  // namespace ixr
#endif  // IXR_ENGINE_PLATFORM_WINDOW_WINDOW_EVENT_H_
