/********************************************************************
Copyright 2018 Intel Corp. All Rights Reserved.
Description : window on Windows
Author      : Wenyi Tang
Email       : wenyi.tang@intel.com
Created     : Feb. 28th, 2018
********************************************************************/
#include "platform/window/window_win32.h"

namespace ixr {
namespace engine {
namespace window {

/// Window callback function
static LRESULT CALLBACK MainWindowCallback(HWND hWnd, UINT Msg, WPARAM wParam,
                                           LPARAM lParam) {
  auto p = reinterpret_cast<Win32Window *>(GetWindowLongPtr(hWnd, 0));
  if (p) {
    for (auto &e : p->events_) {
      WindowEvent event = e;
      event.id = e.id;
      event.type &= Msg;
      event.value[0] = wParam;
      event.value[1] = lParam;
      p->EventHandler(&event);
    }
  }
  return DefWindowProcW(hWnd, Msg, wParam, lParam);
}

Win32Window::Win32Window() { handle_ = nullptr; }

Win32Window::~Win32Window() {
  if (handle_) {
    DestroyWindow(handle_);
    UnregisterClassA(class_token_.c_str(), NULL);
    handle_ = nullptr;
  }
}

void Win32Window::Create(const WindowDesc &desc) {
  HINSTANCE ins = GetModuleHandle(NULL);
  class_token_ = desc.name;
  std::wstring wname(desc.name.cbegin(), desc.name.cend());

  WNDCLASS win_class{};
  win_class.lpszClassName = wname.c_str();
  win_class.style = CS_OWNDC;
  win_class.lpfnWndProc = MainWindowCallback;
  win_class.cbWndExtra = sizeof(*this);
  RegisterClass(&win_class);
  /// adjust the window size and show at InitDevice time
  // During CreateWindow, the callback WindowProc is called
  // several times and must return true.
  handle_ = CreateWindowEx(0, win_class.lpszClassName, wname.c_str(),
                           desc.style, desc.start_x, desc.start_y, desc.width,
                           desc.height, NULL, NULL, ins, NULL);

  SetWindowLongPtr(handle_, 0, reinterpret_cast<LONG_PTR>(this));
  // Set a default event
  RegisterEvent({0, 0xFFFFFFFF, 0, 0, 0, 0,
                 [this](uint32_t id, uint32_t type, uint64_t *value) {
                   switch (type) {
                     case WM_KEYDOWN:
                       if (value[0] == VK_ESCAPE) {
                         // Quit if press ESC
                         state_ = WINDOW_STATE_HALT;
                       }
                       break;
                     case WM_KEYUP:
                       break;
                     case WM_DESTROY:
                     case WM_QUIT:
                       state_ = WINDOW_STATE_HALT;
                       break;
                     case WM_SIZE:
                       if (value[0] == SIZE_MINIMIZED)
                         state_ = WINDOW_STATE_MINIMUM;
                       else
                         state_ = WINDOW_STATE_RUN;
                       break;
                     default:
                       break;
                   }
                 }});
}

void Win32Window::Show() {
  ShowWindow(handle_, SW_SHOW);
  state_ = WINDOW_STATE_RUN;
}

void Win32Window::Hide() {
  ShowWindow(handle_, SW_HIDE);
  state_ = WINDOW_STATE_MINIMUM;
}

Handle Win32Window::GetHandle() const { return Handle(handle_); }

WindowState Win32Window::LoopState() {
  MSG msg;
  while (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE)) {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
    // Receive a quit message
    if (msg.message == WM_QUIT) state_ = WINDOW_STATE_HALT;
  }
  return state_;
}

void Win32Window::RegisterEvent(const WindowEvent &e) { events_.push_back(e); }

void Win32Window::EventHandler(WindowEvent *event) {
  if (event->callback) {
    event->callback(event->id, event->type, event->value);
  }
}
}  // namespace window
}  // namespace engine
}  // namespace ixr