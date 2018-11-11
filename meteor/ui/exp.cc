#include <algorithm>
#include "meteor/ui/ui_window.h"

extern std::string OnButtonOpenFile(std::string pth);

ExpInfo ExpPanel(const ExpArgs &args) {
  static ExpInfo ret_info{};
  ret_info.toggle_open = false;
  ImGui::Begin(__FILE__, nullptr,
               static_cast<ImGuiWindowFlags>(args.window_flag));
  ret_info.pos = args.parent_pos;
  ret_info.pos.y += args.parent_size.y + 1;
  ret_info.size.x = args.parent_size.x;
  ret_info.size.y =
      ImGui::GetIO().DisplaySize.y - ret_info.pos.y - kStatusBarHeight - 2;
  ImGui::SetWindowPos(ret_info.pos, ImGuiCond_Always);
  ImGui::SetWindowSize(ret_info.size, ImGuiCond_Always);
  float w1 = ret_info.size.x * 0.13f;
  float w2 = ret_info.size.x * 0.46f;
  if (ImGui::Button("Open File...", {w1, 20})) {
    ret_info.path = OnButtonOpenFile(ret_info.path);
    ret_info.toggle_open = true;
    ret_info.scale = 1.f;
    ret_info.image_pos_uv[0] = 0.f;
    ret_info.image_pos_uv[1] = 0.f;
    ret_info.image_pos_uv[2] = 1.f;
    ret_info.image_pos_uv[3] = 1.f;
  }
  ImGui::PushItemWidth(w2);
  ImGui::SameLine();
  ImGui::LabelText("Path", "%s", ret_info.path.c_str());
  ImGui::PopItemWidth();
  ImGui::SameLine();
  ret_info.toggle_reset = ImGui::Button("Reset");
  if (ret_info.toggle_reset) {
    ret_info.scale = 1.f;
    ret_info.image_pos_uv[0] = 0.f;
    ret_info.image_pos_uv[1] = 0.f;
    ret_info.image_pos_uv[2] = 1.f;
    ret_info.image_pos_uv[3] = 1.f;
  }
  float w3 = ret_info.size.x * 0.113f;
  ImGui::PushItemWidth(w3);
  ret_info.toggle_format_change = false;
  if (ImGui::BeginCombo("Format", kSupportedFormats[ret_info.format_id])) {
    for (int n = 0; n < IM_ARRAYSIZE(kSupportedFormats); n++) {
      bool is_selected = false;
      if (ImGui::Selectable(kSupportedFormats[n], is_selected)) {
        if (n != ret_info.format_id) ret_info.toggle_format_change = true;
        ret_info.format_id = n;
      }
      if (is_selected) ImGui::SetItemDefaultFocus();
    }
    ImGui::EndCombo();
  }
  ImGui::SameLine();
  if (args.image_size[0] && args.image_size[1] && ret_info.format_id == 0) {
    ret_info.image_size[0] = args.image_size[0];
    ret_info.image_size[1] = args.image_size[1];
  }
  ret_info.toggle_refresh = ImGui::InputInt2("Image Size", ret_info.image_size);
  ImGui::SameLine();
  ret_info.toggle_run ^= ImGui::Button("R", ImVec2{20, 20});
  if (ImGui::IsItemHovered()) {
    ImGui::SetTooltip("Run");
  }
  ImGui::SameLine();
  ret_info.toggle_prev = ImGui::Button("P", ImVec2{20, 20});
  if (ImGui::IsItemHovered()) {
    ImGui::SetTooltip("Previous frame");
  }
  ImGui::SameLine();
  ret_info.toggle_next = ImGui::Button("N", ImVec2{20, 20});
  if (ImGui::IsItemHovered()) {
    ImGui::SetTooltip("Next frame");
  }
  if (ret_info.toggle_prev) {
    if (ret_info.frame_num > 0) ret_info.frame_num--;
  } else if (ret_info.toggle_next) {
    if (ret_info.frame_num < args.max_frame - 1) ret_info.frame_num++;
  } else if (ret_info.toggle_run) {
    ret_info.frame_num++;
    if (ret_info.frame_num >= args.max_frame - 1) ret_info.frame_num = 0;
  }
  ImGui::SameLine();
  ret_info.toggle_jump =
      ImGui::SliderInt("Frames", &ret_info.frame_num, 0, args.max_frame - 1);
  if (ImGui::IsItemHovered()) {
    ImGui::SetTooltip("Goto");
  }
  ImGui::PopItemWidth();
  ImGui::InputText("Model URL", ret_info.url, sizeof(ret_info.url));
  ImGui::SameLine();
  ret_info.toggle_connect = ImGui::Button("Connect") && !args.connected;
  static ImVec2 select_region{};
  ImGui::BeginChild(
      "Image", {-260, 0}, true,
      ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoScrollbar);
  if (args.tex_id) {
    // valid texture input
    // texture size: ret_info.image_size
    ImVec2 tex_sz, uv0, uv1;
    tex_sz.x = static_cast<float>(ret_info.image_size[0]) * ret_info.scale;
    tex_sz.y = static_cast<float>(ret_info.image_size[1]) * ret_info.scale;
    uv0 = {ret_info.image_pos_uv[0], ret_info.image_pos_uv[1]};
    uv1 = {ret_info.image_pos_uv[2], ret_info.image_pos_uv[3]};
    auto &io = ImGui::GetIO();
    auto pos = ImGui::GetCursorScreenPos();
    ImGui::Image(args.tex_id, tex_sz, uv0, uv1);
    if (ImGui::IsItemHovered()) {
      // Draw focus rect
      static ImVec2 rect;
      if (io.KeyCtrl && io.MouseDown[0]) {
        ImDrawList *draw_list = ImGui::GetWindowDrawList();
        draw_list->PushClipRectFullScreen();
        auto top_left = io.MouseClickedPos[0];
        auto bottom_right = io.MousePos;
        draw_list->AddQuad(top_left, {bottom_right.x, top_left.y}, bottom_right,
                           {top_left.x, bottom_right.y}, 0xFF0000FF);
        draw_list->PopClipRect();
        rect.x = fabs(bottom_right.x - top_left.x);
        rect.y = fabs(bottom_right.y - top_left.y);
      }
      if (io.KeyCtrl && !io.MouseDown[0]) {
        select_region = rect;
      }
      static float dx, dy;
      if (io.MouseDown[0]) {
        // Drag to move
        static ImVec2 prev_uv;
        if (io.MouseClicked[0]) {
          prev_uv = uv0;
        }
        dx = io.MouseClickedPos[0].x - io.MousePos.x;
        dy = io.MouseClickedPos[0].y - io.MousePos.y;
        dx += prev_uv.x * tex_sz.x;
        dy += prev_uv.y * tex_sz.y;
      }
      auto boarder = ImGui::GetWindowSize();
      dx = std::min(std::max(0.f, dx), std::max(0.f, tex_sz.x - boarder.x));
      dy = std::min(std::max(0.f, dy), std::max(0.f, tex_sz.y - boarder.y));
      ret_info.image_pos_uv[0] = dx / tex_sz.x;
      ret_info.image_pos_uv[1] = dy / tex_sz.y;
      ret_info.image_pos_uv[2] = 1 + dx / tex_sz.x;
      ret_info.image_pos_uv[3] = 1 + dy / tex_sz.y;
      // update cursor coordinate
      ret_info.image_cursor.x = (io.MousePos.x - pos.x + dx) / ret_info.scale;
      ret_info.image_cursor.y = (io.MousePos.y - pos.y + dy) / ret_info.scale;
    }
  }
  ImGui::EndChild();
  ImGui::SameLine();
  ImGui::BeginChild(
      "SubImage", {0, 0}, true,
      ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoScrollbar);
  ImGui::LabelText("Debug", "Experimenal!");
  if (args.tex_id) {
    ImGui::Image(args.tex_id, select_region);
  }
  if (args.sub_tex_id[0]) {
    ImVec2 sub_tex_sz;
    sub_tex_sz.x = static_cast<float>(args.subimage_size[0]);
    sub_tex_sz.y = static_cast<float>(args.subimage_size[1]);
    ImGui::Image(args.sub_tex_id[0], sub_tex_sz);
  }
  ImGui::EndChild();
  ImGui::End();
  return ret_info;
}
