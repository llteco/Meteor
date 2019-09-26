/****************************************
 * Copyright (c) 2019 Wenyi Tang
 * Author: Wenyi Tang
 * E-mail: wenyitang@outlook.com
 * Description:
 ****************************************/
#include "mt_config.h"
#include "imgui/imgui.h"
#include "ll_graphic/engine/engine.h"
#include "meteor/ui/imgui_meteor.h"
#include "meteor/ui/ui_window.h"
#include "meteor/ui/win32dialog.h"

std::string OnButtonOpenFile(std::string pth) {
  return mt::ui::CallOpenFileDialog(pth);
}

std::vector<std::string> OnButtonOpenFile() {
  return mt::ui::CallOpenMultipleFileDialog();
}

std::string OnButtonSaveFile(std::string auto_name) {
  return mt::ui::CallSaveFileDialog(auto_name);
}

ll::engine::window::Window *CreateUIWindow(ll::engine::Env *env) {
  ll::engine::WindowDesc wd{
      "MeteorViewer", ll::engine::WINDOW_STYLE_DEFAULT, 1000, 800, 0, 0};
  auto window = env->NewWindow(wd);
  ll::engine::WindowEvent we{};
  we.value[3] = reinterpret_cast<uint64_t>(window->GetHandle());
  we.type = 0xFFFFFFFF;
  we.callback = [](uint32_t id, uint32_t msg, uint64_t *value) {
    if (ImGui_ImplWin32_WndProcHandler((HWND)value[3], msg, value[0], value[1]))
      return;
  };
  window->RegisterEvent(we);
  return window;
}

constexpr int FLAG_FIXED = ImGuiWindowFlags_NoMove |
                           ImGuiWindowFlags_NoCollapse |
                           ImGuiWindowFlags_NoResize;

constexpr int FLAG_FIXED_BARE = FLAG_FIXED | ImGuiWindowFlags_NoTitleBar;

#ifdef MT_NO_CONSOLE
int WINAPI wWinMain(HINSTANCE hinst, HINSTANCE, LPWSTR, INT) {
#else
int main(int argc, char *argv[]) {
#endif
  auto env = ll::engine::Env::NewEnvironment(ll::engine::GRAPHIC_API_DX11);
  auto window = CreateUIWindow(env);
  // Initialize Direct3D
  auto ui_renderer = env->NewRenderer();
  // Setup swap chain
  ll::engine::SwapChainDesc sd;
  sd.format = ll::engine::PF_RGBA32_UNORM;
  sd.window_handle = window->GetHandle();
  auto swapchain = env->NewSwapChain(sd);
  auto rt = swapchain->GetBuffer(0);

  // Show the window
  window->Show();
  window->Centered();
  // Register window resize event
  ll::engine::WindowEvent we{};
  we.value[3] = reinterpret_cast<uint64_t>(window->GetHandle());
  we.type = 0xFFFFFFFF;
  we.callback = [&](uint32_t id, uint32_t msg, uint64_t *value) {
    switch (msg) {
      case WM_SIZE:
        env->ReleaseObject(swapchain);
        ll::engine::SwapChainDesc sd;
        sd.format = ll::engine::PF_RGBA32_UNORM;
        sd.window_handle = window->GetHandle();
        swapchain = env->NewSwapChain(sd);
        rt = swapchain->GetBuffer(0);
        break;
    }
  };
  window->RegisterEvent(we);

  ImGui::CreateContext();
  // Setup ImGui binding
  ID3D11Device *dev = static_cast<ID3D11Device *>(env->GetHandle().pthis);
  ImGui_ImplDX11_Init(window->GetHandle(), dev, nullptr);
  ImGui::StyleColorsDark();
  ImGui_ImplDX11_CreateDeviceObjects(env, ui_renderer);

  mt::ui::TitleBarArgs tb{};
  mt::ui::TitleBarInfo tbr;
  tb.name = "Meteor v";
  tb.name += METEOR_VERSION;
  tb.window_flag = FLAG_FIXED;

  mt::ui::ImageViewerArgs iv{};
  mt::ui::ImageViewerInfo ivr;
  iv.enable = true;
  iv.max_frame = 1000;
  iv.window_flag = FLAG_FIXED_BARE;

  mt::ui::ImageCompareArgs ic{};
  mt::ui::ImageCompareInfo icr;
  ic.enable = false;
  ic.window_flag = FLAG_FIXED_BARE;

  mt::ui::StatusBarArgs sb{};
  mt::ui::StatusBarInfo sbr;
  sb.window_flag = FLAG_FIXED_BARE;

  mt::ui::ExpArgs exp{};
  mt::ui::ExpInfo expr;
  exp.enable = false;
  exp.window_flag = FLAG_FIXED_BARE;

  mt::ui::PlayerArgs player{};
  mt::ui::PlayerInfo playerr;
  player.enable = false;
  player.window_flag = FLAG_FIXED_BARE;

  while (window->LoopState() != ll::engine::WINDOW_STATE_HALT) {
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
    exp.enable = tbr.exp;
    exp.parent_pos = tbr.pos;
    exp.parent_size = tbr.size;
    player.enable = tbr.player;
    player.parent_pos = tbr.pos;
    player.parent_size = tbr.size;
    if (iv.enable) {
      ivr = ImageViewer(iv);
      ImageViewerBehave(env, ui_renderer, ivr, &iv);
      sb.cursor_color = iv.cursor_color;
      sb.parent_pos = ivr.pos;
      sb.parent_size = ivr.size;
      sb.image_name = ivr.path;
      sb.cursor_coord = ivr.image_cursor;
      sb.scale = ivr.scale;
    }
    if (ic.enable) {
      icr = ImageCompare(ic);
      ImageCompareBehave(env, ui_renderer, icr, &ic);
      sb.cursor_color = ic.cursor_color;
      sb.parent_pos = icr.pos;
      sb.parent_size = icr.size;
      sb.image_name = icr.image_name;
      sb.cursor_coord = icr.image_cursor;
      sb.scale = icr.scale;
      sb.global_psnr = ic.global_psnr;
    }
    if (exp.enable) {
      expr = ExpPanel(exp);
      ExpActualBehave(env, ui_renderer, expr, &exp);
      sb.parent_pos = expr.pos;
      sb.parent_size = expr.size;
      sb.image_name = expr.path;
      sb.cursor_coord = expr.image_cursor;
      sb.scale = 1.f;
      sb.cursor_color = exp.cursor_color;
    }
    if (player.enable) {
      playerr = PlayerPannel(player);
      PlayerActualBehave(env, ui_renderer, playerr, &player);
      sb.parent_pos = playerr.pos;
      sb.parent_size = playerr.size;
      sb.image_name = playerr.name;
      sb.scale = playerr.scale;
      sb.cursor_coord = playerr.image_cursor;
      sb.cursor_color = playerr.cursor_color;
    }
    sbr = StatusBar(sb);
    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData(), env, ui_renderer);
    swapchain->Present();
  }

  ImGui::DestroyContext();
  ll::engine::Env::CleanupEnvironment(env);
}
