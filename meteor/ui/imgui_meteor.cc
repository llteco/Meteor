#include "imgui_meteor.h"

static ImGuiMouseCursor g_LastMouseCursor = ImGuiMouseCursor_COUNT;

bool ImGui_ImplDX11_Init(void* hwnd, ID3D11Device* device,
                         ID3D11DeviceContext* device_context) {
  // Setup back-end capabilities flags
  ImGuiIO& io = ImGui::GetIO();
  io.BackendFlags |=
      ImGuiBackendFlags_HasMouseCursors;  // We can honor GetMouseCursor()
                                          // values (optional)
  io.BackendFlags |=
      ImGuiBackendFlags_HasSetMousePos;  // We can honor io.WantSetMousePos
                                         // requests (optional, rarely used)

  // Keyboard mapping. ImGui will use those indices to peek into the
  // io.KeyDown[] array that we will update during the application lifetime.
  io.KeyMap[ImGuiKey_Tab] = VK_TAB;
  io.KeyMap[ImGuiKey_LeftArrow] = VK_LEFT;
  io.KeyMap[ImGuiKey_RightArrow] = VK_RIGHT;
  io.KeyMap[ImGuiKey_UpArrow] = VK_UP;
  io.KeyMap[ImGuiKey_DownArrow] = VK_DOWN;
  io.KeyMap[ImGuiKey_PageUp] = VK_PRIOR;
  io.KeyMap[ImGuiKey_PageDown] = VK_NEXT;
  io.KeyMap[ImGuiKey_Home] = VK_HOME;
  io.KeyMap[ImGuiKey_End] = VK_END;
  io.KeyMap[ImGuiKey_Insert] = VK_INSERT;
  io.KeyMap[ImGuiKey_Delete] = VK_DELETE;
  io.KeyMap[ImGuiKey_Backspace] = VK_BACK;
  io.KeyMap[ImGuiKey_Space] = VK_SPACE;
  io.KeyMap[ImGuiKey_Enter] = VK_RETURN;
  io.KeyMap[ImGuiKey_Escape] = VK_ESCAPE;
  io.KeyMap[ImGuiKey_A] = 'A';
  io.KeyMap[ImGuiKey_C] = 'C';
  io.KeyMap[ImGuiKey_V] = 'V';
  io.KeyMap[ImGuiKey_X] = 'X';
  io.KeyMap[ImGuiKey_Y] = 'Y';
  io.KeyMap[ImGuiKey_Z] = 'Z';

  io.ImeWindowHandle = hwnd;

  return true;
}

bool ImGui_ImplDX11_CreateDeviceObjects(Env* env, core::Renderer* rd) {
  static const char* vertexShader =
      "cbuffer vertexBuffer : register(b0) \
            {\
            float4x4 ProjectionMatrix; \
            };\
            struct VS_INPUT\
            {\
            float2 pos : POSITION;\
            float4 col : COLOR0;\
            float2 uv  : TEXCOORD0;\
            };\
            \
            struct PS_INPUT\
            {\
            float4 pos : SV_POSITION;\
            float4 col : COLOR0;\
            float2 uv  : TEXCOORD0;\
            };\
            \
            PS_INPUT main(VS_INPUT input)\
            {\
            PS_INPUT output;\
            output.pos = mul( ProjectionMatrix, float4(input.pos.xy, 0.f, 1.f));\
            output.col = input.col;\
            output.uv  = input.uv;\
            return output;\
            }";
  static const char* pixelShader =
      "struct PS_INPUT\
            {\
            float4 pos : SV_POSITION;\
            float4 col : COLOR0;\
            float2 uv  : TEXCOORD0;\
            };\
            sampler sampler0;\
            Texture2D texture0;\
            \
            float4 main(PS_INPUT input) : SV_Target\
            {\
            float4 out_col = input.col * texture0.Sample(sampler0, input.uv); \
            return out_col; \
            }";
  auto vs = env->NewShader();
  auto ps = env->NewShader();
  vs->Compile(vertexShader, strlen(vertexShader), SHADER_TARGET_VERTEX);
  ps->Compile(pixelShader, strlen(pixelShader), SHADER_TARGET_PIXEL);
  core::VertexElement v1("POSITION", VET_FLOAT2), v2("TEXCOORD", VET_FLOAT2),
      v3("COLOR", VET_UINT3);
  rd->SetInputLayout({v1, v2, v3}, vs);
  rd->SetShader(vs);
  rd->SetShader(ps);

  const float blending[4]{0.f, 0.f, 0.f, 0.f};
  rd->SetBlendState(blending, 0xFFFFFFFF);
  rd->SetDepthStencil();
  rd->SetRasterizer(RS_CULL_NONE | RS_FILL_SOLID | RS_SCISSOR_ENABLE);

  // Build texture atlas
  ImGuiIO& io = ImGui::GetIO();
  unsigned char* pixels;
  int width, height;
  io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);

  TextureDesc td;
  td.width = width;
  td.height = height;
  td.format = PF_RGBA32_UNORM;
  td.usage = TU_SHADERRESOURCE;
  td.data = pixels;
  auto font_tex = env->NewTexture(td);

  // Store our identifier
  io.Fonts->TexID = (void*)font_tex->AsShaderResource();
  rd->SetSampler();
  return true;
}

static bool ImGui_ImplWin32_UpdateMouseCursor() {
  ImGuiIO& io = ImGui::GetIO();
  if (io.ConfigFlags & ImGuiConfigFlags_NoMouseCursorChange) return false;

  ImGuiMouseCursor imgui_cursor =
      io.MouseDrawCursor ? ImGuiMouseCursor_None : ImGui::GetMouseCursor();
  if (imgui_cursor == ImGuiMouseCursor_None) {
    // Hide OS mouse cursor if imgui is drawing it or if it wants no cursor
    ::SetCursor(NULL);
  } else {
    // Hardware cursor type
    LPTSTR win32_cursor = IDC_ARROW;
    switch (imgui_cursor) {
      case ImGuiMouseCursor_Arrow:
        win32_cursor = IDC_ARROW;
        break;
      case ImGuiMouseCursor_TextInput:
        win32_cursor = IDC_IBEAM;
        break;
      case ImGuiMouseCursor_ResizeAll:
        win32_cursor = IDC_SIZEALL;
        break;
      case ImGuiMouseCursor_ResizeEW:
        win32_cursor = IDC_SIZEWE;
        break;
      case ImGuiMouseCursor_ResizeNS:
        win32_cursor = IDC_SIZENS;
        break;
      case ImGuiMouseCursor_ResizeNESW:
        win32_cursor = IDC_SIZENESW;
        break;
      case ImGuiMouseCursor_ResizeNWSE:
        win32_cursor = IDC_SIZENWSE;
        break;
    }
    ::SetCursor(::LoadCursor(NULL, win32_cursor));
  }
  return true;
}

void ImGui_ImplDX11_NewFrame(void* hwnd) {
  HWND g_hWnd = (HWND)hwnd;
  ImGuiIO& io = ImGui::GetIO();

  // Setup display size (every frame to accommodate for window resizing)
  RECT rect;
  GetClientRect(g_hWnd, &rect);
  io.DisplaySize =
      ImVec2((float)(rect.right - rect.left), (float)(rect.bottom - rect.top));

  // Setup time step
  io.DeltaTime = 1 / 60.f;

  // Read keyboard modifiers inputs
  io.KeyCtrl = (GetKeyState(VK_CONTROL) & 0x8000) != 0;
  io.KeyShift = (GetKeyState(VK_SHIFT) & 0x8000) != 0;
  io.KeyAlt = (GetKeyState(VK_MENU) & 0x8000) != 0;
  io.KeySuper = false;
  // io.KeysDown : filled by WM_KEYDOWN/WM_KEYUP events
  // io.MousePos : filled by WM_MOUSEMOVE events
  // io.MouseDown : filled by WM_*BUTTON* events
  // io.MouseWheel : filled by WM_MOUSEWHEEL events

  // Set OS mouse position if requested (only used when
  // ImGuiConfigFlags_NavEnableSetMousePos is enabled by user)
  if (io.WantSetMousePos) {
    POINT pos = {(int)io.MousePos.x, (int)io.MousePos.y};
    ClientToScreen(g_hWnd, &pos);
    SetCursorPos(pos.x, pos.y);
  }

  // Update OS mouse cursor with the cursor requested by imgui
  ImGuiMouseCursor mouse_cursor =
      io.MouseDrawCursor ? ImGuiMouseCursor_None : ImGui::GetMouseCursor();
  if (g_LastMouseCursor != mouse_cursor) {
    g_LastMouseCursor = mouse_cursor;
    ImGui_ImplWin32_UpdateMouseCursor();
  }

  // Start the frame. This call will update the io.WantCaptureMouse,
  // io.WantCaptureKeyboard flag that you can use to dispatch inputs (or not) to
  // your application.
  ImGui::NewFrame();
}

static UINT g_VertexBufferSize = 5000;
static UINT g_IndexBufferSize = 10000;
static core::GpuBuffer *g_pVB, *g_pIB;

void ImGui_ImplDX11_RenderDrawData(ImDrawData* draw_data, Env* env,
                                   core::Renderer* rd) {
  // Create and grow vertex/index buffers if needed
  if (!g_pVB || g_VertexBufferSize < draw_data->TotalVtxCount) {
    if (g_pVB) {
      env->ReleaseObject(g_pVB);
      g_pVB = NULL;
    }
    g_VertexBufferSize = draw_data->TotalVtxCount + 5000;
    BufferDesc bd{};
    bd.usage = BUFFER_USAGE_VERTEX | BUFFER_USAGE_CPUWRITE;
    bd.size = g_VertexBufferSize * sizeof(ImDrawVert);
    g_pVB = env->NewGpuBuffer(bd);
  }
  if (!g_pIB || g_IndexBufferSize < draw_data->TotalIdxCount) {
    if (g_pIB) {
      env->ReleaseObject(g_pIB);
      g_pIB = NULL;
    }
    g_IndexBufferSize = draw_data->TotalIdxCount + 10000;
    BufferDesc bd{};
    bd.usage = BUFFER_USAGE_INDICE | BUFFER_USAGE_CPUWRITE;
    bd.size = g_IndexBufferSize * sizeof(ImDrawIdx);
    g_pIB = env->NewGpuBuffer(bd);
  }

  {
    auto vtx = g_pVB->Map();
    auto idx = g_pIB->Map();
    for (int n = 0; n < draw_data->CmdListsCount; n++) {
      const ImDrawList* cmd_list = draw_data->CmdLists[n];
      vtx->UpdateFromRawData(cmd_list->VtxBuffer.Data,
                             n * cmd_list->VtxBuffer.Size,
                             cmd_list->VtxBuffer.Size * sizeof(ImDrawVert));
      idx->UpdateFromRawData(cmd_list->IdxBuffer.Data,
                             n * cmd_list->IdxBuffer.Size,
                             cmd_list->IdxBuffer.Size * sizeof(ImDrawIdx));
    }
  }
  float L = 0.0f;
  float R = ImGui::GetIO().DisplaySize.x;
  float B = ImGui::GetIO().DisplaySize.y;
  float T = 0.0f;
  float mvp[16] = {
      2.0f / (R - L),
      0.0f,
      0.0f,
      0.0f,
      0.0f,
      2.0f / (T - B),
      0.0f,
      0.0f,
      0.0f,
      0.0f,
      0.5f,
      0.0f,
      (R + L) / (L - R),
      (T + B) / (B - T),
      0.5f,
      1.0f,
  };
  rd->SetModelViewPerspectiveProjection(mvp);
  rd->SetViewport(ImGui::GetIO().DisplaySize.x, ImGui::GetIO().DisplaySize.y, 0,
                  0, 0, 1.f);
  rd->SetVertexAndIndex(g_pVB, sizeof(ImDrawVert), g_pIB);
  // Render command lists
  int vtx_offset = 0;
  int idx_offset = 0;
  for (int n = 0; n < draw_data->CmdListsCount; n++) {
    const ImDrawList* cmd_list = draw_data->CmdLists[n];
    for (int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; cmd_i++) {
      const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[cmd_i];
      if (pcmd->UserCallback) {
        pcmd->UserCallback(cmd_list, pcmd);
      } else {
        TextureDesc td;
        td.ref = pcmd->TextureId;
        td.type = TEXTURE_TYPE_VIEW;
        rd->SetShaderResources(env->NewTexture(td));
        rd->SetScissor((INT)pcmd->ClipRect.x, (INT)pcmd->ClipRect.y,
                       (INT)pcmd->ClipRect.z, (INT)pcmd->ClipRect.w);
        rd->Draw(pcmd->ElemCount, idx_offset, vtx_offset);
      }
      idx_offset += pcmd->ElemCount;
    }
    vtx_offset += cmd_list->VtxBuffer.Size;
  }
}

// Allow compilation with old Windows SDK. MinGW doesn't have default
// _WIN32_WINNT/WINVER versions.
#ifndef WM_MOUSEHWHEEL
#define WM_MOUSEHWHEEL 0x020E
#endif

// Process Win32 mouse/keyboard inputs.
// You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if
// dear imgui wants to use your inputs.
// - When io.WantCaptureMouse is true, do not dispatch mouse input data to your
// main application.
// - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to
// your main application. Generally you may always pass all inputs to dear
// imgui, and hide them from your application based on those two flags. PS: In
// this Win32 handler, we use the capture API
// (GetCapture/SetCapture/ReleaseCapture) to be able to read mouse coordinations
// when dragging mouse outside of our window bounds. PS: We treat DBLCLK
// messages as regular mouse down messages, so this code will work on windows
// classes that have the CS_DBLCLKS flag set. Our own example app code doesn't
// set this flag.
IMGUI_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg,
                                                 WPARAM wParam, LPARAM lParam) {
  if (ImGui::GetCurrentContext() == NULL) return 0;

  ImGuiIO& io = ImGui::GetIO();
  switch (msg) {
    case WM_LBUTTONDOWN:
    case WM_LBUTTONDBLCLK:
    case WM_RBUTTONDOWN:
    case WM_RBUTTONDBLCLK:
    case WM_MBUTTONDOWN:
    case WM_MBUTTONDBLCLK: {
      int button = 0;
      if (msg == WM_LBUTTONDOWN || msg == WM_LBUTTONDBLCLK) button = 0;
      if (msg == WM_RBUTTONDOWN || msg == WM_RBUTTONDBLCLK) button = 1;
      if (msg == WM_MBUTTONDOWN || msg == WM_MBUTTONDBLCLK) button = 2;
      if (!ImGui::IsAnyMouseDown() && ::GetCapture() == NULL)
        ::SetCapture(hwnd);
      io.MouseDown[button] = true;
      return 0;
    }
    case WM_LBUTTONUP:
    case WM_RBUTTONUP:
    case WM_MBUTTONUP: {
      int button = 0;
      if (msg == WM_LBUTTONUP) button = 0;
      if (msg == WM_RBUTTONUP) button = 1;
      if (msg == WM_MBUTTONUP) button = 2;
      io.MouseDown[button] = false;
      if (!ImGui::IsAnyMouseDown() && ::GetCapture() == hwnd)
        ::ReleaseCapture();
      return 0;
    }
    case WM_MOUSEWHEEL:
      io.MouseWheel += GET_WHEEL_DELTA_WPARAM(wParam) > 0 ? +1.0f : -1.0f;
      return 0;
    case WM_MOUSEHWHEEL:
      io.MouseWheelH += GET_WHEEL_DELTA_WPARAM(wParam) > 0 ? +1.0f : -1.0f;
      return 0;
    case WM_MOUSEMOVE:
      io.MousePos.x = (signed short)(lParam);
      io.MousePos.y = (signed short)(lParam >> 16);
      return 0;
    case WM_KEYDOWN:
    case WM_SYSKEYDOWN:
      if (wParam < 256) io.KeysDown[wParam] = 1;
      return 0;
    case WM_KEYUP:
    case WM_SYSKEYUP:
      if (wParam < 256) io.KeysDown[wParam] = 0;
      return 0;
    case WM_CHAR:
      // You can also use ToAscii()+GetKeyboardState() to retrieve characters.
      if (wParam > 0 && wParam < 0x10000)
        io.AddInputCharacter((unsigned short)wParam);
      return 0;
    case WM_SETCURSOR:
      if (LOWORD(lParam) == HTCLIENT && ImGui_ImplWin32_UpdateMouseCursor())
        return 1;
      return 0;
  }
  return 0;
}