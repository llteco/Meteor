#include <algorithm>
#include <map>
#include <memory>
#include "meteor/core/pic_loader.h"
#include "meteor/core/raw_loader.h"
#include "meteor/core/tex_pool.h"
#include "meteor/ui/ui_window.h"

using namespace ixr::engine;

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
  if (max < min) max = min;
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
  static std::unique_ptr<Loader> g_loader;
  static std::unique_ptr<TexPool> g_texpool;
  static std::vector<char> g_buffer;
  bool use_dec = info.format_id == 0;

  if (info.toggle_open | info.toggle_format_change | info.toggle_refresh) {
    try {
      Decoder d;
      if (use_dec) {
        d.Load(info.path);
        g_loader.reset(new PicLoader(info.path, d));
        args->image_size[0] = d.Width();
        args->image_size[1] = d.Height();
      } else {
        g_loader.reset(new RawLoader(info.path, ColorBytes(info.format_id),
                                     info.image_size[0], info.image_size[1]));
        args->image_size[0] = info.image_size[0];
        args->image_size[1] = info.image_size[1];
      }
      g_texpool.reset(
          new TexPool(e, r, args->image_size[0], args->image_size[1], 0));
      g_buffer = g_loader->ReadF();
      g_texpool->Update(ToRGBA(g_buffer, info.format_id, info.image_size[0],
                               info.image_size[1]));
      args->tex_id = g_texpool->GetTexID();
      args->max_frame = g_loader->Length();
    } catch (...) {
      printf("warning: exception at %s:%d\n", __FUNCTION__, __LINE__);
    }
  }
  if (info.toggle_jump | info.toggle_next | info.toggle_prev |
      info.toggle_run) {
    if (g_loader && !use_dec) {
      g_loader->Seekg(info.frame_num);
      g_buffer = g_loader->ReadF();
      g_texpool->Update(ToRGBA(g_buffer, info.format_id, info.image_size[0],
                               info.image_size[1]));
    }
  }
  // update cursor pixel color
  if (!g_buffer.empty() && g_texpool) {
    int u = static_cast<int>(info.image_cursor.x);
    int v = static_cast<int>(info.image_cursor.y);
    int offset = info.image_size[0] * v * 4 + u * 4;
    args->cursor_color.x = static_cast<float>((uint8_t)g_buffer[offset]);
    args->cursor_color.y = static_cast<float>((uint8_t)g_buffer[offset + 1]);
    args->cursor_color.z = static_cast<float>((uint8_t)g_buffer[offset + 2]);
    args->cursor_color.w = static_cast<float>((uint8_t)g_buffer[offset + 3]);
  }
}

void ImageCompareBehave(ixr::engine::Env *e, ixr::engine::core::Renderer *r,
                        const ImageCompareInfo &info, ImageCompareArgs *args) {
  using TexLoader =
      std::pair<std::unique_ptr<Loader>, std::unique_ptr<TexPool>>;
  static std::vector<TexLoader> g_pools;
  static std::vector<std::vector<char>> g_buffers;
  bool use_dec = info.format_id == 0;
  if (info.toggle_reset | info.toggle_remove) {
    try {
      g_pools.clear();
      g_buffers.clear();
      args->tex_ids.clear();
      if (use_dec) {
        Decoder d = Decoder();
        for (auto &pth : info.paths) {
          d.Load(pth);
          auto loader = std::make_unique<PicLoader>(pth, d);
          args->image_size[0] = d.Width();
          args->image_size[1] = d.Height();
          auto tex = std::make_unique<TexPool>(e, r, d.Width(), d.Height(), 0);
          g_buffers.push_back(loader->ReadF());
          tex->Update(g_buffers.back().data());
          args->tex_ids.push_back(tex->GetTexID());
          g_pools.push_back(std::make_pair(std::move(loader), std::move(tex)));
        }
        args->max_frame = 1;
      } else {
        for (auto &pth : info.paths) {
          auto loader = std::make_unique<RawLoader>(
              pth, ColorBytes(info.format_id), info.image_size[0],
              info.image_size[1]);
          args->image_size[0] = info.image_size[0];
          args->image_size[1] = info.image_size[1];
          auto tex = std::make_unique<TexPool>(e, r, info.image_size[0],
                                               info.image_size[1], 0);
          g_buffers.push_back(loader->ReadF());
          tex->Update(ToRGBA(g_buffers.back(), info.format_id,
                             info.image_size[0], info.image_size[1]));
          args->tex_ids.push_back(tex->GetTexID());
          args->max_frame = std::max(args->max_frame, loader->Length());
          g_pools.push_back(std::make_pair(std::move(loader), std::move(tex)));
        }
      }
    } catch (...) {
      printf("warning: exception at %s:%d\n", __FUNCTION__, __LINE__);
    }
  }
  if (info.toggle_jump | info.toggle_next | info.toggle_prev |
      info.toggle_run) {
    if (!g_pools.empty() && !use_dec) {
      for (auto &tl : g_pools) {
        tl.first->Seekg(info.frame_num);
        auto data = tl.first->ReadF();
        tl.second->Update(ToRGBA(data, info.format_id, info.image_size[0],
                                 info.image_size[1]));
      }
    }
  }
  // update cursor pixel color
  if (info.image_index >= g_buffers.size()) return;
  auto &buffer = g_buffers[info.image_index];
  if (!buffer.empty()) {
    int u = static_cast<int>(info.image_cursor.x);
    int v = static_cast<int>(info.image_cursor.y);
    u = clip(u, 0, info.image_size[0] - 1);
    v = clip(v, 0, info.image_size[1] - 1);
    int offset = info.image_size[0] * v * 4 + u * 4;
    args->cursor_color.x = static_cast<float>((uint8_t)buffer[offset]);
    args->cursor_color.y = static_cast<float>((uint8_t)buffer[offset + 1]);
    args->cursor_color.z = static_cast<float>((uint8_t)buffer[offset + 2]);
    args->cursor_color.w = static_cast<float>((uint8_t)buffer[offset + 3]);
  }
}
