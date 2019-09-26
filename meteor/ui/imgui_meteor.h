/****************************************
 * Copyright (c) 2019 Wenyi Tang
 * Author: Wenyi Tang
 * E-mail: wenyitang@outlook.com
 * Description: Adapter to LL_GFX for IMGUI
 ****************************************/
#ifndef METEOR_UI_IMGUI_METEOR_H_
#define METEOR_UI_IMGUI_METEOR_H_
#include <Windows.h>
#include <imgui/imgui.h>
#include "ll_graphic/engine/engine.h"

struct ID3D11Device;
struct ID3D11DeviceContext;

bool ImGui_ImplDX11_Init(void* hwnd, ID3D11Device* device,
                         ID3D11DeviceContext* device_context);

bool ImGui_ImplDX11_CreateDeviceObjects(ll::engine::Env* env,
                                        ll::engine::core::Renderer* rd);

void ImGui_ImplDX11_NewFrame(void* hwnd);

void ImGui_ImplDX11_RenderDrawData(ImDrawData* draw_data, ll::engine::Env* env,
                                   ll::engine::core::Renderer* rd);

IMGUI_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg,
                                                 WPARAM wParam, LPARAM lParam);
#endif  // METEOR_UI_IMGUI_METEOR_H_
