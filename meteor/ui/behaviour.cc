#include <memory>
#include "meteor/core/pic_loader.h"
#include "meteor/core/raw_loader.h"
#include "meteor/core/tex_pool.h"
#include "meteor/ui/ui_window.h"

using namespace ixr::engine;

std::unique_ptr<Loader> g_loader;
std::unique_ptr<TexPool> g_texpool;

inline float ColorBytes(int cmode) {
  std::string fmt = kSupportedFormats[cmode];
  switch (cmode) {
    case 1:
      return 4;
    case 2:
      return 3;
    case 3:
    case 4:
      return 1.5f;
  }
  return 0.0f;
}

inline char *ToRGBA(std::vector<char> &data, int cmode, int w, int h) {
  return data.data();
}

inline char *ToYUV(std::vector<char> &data, int cmode, int w, int h) {
  std::vector<char> buf;
  int sub_v = 1, sub_h = 1;
  auto y_ptr = data.cbegin();
  auto u_ptr = data.cbegin() + w * h;
  auto v_ptr = data.cbegin() + w * h + 1;
  bool planar = false;
  switch (cmode) {
  case 1:
  case 2:
    return data.data();
  case 3:
    std::swap(u_ptr, v_ptr);
    sub_h = sub_v = 2;
    break;
  case 4:
    v_ptr = data.cbegin() + w * h;
    u_ptr = data.cbegin() + w * h * 5 / 4;
    sub_h = sub_v = 2;
    planar = true;
  default:
    break;
  }
  for (int row = 0; row < h; row++) {
    for (int col = 0; col < w; col++) {
      buf.push_back(*y_ptr);
      buf.push_back(*u_ptr);
      buf.push_back(*v_ptr);
      buf.push_back(0xFF);  // alpha
      ++y_ptr;
      if (col % sub_h == sub_h - 1) {
        ++u_ptr;
        ++v_ptr;
        if (!planar) {
          ++u_ptr;
          ++v_ptr;
        }
      }
    }
    if (row % sub_v != sub_v - 1) {
      u_ptr -= w / sub_h;
      v_ptr -= w / sub_h;
      if (!planar) {
        u_ptr -= w / sub_h;
        v_ptr -= w / sub_h;
      }
    }
  }
  data.swap(buf);
  return data.data();
}

void ImageViewerBehave(Env *e, core::Renderer *r, const ImageViewerInfo &info,
                       ImageViewerArgs *args) {
  bool use_dec = info.format_id == 0;

  if (info.toggle_open) {
    try {
      if (use_dec) {
        //g_loader.reset(new PicLoader(info.path, d));
      } else {
        g_loader.reset(new RawLoader(info.path, ColorBytes(info.format_id),
                                     info.image_size[0], info.image_size[1]));
      }
      g_texpool.reset(
        new TexPool(e, r, info.image_size[0], info.image_size[1], 0));
      auto data = g_loader->ReadF();
      g_texpool->Update(ToYUV(data, info.format_id, info.image_size[0], info.image_size[1]));
      args->tex_id = g_texpool->GetTexID();
    } catch (...) {

    }
  }
  if (info.toggle_refresh) {
    try {
      g_texpool.reset(
        new TexPool(e, r, info.image_size[0], info.image_size[1], 0));
      if (g_loader) {
        g_loader->Seekg(0);
        auto data = g_loader->ReadF();
        g_texpool->Update(ToYUV(data, info.format_id, info.image_size[0], info.image_size[1]));
        args->tex_id = g_texpool->GetTexID();
      }
    } catch (...) {

    }
  }
  if (info.toggle_jump | info.toggle_next | info.toggle_prev) {
    g_loader->Seekg(info.frame_num);
    auto data = g_loader->ReadF();
    g_texpool->Update(ToYUV(data, info.format_id, info.image_size[0], info.image_size[1]));
    args->tex_id = g_texpool->GetTexID();
  }
  if (g_loader) {
    args->max_frame = g_loader->Length();
  }
}