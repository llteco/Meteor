#include <string>
#include "imgui/imgui.h"

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
  char path[512];
  int format_id;
  int image_size[2];
};

struct ImageViewerArgs {
  bool enable;
  uint32_t window_flag;
  ImVec2 parent_pos;
  ImVec2 parent_size;
};

ImageViewerInfo ImageViewer(const ImageViewerArgs &args);

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

