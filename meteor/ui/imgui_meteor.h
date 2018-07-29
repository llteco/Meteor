#ifndef METEOR_UI_IMGUI_METEOR_H_
#define METEOR_UI_IMGUI_METEOR_H_
#include <Windows.h>
#include <imgui/imgui.h>
#include "engine/engine.h"

struct ID3D11Device;
struct ID3D11DeviceContext;
using namespace ixr::engine;

bool ImGui_ImplDX11_Init(void* hwnd, ID3D11Device* device,
                         ID3D11DeviceContext* device_context);

bool ImGui_ImplDX11_CreateDeviceObjects(Env* env, core::Renderer* rd);
void ImGui_ImplDX11_NewFrame(void* hwnd);
void ImGui_ImplDX11_RenderDrawData(ImDrawData* draw_data, Env* env,
                                   core::Renderer* rd);

IMGUI_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg,
                                                 WPARAM wParam, LPARAM lParam);
#endif  // METEOR_UI_IMGUI_METEOR_H_
