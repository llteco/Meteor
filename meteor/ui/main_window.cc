#include "engine/engine.h"
#include "imgui/imgui.h"
#include "imgui_meteor.h"
#include "meteor/ui/ui_window.h"

using namespace ixr;

engine::window::Window *CreateUIWindow(engine::Env *env) {
  engine::WindowDesc wd{
      "MeteorViewer", engine::WINDOW_STYLE_DEFAULT, 1000, 800, 0, 0};
  auto window = env->NewWindow(wd);
  engine::WindowEvent we{};
  we.value[3] = reinterpret_cast<uint64_t>(window->GetHandle());
  we.type = 0xFFFFFFFF;
  we.callback = [](uint32_t id, uint32_t msg, uint64_t *value) {
    if (ImGui_ImplWin32_WndProcHandler((HWND)value[3], msg, value[0], value[1]))
      return;
  };
  window->RegisterEvent(we);
  return window;
}

int main() {
  auto env = engine::Env::NewEnvironment(engine::GRAPHIC_API_DX11);
  auto window = CreateUIWindow(env);
  // Initialize Direct3D
  auto ui_renderer = env->NewRenderer();
  // Setup swap chain
  engine::SwapChainDesc sd;
  sd.format = engine::PF_RGBA32_UNORM;
  sd.window_handle = window->GetHandle();
  auto swapchain = env->NewSwapChain(sd);
  auto rt = swapchain->GetBuffer(0);

  // Show the window
  window->Show();

  ImGui::CreateContext();
  // Setup ImGui binding
  ID3D11Device *dev = static_cast<ID3D11Device *>(env->GetHandle().pthis);
  ImGui_ImplDX11_Init(window->GetHandle(), dev, nullptr);
  ImGui::StyleColorsDark();
  ImGui_ImplDX11_CreateDeviceObjects(env, ui_renderer);

  TitleBarArgs tb{};
  TitleBarInfo tbr;
  tb.name = "Meteor v0.0.0.1";
  tb.window_flag = ImGuiWindowFlags_NoMove |
    ImGuiWindowFlags_NoCollapse |
    ImGuiWindowFlags_NoResize;

  ImageViewerArgs iv{};
  ImageViewerInfo ivr;
  iv.enable = true;
  iv.max_frame = 1000;
  iv.window_flag = ImGuiWindowFlags_NoMove |
    ImGuiWindowFlags_NoCollapse |
    ImGuiWindowFlags_NoTitleBar |
    ImGuiWindowFlags_NoResize;

  ImageCompareArgs ic{};
  ImageCompareInfo icr;
  ic.enable = false;
  ic.window_flag = ImGuiWindowFlags_NoMove |
    ImGuiWindowFlags_NoCollapse |
    ImGuiWindowFlags_NoTitleBar |
    ImGuiWindowFlags_NoResize;

  StatusBarArgs sb{};
  sb.window_flag = ImGuiWindowFlags_NoMove |
    ImGuiWindowFlags_NoCollapse |
    ImGuiWindowFlags_NoTitleBar |
    ImGuiWindowFlags_NoResize;

  while (window->LoopState() != engine::WINDOW_STATE_HALT) {
    ImGui_ImplDX11_NewFrame(window->GetHandle());
    ImVec4 clear_color = ImColor(114, 144, 154);
    ui_renderer->SetRenderTargets({rt}, nullptr);
    ui_renderer->ClearRenderTarget(rt, (float *)&clear_color);
    tbr = TitleBar(tb);
    iv.enable = tbr.image_view;
    iv.parent_pos = tbr.pos;
    iv.parent_size = tbr.size;
    ic.enable = tbr.image_compare;
    ic.parent_pos = tbr.pos;
    ic.parent_size = tbr.size;
    if (iv.enable) {
      ivr = ImageViewer(iv);
      if (ivr.toggle_open) {
        iv.tex_id = ImGui::GetIO().Fonts->TexID;
      }
      sb.parent_pos = ivr.pos;
      sb.parent_size = ivr.size;
    }
    if (ic.enable) {
      icr = ImageCompare(ic);
      sb.parent_pos = icr.pos;
      sb.parent_size = icr.size;
    }
    StatusBar(sb);
    // ImGui::ShowDemoWindow();
    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData(), env, ui_renderer);
    swapchain->Present();
  }

  ImGui::DestroyContext();
  engine::Env::CleanupEnvironment(env);
}
