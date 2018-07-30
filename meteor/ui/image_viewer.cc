#include "meteor/ui/ui_window.h"

ImageViewerInfo ImageViewer(const ImageViewerArgs &args) {
  static ImageViewerInfo ret_info{};
  ImGui::Begin(__FILE__, nullptr,
               static_cast<ImGuiWindowFlags>(args.window_flag));
  ret_info.pos = args.parent_pos;
  ret_info.pos.y += args.parent_size.y + 1;
  ret_info.size.x = args.parent_size.x;
  ret_info.size.y = 500;
  ImGui::SetWindowPos(ret_info.pos, ImGuiCond_Always);
  ImGui::SetWindowSize(ret_info.size, ImGuiCond_Always);
  if (ImGui::Button("Browse File...")) {

  }
  ImGui::SameLine();
  ImGui::InputText("Path", ret_info.path, sizeof(ret_info.path));
  const char *kSupportedFormats[] = {
    "RGBA", "YUV", "NV12", "YV12",
  };
  ImGui::PushItemWidth(100);
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
  ImGui::Button("G", ImVec2{ 20,20 });
  if (ImGui::IsItemHovered()) {
    ImGui::SetTooltip("Go!");
  }
  ImGui::PopItemWidth();
  ImGui::BeginChild("Image", {}, true);
  //ImGui::Image();
  ImGui::EndChild();
  ImGui::End();
  return ret_info;
}