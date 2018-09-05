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
  // show image names while mouse cursor on it
  ImGui::TextColored({0, 1.f, 0, 1.f}, args.image_name.c_str());
  // cursor coordinate of image.
  ImGui::Text("<Coord: X [%d]  Y [%d]>", int(args.cursor_coord.x),
              int(args.cursor_coord.y));
  ImGui::SameLine();
  // pixel color, alpha ignored
  ImGui::Text("<Color: R[%d] G[%d] B[%d]>", (int)args.cursor_color.x,
              (int)args.cursor_color.y, (int)args.cursor_color.z);
  ImGui::SameLine();
  ImGui::Text("<Scale: %.1f%%>", args.scale * 100.f);
  ImGui::End();
  return ret_info;
}