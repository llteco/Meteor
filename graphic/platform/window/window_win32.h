/********************************************************************
Copyright 2018 Intel Corp. All Rights Reserved.
Description : window on Windows
Author      : Wenyi Tang
Email       : wenyi.tang@intel.com
Created     : Feb. 28th, 2018
********************************************************************/
#ifndef IXR_ENGINE_PLATFORM_WINDOW_WINDOW_WIN32_H_
#define IXR_ENGINE_PLATFORM_WINDOW_WINDOW_WIN32_H_
#include <Windows.h>
#include <deque>
#include "engine/types.h"
#include "platform/window/window.h"

namespace ixr {
namespace engine {
namespace window {

/** Win32 windows helper */
class Win32Window : public Window {
 public:
  using EventList = std::deque<WindowEvent>;

  Win32Window();
  virtual ~Win32Window();

  virtual void Create(const WindowDesc &desc) override;

  virtual void Show() override;
  virtual void Hide() override;

  virtual void SetPos(int x, int y) override;
  virtual void Centered() override;

  virtual Handle GetHandle() const override;

  virtual WindowState LoopState() override;

  virtual void RegisterEvent(const WindowEvent &e) override;

  void EventHandler(WindowEvent *event);

  friend static LRESULT CALLBACK MainWindowCallback(HWND hWnd, UINT Msg,
                                                    WPARAM wParam,
                                                    LPARAM lParam);

 private:
  HWND handle_;
  String class_token_;
  WindowState state_;
  EventList events_;
};
}  // namespace window
}  // namespace engine
}  // namespace ixr

#endif  // IXR_ENGINE_PLATFORM_WINDOW_WINDOW_WIN32_H_
