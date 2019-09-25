#include <algorithm>
#include <fstream>
#include <map>
#include <memory>
#include "meteor/core/cam_loader.h"
#include "meteor/core/pic_loader.h"
#include "meteor/core/raw_loader.h"
#include "meteor/core/tex_pool.h"
#include "meteor/lodepng/lodepng.h"
#include "meteor/ui/ui_window.h"

using namespace ll::engine;

inline float ColorBytes(int cmode) {
  std::string fmt = kSupportedFormats[cmode];
  switch (cmode) {
    case RGBA:
    case ARGB:
      return 4.f;
    case YUV:
      return 3.f;
    case NV12:
    case NV21:
    case YV12:
    case YV21:
      return 1.5f;
    case Y:
      return 1.f;
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

inline std::vector<uint8_t> crop(const std::vector<char> &b, int pitch, int u0,
                                 int v0, int u1, int v1) {
  if (u1 <= u0 || v1 <= v0) {
    printf("[!] wrong crop box!\n");
    return {};
  }
  int crop_pitch = 4 * (u1 - u0);
  std::vector<uint8_t> new_crop((v1 - v0) * crop_pitch);
  for (int i = 0; i < v1 - v0; i++) {
    const char *src = b.data() + (v0 + i) * pitch + u0 * 4;
    memcpy(new_crop.data() + i * crop_pitch, src, crop_pitch);
  }
  return new_crop;
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
    case NV21:
      u_ptr = data.cbegin() + w * h + 1;
      v_ptr = data.cbegin() + w * h;
      if (cmode == NV12) {
        std::swap(u_ptr, v_ptr);
      }
      sub_h = sub_v = 2;
      break;
    case YV12:
    case YV21:
      v_ptr = data.cbegin() + w * h;
      u_ptr = data.cbegin() + w * h * 5 / 4;
      if (cmode == YV12) {
        std::swap(v_ptr, u_ptr);
      }
      sub_h = sub_v = 2;
      planar = true;
      break;
    case Y:
      v_ptr = u_ptr = data.cend();
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
        buf.push_back(128);
      }
      if (v_ptr != data.cend()) {
        buf.push_back(*v_ptr);
      } else {
        buf.push_back(128);
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

inline char *NormalizeRGBA(std::vector<char> &data, int cmode, int w, int h) {
  std::vector<char> buf;
  auto head_ptr = data.cbegin();
  switch (cmode) {
    case ARGB:
      for (int row = 0; row < h; row++) {
        for (int col = 0; col < w; col++) {
          buf.push_back(*(head_ptr + 1));
          buf.push_back(*(head_ptr + 2));
          buf.push_back(*(head_ptr + 3));
          buf.push_back(*(head_ptr + 0));
          head_ptr += 4;
        }
      }
      break;
    case RGBA:
    default:
      return data.data();
  }
}

inline char *ToRGBA(std::vector<char> &data, int cmode, int w, int h) {
  std::vector<char> buf;
  switch (cmode) {
    case YUV:
      PackedToYUV(data, cmode, w, h);
      break;
    case Y:
    case NV12:
    case NV21:
    case YV12:
    case YV21:
      PlanerToYUV(data, cmode, w, h);
      break;
    case ARGB:
    case RGBA:
    default:
      return NormalizeRGBA(data, cmode, w, h);
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

void ImageCompareBehave(ll::engine::Env *e, ll::engine::core::Renderer *r,
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
  } else if (info.toggle_save) {
    auto iter = info.saved_paths.begin();
    for (auto &&b : g_buffers) {
      if (b.empty()) continue;
      int u0 = static_cast<int>(info.real_box.x);
      int v0 = static_cast<int>(info.real_box.y);
      u0 = clip(u0, 0, info.image_size[0] - 1);
      v0 = clip(v0, 0, info.image_size[1] - 1);
      int u1 = static_cast<int>(info.real_box.z);
      int v1 = static_cast<int>(info.real_box.w);
      u1 = clip(u1, 0, info.image_size[0] - 1);
      v1 = clip(v1, 0, info.image_size[1] - 1);
      auto cropped_image = crop(b, info.image_size[0] * 4, u0, v0, u1, v1);
      std::vector<uint8_t> png;
      if (lodepng::encode(png, cropped_image, u1 - u0, v1 - v0) == 0) {
        printf("[!] saved to %s\n", iter->c_str());
        std::ofstream fp(iter->c_str(), std::ios::binary);
        fp.write(reinterpret_cast<const char *>(png.data()), png.size());
        ++iter;
      }
    }
    if (iter != info.saved_paths.end()) {
      printf("[*] warning: error occurred while saving files.\n");
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
    if (info.image_index > 0) {
      // PSNR
      auto &label = g_buffers[0];
      int64_t mse = 0;
      for (int i = 0; i < buffer.size(); i++) {
        int64_t diff = (uint8_t)buffer[i] - (uint8_t)label[i];
        mse += diff * diff;
      }
      // 255^2 / 4 * 3 = 48768.75
      double psnr = 10.0 * log10(48768.75 * buffer.size()) - 10.0 * log10(mse);
      args->global_psnr = psnr;
    }
  }
}

void ExpActualBehave(ll::engine::Env *e, ll::engine::core::Renderer *r,
                     const ExpInfo &info, ExpArgs *args) {
  static std::unique_ptr<Loader> g_loader;
  static std::unique_ptr<TexPool> g_texpool;
  static std::vector<char> g_buffer;
  // static std::unique_ptr<TensorWorker> g_worker;
  static std::unique_ptr<TexPool> g_subtex;
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
#if 0
  if (info.toggle_connect && !args->connected) {
    // connect to server
    TensorWorkerParam par;
    par.url = info.url;
    try {
      g_worker = std::make_unique<TensorWorker>(par);
      args->connected = true;
    } catch (const std::exception &ex) {
      printf("error: can't connect to server. %s\n", ex.what());
      args->connected = false;
    }
  }
  if (info.toggle_compare && g_worker) {
    int w = static_cast<int>(info.select_region.x);
    int h = static_cast<int>(info.select_region.y);
    g_subtex.reset(new TexPool(e, r, w * 4, h * 4, 0));
    TensorImage image_in{new char[w * h * 4], w, h, 3};
    for (int i = 0; i < h; i++) {
      int offset =
          static_cast<int>(info.select_offset.y * info.image_size[0] +
                           info.select_offset.x + info.image_size[0] * i);
      memcpy(image_in.imagebytes + w * i, g_buffer.data() + offset, w * 4);
    }
    TensorImage image_out = g_worker->Execute(image_in);
    delete[] image_in.imagebytes;
    g_subtex->Update(image_out.imagebytes);
    args->sub_tex_id[0] = g_subtex->GetTexID();
  }
#endif
}

void PlayerActualBehave(ll::engine::Env *e, ll::engine::core::Renderer *r,
                        const PlayerInfo &info, PlayerArgs *args) {
  static ixr::app::helper::MfHelper mfh;
  static ixr::app::helper::MfHelper::TypeLists cam_types;
  static std::unique_ptr<TexPool> g_texpool;
  static std::vector<char> g_buffer;
  static int format_id;
  if (args->cams.empty()) {
    args->cams = mfh.EnumVideoCaptureDevices();
  }
  if (info.toggle_reload) {
    mfh.EnumVideoCaptureDevices(args->cams[info.cam_id]);
    cam_types = mfh.EnumCaptureTypes();
    args->cam_types.clear();
    for (auto &t : cam_types) {
      if (t == MFVideoFormat_NV12) {
        args->cam_types.push_back("NV12");
      } else if (t == MFVideoFormat_YV12) {
        args->cam_types.push_back("YV12");
      } else {
        args->cam_types.push_back("Unsupported");
      }
    }
    auto itr = cam_types.begin();
    for (int i = 0; i < info.type_id; i++) ++itr;
    mfh.EnumCaptureTypes(*itr);
    if (*itr == MFVideoFormat_NV12) {
      format_id = 5;
    } else if (*itr == MFVideoFormat_YV12) {
      format_id = 7;
    }
    mfh.QueryFrameSize(&args->image_size[0], &args->image_size[1]);
    args->playing = 0;
  }
  if (info.toggle_play) {
    try {
      args->playing ^= 1;
      if (!args->playing) {
        mfh.EnumVideoCaptureDevices(args->cams[info.cam_id]);
      } else {
        auto itr = cam_types.begin();
        for (int i = 0; i < info.type_id; i++) ++itr;
        mfh.EnumCaptureTypes(*itr);
        if (*itr == MFVideoFormat_NV12) {
          format_id = 5;
        } else if (*itr == MFVideoFormat_YV12) {
          format_id = 7;
        }
        mfh.QueryFrameSize(&args->image_size[0], &args->image_size[1]);
        g_texpool.reset(
            new TexPool(e, r, args->image_size[0], args->image_size[1], 0));
        args->tex_id = g_texpool->GetTexID();
      }
    } catch (...) {
      printf("warning: exception at %s:%d\n", __FUNCTION__, __LINE__);
    }
  }
  if (args->playing) {
    BYTE *raw_ptr;
    UINT length;
    HRESULT hr = mfh.LockFrame(&raw_ptr, &length);
    if (SUCCEEDED(hr)) {
      if (g_buffer.size() < length) {
        g_buffer.resize(length);
      }
      memcpy_s(g_buffer.data(), g_buffer.size(), raw_ptr, length);
      mfh.UnlockFrame();
      g_texpool->Update(
          ToRGBA(g_buffer, format_id, info.image_size[0], info.image_size[1]));
    }
  }
}
