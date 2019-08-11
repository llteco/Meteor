#include <algorithm>
#include "meteor/ui/ui_window.h"

PlayerInfo PlayerPannel(const PlayerArgs &args) {
  static PlayerInfo info{};
  ImGui::Begin(__FILE__, nullptr,
               static_cast<ImGuiWindowFlags>(args.window_flag));
  info.pos = args.parent_pos;
  info.pos.y += args.parent_size.y + 1;
  info.size.x = args.parent_size.x;
  info.size.y =
      ImGui::GetIO().DisplaySize.y - info.pos.y - kStatusBarHeight - 2;
  ImGui::SetWindowPos(info.pos, ImGuiCond_Always);
  ImGui::SetWindowSize(info.size, ImGuiCond_Always);
  std::vector<const char *> devicenames;
  int cap_size = static_cast<int>(args.cams.size());
  for (auto &d : args.cams) {
    devicenames.push_back(d.c_str());
  }
  ImGui::Combo("Devices", &info.cam_id, devicenames.data(), cap_size);
  ImGui::SameLine();
  info.toggle_reload = ImGui::Button("Reload");
  info.image_size[0] = (int)args.image_size[0];
  info.image_size[1] = (int)args.image_size[1];
  std::vector<const char *> typenames;
  int type_size = static_cast<int>(args.cam_types.size());
  for (auto &t : args.cam_types) {
    typenames.push_back(t.c_str());
  }
  float w3 = info.size.x * 0.113f;
  ImGui::PushItemWidth(w3);
  ImGui::Combo("Video Types", &info.type_id, typenames.data(), type_size);
  ImGui::SameLine();
  ImGui::InputInt2("Size", info.image_size);
  ImGui::SameLine();
  info.toggle_play = ImGui::Button("Play");
  if (info.toggle_play) {
    info.scale = 1;
    info.image_pos_uv[0] = 0.f;
    info.image_pos_uv[1] = 0.f;
    info.image_pos_uv[2] = 1.f;
    info.image_pos_uv[3] = 1.f;
  }
  ImGui::SameLine();
  ImGui::Checkbox("Revert", &info.toggle_revert);
  ImGui::PopItemWidth();
  ImGui::SameLine();
  if (args.playing) {
    ImGui::TextColored({0, 255, 0, 255}, "Playing...");
  } else {
    ImGui::TextColored({255, 0, 0, 255}, "Stop");
  }
  ImGui::BeginChild(
      "Image", {}, true,
      ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoScrollbar);
  if (args.tex_id) {
    // valid texture input
    // texture size: ret_info.image_size
    ImVec2 tex_sz, uv0, uv1;
    tex_sz.x = static_cast<float>(info.image_size[0]) * info.scale;
    tex_sz.y = static_cast<float>(info.image_size[1]) * info.scale;
    if (info.toggle_revert) {
      uv0 = {info.image_pos_uv[2], info.image_pos_uv[1]};
      uv1 = {info.image_pos_uv[0], info.image_pos_uv[3]};
    } else {
      uv0 = {info.image_pos_uv[0], info.image_pos_uv[1]};
      uv1 = {info.image_pos_uv[2], info.image_pos_uv[3]};
    }
    auto &io = ImGui::GetIO();
    auto pos = ImGui::GetCursorScreenPos();
    ImGui::Image(args.tex_id, tex_sz, uv0, uv1);
    if (ImGui::IsItemHovered()) {
      // use mouse wheel to zoom image
      info.scale += 0.2f * io.MouseWheel;
      info.scale = std::max(0.1f, info.scale);
      static float dx, dy;
      if (io.MouseDown[0]) {
        // Drag to move
        static ImVec2 prev_uv;
        if (io.MouseClicked[0]) {
          prev_uv = {info.image_pos_uv[0], info.image_pos_uv[1]};
        }
        dx = io.MouseClickedPos[0].x - io.MousePos.x;
        dy = io.MouseClickedPos[0].y - io.MousePos.y;
        if (info.toggle_revert) dx *= -1.f;
        dx += prev_uv.x * tex_sz.x;
        dy += prev_uv.y * tex_sz.y;
      }
      auto boarder = ImGui::GetWindowSize();
      if (info.toggle_revert) {
        dx = std::max(std::min(0.f, dx), std::min(0.f, boarder.x - tex_sz.x));
      } else {
        dx = std::min(std::max(0.f, dx), std::max(0.f, tex_sz.x - boarder.x));
      }
      dy = std::min(std::max(0.f, dy), std::max(0.f, tex_sz.y - boarder.y));
      info.image_pos_uv[1] = dy / tex_sz.y;
      info.image_pos_uv[3] = 1 + dy / tex_sz.y;
      info.image_pos_uv[0] = dx / tex_sz.x;
      info.image_pos_uv[2] = 1 + dx / tex_sz.x;
      // update cursor coordinate
      info.image_cursor.x = (io.MousePos.x - pos.x + dx) / info.scale;
      info.image_cursor.y = (io.MousePos.y - pos.y + dy) / info.scale;
    }
  }
  ImGui::EndChild();
  ImGui::End();
  return info;
}
