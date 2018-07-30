#include "meteor/ui/ui_window.h"

ImageViewerInfo ImageViewer(const ImageViewerArgs &args) {
  static ImageViewerInfo ret_info{};
  ret_info.toggle_open = false;
  ImGui::Begin(__FILE__, nullptr,
               static_cast<ImGuiWindowFlags>(args.window_flag));
  ret_info.pos = args.parent_pos;
  ret_info.pos.y += args.parent_size.y + 1;
  ret_info.size.x = args.parent_size.x;
  ret_info.size.y = 500;
  ImGui::SetWindowPos(ret_info.pos, ImGuiCond_Always);
  ImGui::SetWindowSize(ret_info.size, ImGuiCond_Always);
  float w1 = ret_info.size.x * 0.13f;
  float w2 = ret_info.size.x * 0.46f;
  if (ImGui::Button("Open File...", {w1, 20})) {
    ret_info.toggle_open = true;
  }
  ImGui::PushItemWidth(w2);
  ImGui::SameLine();
  ImGui::InputText("Path", ret_info.path, sizeof(ret_info.path));
  ImGui::PopItemWidth();
  const char *kSupportedFormats[] = {
    "RGBA", "YUV", "NV12", "YV12",
  };
  float w3 = ret_info.size.x * 0.113f;
  ImGui::PushItemWidth(w3);
  if (ImGui::BeginCombo("Format", kSupportedFormats[ret_info.format_id])) {
    for (int n = 0; n < 4; n++) {
      bool is_selected = false;
      if (ImGui::Selectable(kSupportedFormats[n], is_selected))
        ret_info.format_id = n;
      if (is_selected)
        ImGui::SetItemDefaultFocus();
    }
    ImGui::EndCombo();
  }
  ImGui::SameLine();
  ImGui::InputInt2("Image Size", ret_info.image_size);
  ImGui::SameLine();
  ImGui::Button("R", ImVec2{ 20,20 });
  if (ImGui::IsItemHovered()) {
    ImGui::SetTooltip("Run");
  }
  ImGui::SameLine();
  ImGui::Button("P", ImVec2{ 20,20 });
  if (ImGui::IsItemHovered()) {
    ImGui::SetTooltip("Previous frame");
  }
  ImGui::SameLine();
  ImGui::Button("N", ImVec2{ 20,20 });
  if (ImGui::IsItemHovered()) {
    ImGui::SetTooltip("Next frame");
  }
  ImGui::SameLine();
  ImGui::SliderInt("Frames", &ret_info.frame_num, 0, args.max_frame);
  if (ImGui::IsItemHovered()) {
    ImGui::SetTooltip("Goto");
  }
  ImGui::PopItemWidth();
  ImGui::BeginChild("Image", {}, true);
  if (args.tex_id) {
    ImVec2 tex_sz;
    tex_sz.x = static_cast<float>(ret_info.image_size[0]);
    tex_sz.y = static_cast<float>(ret_info.image_size[1]);
    ImGui::Image(args.tex_id, tex_sz);
    auto &io = ImGui::GetIO();
    auto pos = ImGui::GetCursorScreenPos();
    if (ImGui::IsItemHovered()) {
      ImGui::BeginTooltip();
      float region_sz = 32.0f;
      float region_x = io.MousePos.x - pos.x - region_sz * 0.5f; if (region_x < 0.0f) region_x = 0.0f; else if (region_x > tex_sz.x - region_sz) region_x = tex_sz.x - region_sz;
      float region_y = io.MousePos.y - pos.y - region_sz * 0.5f; if (region_y < 0.0f) region_y = 0.0f; else if (region_y > tex_sz.y - region_sz) region_y = tex_sz.y - region_sz;
      float zoom = 4.0f;
      ImGui::Text("Min: (%.2f, %.2f)", region_x, region_y);
      ImGui::Text("Max: (%.2f, %.2f)", region_x + region_sz, region_y + region_sz);
      ImVec2 uv0 = ImVec2((region_x) / tex_sz.x, (region_y) / tex_sz.y);
      ImVec2 uv1 = ImVec2((region_x + region_sz) / tex_sz.x, (region_y + region_sz) / tex_sz.y);
      ImGui::Image(args.tex_id, ImVec2(region_sz * zoom, region_sz * zoom), uv0, uv1, ImColor(255, 255, 255, 255), ImColor(255, 255, 255, 128));
      ImGui::EndTooltip();
    }
  }
  ImGui::EndChild();
  ImGui::End();
  return ret_info;
}