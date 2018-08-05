#ifndef METEOR_UI_UI_WINDOW_H_
#define METEOR_UI_UI_WINDOW_H_
#include <string>
#include "graphic/engine/engine.h"
#include "imgui/imgui.h"


constexpr char *kSupportedFormats[] = {
    "Decoder", "RGBA", "YUV", "NV12", "YV12",
};

struct TitleBarInfo {
  bool image_view;
  bool image_compare;
  ImVec2 pos;
  ImVec2 size;
};

struct TitleBarArgs {
  std::string name;
  uint32_t window_flag;
};

TitleBarInfo TitleBar(const TitleBarArgs &args);

struct ImageViewerInfo {
  ImVec2 pos;
  ImVec2 size;
  bool toggle_open;
  bool toggle_run;
  bool toggle_prev;
  bool toggle_next;
  bool toggle_jump;
  bool toggle_refresh;
  std::string path;
  int format_id;
  int image_size[2];
  float image_pos_uv[4];
  int frame_num;
  float scale;
};

struct ImageViewerArgs {
  bool enable;
  uint32_t window_flag;
  ImVec2 parent_pos;
  ImVec2 parent_size;
  int max_frame;
  ImTextureID tex_id;
};

ImageViewerInfo ImageViewer(const ImageViewerArgs &args);

void ImageViewerBehave(ixr::engine::Env *e, ixr::engine::core::Renderer *r,
                       const ImageViewerInfo &info, ImageViewerArgs *args);

struct ImageCompareInfo {
  ImVec2 pos;
  ImVec2 size;
};

struct ImageCompareArgs {
  bool enable;
  uint32_t window_flag;
  ImVec2 parent_pos;
  ImVec2 parent_size;
};

ImageCompareInfo ImageCompare(const ImageCompareArgs &args);

struct StatusBarInfo {
  ImVec2 pos;
  ImVec2 size;
};

struct StatusBarArgs {
  uint32_t window_flag;
  ImVec2 parent_pos;
  ImVec2 parent_size;
};

StatusBarInfo StatusBar(const StatusBarArgs &args);
#endif  // METEOR_UI_UI_WINDOW_H_
