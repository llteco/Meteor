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
    case RGBA:
      return 4;
    case YUV:
      return 3;
    case NV12:
    case YV12:
      return 1.5f;
  }
  return 0.0f;
}

inline float clip(float x, float min, float max) {
  if (x < min) {
    return min;
  } else if (x > max) {
    return max;
  }
  return x;
}

inline char *PackedToYUV(std::vector<char> &data, int cmode, int w, int h) {
  std::vector<char> buf;
  auto y_ptr = data.cbegin();
  auto u_ptr = data.cbegin();
  auto v_ptr = data.cbegin();
  if (ColorBytes(cmode) * w * h > data.size()) {
    buf.resize(static_cast<size_t>(w * h * ColorBytes(cmode)));
    data.swap(buf);
    return data.data();
  }
  switch (cmode) {
    case YUV:
      return data.data();
    default:
      buf.resize(static_cast<size_t>(w * h * ColorBytes(cmode)));
      data.swap(buf);
      return data.data();
  }
  data.swap(buf);
  return data.data();
}

inline char *PlanerToYUV(std::vector<char> &data, int cmode, int w, int h) {
  std::vector<char> buf;
  int sub_v = 1, sub_h = 1;
  auto y_ptr = data.cbegin();
  auto u_ptr = data.cbegin();
  auto v_ptr = data.cbegin();
  bool planar = false;
  if (ColorBytes(cmode) * w * h > data.size()) {
    buf.resize(static_cast<size_t>(w * h * ColorBytes(cmode)));
    data.swap(buf);
    return data.data();
  }
  switch (cmode) {
    case NV12:
      u_ptr = data.cbegin() + w * h + 1;
      v_ptr = data.cbegin() + w * h;
      sub_h = sub_v = 2;
      break;
    case YV12:
      u_ptr = data.cbegin() + w * h;
      v_ptr = data.cbegin() + w * h * 5 / 4;
      sub_h = sub_v = 2;
      planar = true;
      break;
    default:
      buf.resize(static_cast<size_t>(w * h * ColorBytes(cmode)));
      data.swap(buf);
      return data.data();
  }
  for (int row = 0; row < h; row++) {
    for (int col = 0; col < w; col++) {
      buf.push_back(*y_ptr);
      if (u_ptr != data.cend()) {
        buf.push_back(*u_ptr);
      } else {
        buf.push_back(0);
      }
      if (v_ptr != data.cend()) {
        buf.push_back(*v_ptr);
      } else {
        buf.push_back(0);
      }
      ++y_ptr;
      if (col % sub_h == sub_h - 1) {
        if (u_ptr != data.cend()) ++u_ptr;
        if (v_ptr != data.cend()) ++v_ptr;
        if (!planar) {
          if (u_ptr != data.cend()) ++u_ptr;
          if (v_ptr != data.cend()) ++v_ptr;
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

inline char *ToRGBA(std::vector<char> &data, int cmode, int w, int h) {
  std::vector<char> buf;
  switch (cmode) {
    case YUV:
      PackedToYUV(data, cmode, w, h);
      break;
    case NV12:
    case YV12:
      PlanerToYUV(data, cmode, w, h);
      break;
    case RGBA:
    default:
      return data.data();
  }
  auto y_ptr = data.cbegin();
  auto u_ptr = data.cbegin() + 1;
  auto v_ptr = data.cbegin() + 2;
  for (int row = 0; row < h; row++) {
    for (int col = 0; col < w; col++) {
      float Y = *reinterpret_cast<uint8_t *>(y_ptr._Ptr);
      float U = *reinterpret_cast<uint8_t *>(u_ptr._Ptr);
      float V = *reinterpret_cast<uint8_t *>(v_ptr._Ptr);
      float R = clip(Y + 1.13983f * (V - 128), 0, 255);
      float G = clip(Y - 0.39465f * (U - 128) - (0.58060f * (V - 128)), 0, 255);
      float B = clip(Y + 2.03211f * (U - 128), 0, 255);
      buf.push_back(static_cast<char>(R));
      buf.push_back(static_cast<char>(G));
      buf.push_back(static_cast<char>(B));
      buf.push_back(-1);
      y_ptr += 3;
      if (u_ptr != data.cend()) ++u_ptr;
      if (u_ptr != data.cend()) ++u_ptr;
      if (u_ptr != data.cend()) ++u_ptr;
      if (v_ptr != data.cend()) ++v_ptr;
      if (v_ptr != data.cend()) ++v_ptr;
      if (v_ptr != data.cend()) ++v_ptr;
    }
  }
  data.swap(buf);
  return data.data();
}

void ImageViewerBehave(Env *e, core::Renderer *r, const ImageViewerInfo &info,
                       ImageViewerArgs *args) {
  bool use_dec = info.format_id == 0;

  if (info.toggle_open | info.toggle_format_change | info.toggle_refresh) {
    try {
      if (use_dec) {
        // g_loader.reset(new PicLoader(info.path, d));
      } else {
        g_loader.reset(new RawLoader(info.path, ColorBytes(info.format_id),
                                     info.image_size[0], info.image_size[1]));
      }
      g_texpool.reset(
          new TexPool(e, r, info.image_size[0], info.image_size[1], 0));
      auto data = g_loader->ReadF();
      g_texpool->Update(
          ToRGBA(data, info.format_id, info.image_size[0], info.image_size[1]));
      args->tex_id = g_texpool->GetTexID();
    } catch (...) {
    }
  }
  if (info.toggle_jump | info.toggle_next | info.toggle_prev | info.toggle_run) {
    if (g_loader) {
      g_loader->Seekg(info.frame_num);
      auto data = g_loader->ReadF();
      g_texpool->Update(
          ToRGBA(data, info.format_id, info.image_size[0], info.image_size[1]));
      args->tex_id = g_texpool->GetTexID();
    }
  }
  if (g_loader) {
    args->max_frame = g_loader->Length();
  }
}