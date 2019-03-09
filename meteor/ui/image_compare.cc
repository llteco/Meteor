#include <algorithm>
#include "meteor/ui/ui_window.h"

extern std::vector<std::string> OnButtonOpenFile();

extern std::string OnButtonSaveFile(std::string);

void ArrangeImages(const ImageCompareArgs &args, ImageCompareInfo *info);

ImageCompareInfo ImageCompare(const ImageCompareArgs &args) {
  static ImageCompareInfo ret_info{};
  ImGui::Begin(__FILE__, nullptr,
               static_cast<ImGuiWindowFlags>(args.window_flag));
  ret_info.pos = args.parent_pos;
  ret_info.pos.y += args.parent_size.y + 1;
  ret_info.size.x = args.parent_size.x;
  ret_info.size.y =
      ImGui::GetIO().DisplaySize.y - ret_info.pos.y - kStatusBarHeight - 2;
  ImGui::SetWindowPos(ret_info.pos, ImGuiCond_Always);
  ImGui::SetWindowSize(ret_info.size, ImGuiCond_Always);
  if (ImGui::Button("Add File...")) {
    auto new_paths = OnButtonOpenFile();
    ret_info.paths.insert(ret_info.paths.end(), new_paths.begin(),
                          new_paths.end());
    ret_info.path_id = ret_info.paths.size() - 1;
  }
  ImGui::SameLine();
  if (!ret_info.paths.empty()) {
    if (ImGui::BeginCombo("Files", ret_info.paths[ret_info.path_id].c_str())) {
      for (int n = 0; n < ret_info.paths.size(); n++) {
        bool is_selected = false;
        if (ImGui::Selectable(ret_info.paths[n].c_str(), is_selected)) {
          ret_info.path_id = n;
        }
        if (is_selected) ImGui::SetItemDefaultFocus();
      }
      ImGui::EndCombo();
    }
  }
  ImGui::SameLine();
  ret_info.toggle_remove = ImGui::Button("Remove");
  if (ret_info.toggle_remove && !ret_info.paths.empty()) {
    ret_info.paths.erase(ret_info.paths.begin() + ret_info.path_id);
    if (ret_info.path_id >= ret_info.paths.size()) {
      ret_info.path_id -= 1;
    }
  }
  ImGui::SameLine();
  ret_info.toggle_reset = ImGui::Button("Compare");
  if (ret_info.toggle_reset) {
    ret_info.scale = 1.0f;
    ret_info.image_pos_uv[0] = 0.f;
    ret_info.image_pos_uv[1] = 0.f;
    ret_info.image_pos_uv[2] = 1.f;
    ret_info.image_pos_uv[3] = 1.f;
    ret_info.bound_box = {};
  }
  ImGui::SameLine();
  ret_info.toggle_save = ImGui::Button("Save");
  if (ret_info.toggle_save) {
    ret_info.saved_paths.clear();
    std::string prefix = OnButtonSaveFile("crop");
    if (prefix != "iloveliuxing") {
      std::string savefile;
      auto pos = prefix.find_last_of('.');
      if (pos < prefix.size()) {
        prefix = std::string(prefix.begin(), prefix.begin() + pos);
      }
      for (auto &name : ret_info.paths) {
        auto pos1 = name.find_last_of('\\');
        auto pos2 = name.find_last_of('.');
        if (pos1 < name.size() && pos2 < name.size()) {
          savefile = prefix + "_" + std::string(name.begin() + pos1 + 1,
                                                name.begin() + pos2);
          savefile += ".png";
        }
        ret_info.saved_paths.push_back(savefile);
      }
    } else {
      ret_info.toggle_save = false;
    }
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
  ImGui::BeginChild("Images", {}, true, ImGuiWindowFlags_NoScrollWithMouse);
  ArrangeImages(args, &ret_info);
  ImGui::EndChild();
  ImGui::End();
  return ret_info;
}

void ArrangeImages(const ImageCompareArgs &args, ImageCompareInfo *info) {
  const size_t total_image_num = args.tex_ids.size();
  const float kMinWidth = 200.0f + 1.0f;
  const auto &kIO = ImGui::GetIO();
  const auto kWinSize = ImGui::GetWindowSize();
  // place images horizentally
  float width = std::max(kMinWidth, kWinSize.x / total_image_num);
  int col = std::max(1, static_cast<int>(kWinSize.x / width));
  int cnt = 0;
  ImVec2 tex_sz, uv0, uv1;
  tex_sz.x = static_cast<float>(info->image_size[0]) * info->scale;
  tex_sz.y = static_cast<float>(info->image_size[1]) * info->scale;
  uv0 = {info->image_pos_uv[0], info->image_pos_uv[1]};
  uv1 = {info->image_pos_uv[2], info->image_pos_uv[3]};
  bool child_open = false;
  char N = 'A';  // use different child names
  while (cnt < total_image_num) {
    if (cnt % col == 0) {
      ImGui::BeginChild(
          &N, ImVec2(0, width), false,
          ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoScrollbar);
      child_open = true;
      N++;
      ImGui::Columns(col, nullptr, false);
    }
    ImGui::Image(args.tex_ids[cnt++], tex_sz, uv0, uv1);
    if (ImGui::IsItemHovered()) {
      // use mouse wheel to zoom image
      info->scale += 0.2f * kIO.MouseWheel;
      info->scale = std::max(0.1f, info->scale);
      static float dx, dy;
      if (kIO.MouseDown[0]) {
        // Drag to move
        static ImVec2 prev_uv;
        if (kIO.MouseClicked[0]) {
          prev_uv = uv0;
        }
        dx = kIO.MouseClickedPos[0].x - kIO.MousePos.x;
        dy = kIO.MouseClickedPos[0].y - kIO.MousePos.y;
        dx += prev_uv.x * tex_sz.x;
        dy += prev_uv.y * tex_sz.y;
      } else if (kIO.MouseDown[1]) {
        // Drag to bound
        info->bound_box.x = kIO.MouseClickedPos[1].x;
        info->bound_box.y = kIO.MouseClickedPos[1].y;
        info->bound_box.z = kIO.MousePos.x;
        info->bound_box.w = kIO.MousePos.y;
        info->bound_index = cnt - 1;
      }
      dx = std::min(std::max(0.f, dx), std::max(0.f, tex_sz.x - width));
      dy = std::min(std::max(0.f, dy), std::max(0.f, tex_sz.y - width));
      info->image_pos_uv[0] = dx / tex_sz.x;
      info->image_pos_uv[1] = dy / tex_sz.y;
      info->image_pos_uv[2] = 1 + dx / tex_sz.x;
      info->image_pos_uv[3] = 1 + dy / tex_sz.y;
      // update cursor info
      info->image_index = cnt - 1;
      if (cnt <= info->paths.size())
        info->image_name = info->paths[info->image_index];
      auto pos = ImGui::GetWindowPos();
      float off_y = pos.y;
      float off_x = pos.x;
      float off_bound_x = off_x;
      if (cnt > 1) {
        off_x += ImGui::GetColumnWidth() * ImGui::GetColumnIndex();
      }
      if (info->bound_index > 0) {
        off_bound_x += ImGui::GetColumnWidth() * info->bound_index;
      }
      info->image_cursor.x = (kIO.MousePos.x - off_x + dx) / info->scale;
      info->image_cursor.y = (kIO.MousePos.y - off_y + dy) / info->scale;
      info->real_box.x = (info->bound_box.x - off_bound_x + dx) / info->scale;
      info->real_box.y = (info->bound_box.y - off_y + dy) / info->scale;
      info->real_box.z = (info->bound_box.z - off_bound_x + dx) / info->scale;
      info->real_box.w = (info->bound_box.w - off_y + dy) / info->scale;
    }
    auto drawer = ImGui::GetWindowDrawList();
    auto xyz = info->bound_box;
    drawer->AddRect({xyz.x, xyz.y}, {xyz.z, xyz.w}, ImColor(255, 0, 0));
    ImGui::NextColumn();
    if (cnt % col == 0) {
      ImGui::EndChild();
      child_open = false;
    }
  }
  if (child_open) {
    ImGui::EndChild();
  }
}