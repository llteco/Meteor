#include "engine/engine.h"
#include "imgui/imgui.h"
#include "imgui_meteor.h"
//#include "imgui_impl_dx11.h"

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

  while (window->LoopState() != engine::WINDOW_STATE_HALT) {
    ImGui_ImplDX11_NewFrame(window->GetHandle());
    ImVec4 clear_color = ImColor(114, 144, 154);
    ui_renderer->SetRenderTargets({rt}, nullptr);
    ui_renderer->ClearRenderTarget(rt, (float *)&clear_color);
    ImGui::Begin("Debug");
    ImGui::Text("Hello");
    ImGui::End();
    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData(), env, ui_renderer);
    swapchain->Present();
  }

  ImGui::DestroyContext();
  engine::Env::CleanupEnvironment(env);
}
