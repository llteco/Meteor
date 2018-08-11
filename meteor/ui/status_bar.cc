#include <algorithm>
#include "meteor/ui/ui_window.h"


StatusBarInfo StatusBar(const StatusBarArgs &args) {
  static StatusBarInfo ret_info{};
  ImGui::Begin(__FILE__, nullptr,
               static_cast<ImGuiWindowFlags>(args.window_flag));
  ret_info.pos = args.parent_pos;
  ret_info.pos.y = ImGui::GetIO().DisplaySize.y - kStatusBarHeight - 1;
  ret_info.size.x = args.parent_size.x;
  ret_info.size.y = kStatusBarHeight;
  ImGui::SetWindowPos(ret_info.pos, ImGuiCond_Always);
  ImGui::SetWindowSize(ret_info.size, ImGuiCond_Always);

  ImGui::End();
  return ret_info;
}