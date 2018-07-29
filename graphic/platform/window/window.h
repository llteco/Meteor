/********************************************************************
Copyright 2018 Intel Corp. All Rights Reserved.
Description : window ui
Author      : Wenyi Tang
Email       : wenyi.tang@intel.com
Created     : Feb. 28th, 2018
********************************************************************/
#ifndef IXR_ENGINE_PLATFORM_WINDOW_WINDOW_H_
#define IXR_ENGINE_PLATFORM_WINDOW_WINDOW_H_
#include "engine/types.h"
#include "errors/error.h"
#include "platform/native_object.h"
#include "platform/noncopyable.h"
#include "platform/window/window_event.h"

namespace ixr {
namespace engine {
enum WindowStyle {
  WINDOW_STYLE_OVERLAPPED = 0x00000000L,
  WINDOW_STYLE_POPUP = 0x80000000L,
  WINDOW_STYLE_CHILD = 0x40000000L,
  WINDOW_STYLE_MINIMIZE = 0x20000000L,
  WINDOW_STYLE_VISIBLE = 0x10000000L,
  WINDOW_STYLE_DISABLED = 0x08000000L,
  WINDOW_STYLE_CLIPSIBLINGS = 0x04000000L,
  WINDOW_STYLE_CLIPCHILDREN = 0x02000000L,
  WINDOW_STYLE_MAXIMIZE = 0x01000000L,
  WINDOW_STYLE_CAPTION = 0x00C00000L, /* WS_BORDER | WS_DLGFRAME  */
  WINDOW_STYLE_BORDER = 0x00800000L,
  WINDOW_STYLE_DLGFRAME = 0x00400000L,
  WINDOW_STYLE_VSCROLL = 0x00200000L,
  WINDOW_STYLE_HSCROLL = 0x00100000L,
  WINDOW_STYLE_SYSMENU = 0x00080000L,
  WINDOW_STYLE_THICKFRAME = 0x00040000L,
  WINDOW_STYLE_GROUP = 0x00020000L,
  WINDOW_STYLE_TABSTOP = 0x00010000L,
  WINDOW_STYLE_MINIMIZEBOX = 0x00020000L,
  WINDOW_STYLE_MAXIMIZEBOX = 0x00010000L,
  WINDOW_STYLE_DEFAULT = WINDOW_STYLE_OVERLAPPED | WINDOW_STYLE_CAPTION |
                         WINDOW_STYLE_SYSMENU | WINDOW_STYLE_THICKFRAME |
                         WINDOW_STYLE_MINIMIZEBOX | WINDOW_STYLE_MAXIMIZEBOX
};

enum WindowState {
  WINDOW_STATE_HALT,
  WINDOW_STATE_RUN,
  WINDOW_STATE_MINIMUM,
};

struct WindowDesc {
  String name;
  uint32_t style;
  int width;
  int height;
  int start_x = 0;
  int start_y = 0;
};

namespace window {
class Window : public noncopyable, public Object {
 public:
  virtual void Create(const WindowDesc &desc) = 0;

  virtual void Show() = 0;
  virtual void Hide() = 0;

  virtual Handle GetHandle() const = 0;

  virtual WindowState LoopState() = 0;

  virtual void RegisterEvent(const WindowEvent &e) = 0;
};
}  // namespace window
}  // namespace engine
}  // namespace ixr
#endif  // IXR_ENGINE_PLATFORM_WINDOW_WINDOW_H_
