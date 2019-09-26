// colorcode.cpp
//
// Color encoding of flow vectors
// adapted from the color circle idea described at
//   http://members.shaw.ca/quadibloc/other/colint.htm
//
// Daniel Scharstein, 4/2007
// added tick marks and out-of-range coding 6/05/07

#include <stdint.h>
#include <cmath>
#include <fstream>
#include <stdexcept>
#include <string>
#include <vector>
#include "meteor/builtin/builtin.h"

using std::ifstream;
using std::string;
using std::vector;

namespace mt {
template <class T>
inline T clip(T value, T min, T max) {
  if (max < min) max = min;
  if (value < min) return min;
  if (value > max) return max;
  return value;
}

struct Pixel {
  uint8_t r, g, b, a;
};

struct fPixel {
  float r, g, b;

  fPixel(float x, float y, float z) : r(x), g(y), b(z) {}
  fPixel(int32_t x, int32_t y, int32_t z) {
    r = static_cast<float>(x);
    g = static_cast<float>(y);
    b = static_cast<float>(z);
  }

  fPixel Scale(float s) { return fPixel(r * s, g * s, b * s); }

  fPixel operator+(const fPixel &rhs) {
    float r_ = r + rhs.r;
    float g_ = g + rhs.g;
    float b_ = b + rhs.b;
    return fPixel(r_, g_, b_);
  }

  fPixel operator-(const fPixel &rhs) {
    float r_ = r - rhs.r;
    float g_ = g - rhs.g;
    float b_ = b - rhs.b;
    return fPixel(r_, g_, b_);
  }

  Pixel ToUint8() const {
    uint8_t r_ = static_cast<uint8_t>(round(r));
    uint8_t g_ = static_cast<uint8_t>(round(g));
    uint8_t b_ = static_cast<uint8_t>(round(b));
    return {r_, g_, b_, 255};
  }

  static fPixel I() { return fPixel(255, 255, 255); }
};

class ColorWheel {
 public:
  ColorWheel(bool log = true, int scaledown = 6)
      : logscale_(log), scaledown_(scaledown) {
    const int RY = 15;
    const int YG = 6;
    const int GC = 4;
    const int CB = 11;
    const int BM = 13;
    const int MR = 6;
    constexpr int ncols = RY + YG + GC + CB + BM + MR;
    wheel_.reserve(ncols);
    for (int i = 0; i < RY; i++)
      wheel_.push_back(fPixel(255.f, 255.f * i / RY, 0.f));
    for (int i = 0; i < YG; i++)
      wheel_.push_back(fPixel(255.f - 255.f * i / YG, 255.f, 0.f));
    for (int i = 0; i < GC; i++)
      wheel_.push_back(fPixel(0.f, 255.f, 255.f * i / GC));
    for (int i = 0; i < CB; i++)
      wheel_.push_back(fPixel(0.f, 255.f - 255.f * i / CB, 255.f));
    for (int i = 0; i < BM; i++)
      wheel_.push_back(fPixel(255.f * i / BM, 0.f, 255.f));
    for (int i = 0; i < MR; i++)
      wheel_.push_back(fPixel(255.f, 0.f, 255.f - 255.f * i / MR));
  }
  ~ColorWheel() {}

  Pixel Color(float fx, float fy) const {
    constexpr float M_PI = 3.141592654f;
    auto ncols = wheel_.size();
    float rad = sqrt(fx * fx + fy * fy);
    if (logscale_) {
      rad = log1pf(rad);
    }
    rad /= scaledown_;
    float a = atan2(-fy, -fx) / M_PI;
    float fk = (a + 1.0f) / 2.0f * (ncols - 1);
    int k0 = static_cast<int>(fk);
    int k1 = (k0 + 1) % ncols;
    float f = fk - k0;
    // f = 0; // uncomment to see original color wheel
    auto col0 = wheel_[k0];
    auto col1 = wheel_[k1];
    auto col = col0.Scale(1 - f) + col1.Scale(f);
    if (rad <= 1) {
      col = fPixel::I() - (fPixel::I() - col).Scale(rad);
    } else {
      col = col.Scale(0.75f);
    }
    return col.ToUint8();
  }

 private:
  vector<fPixel> wheel_;
  bool logscale_;
  float scaledown_;
};

class VizFlow {
 public:
  VizFlow();
  VizFlow(string file);
  ~VizFlow();

  vector<Pixel> Visualize();
  int32_t GetWidth() const;
  int32_t GetHeight() const;

 private:
  int32_t width_;
  int32_t height_;
  vector<float> u_;
  vector<float> v_;
};

VizFlow::VizFlow(string file) {
  constexpr float kMagic = 202021.25f;
  ifstream fd(file, std::ios::binary);
  float magic;
  fd.read((char *)&magic, sizeof(magic));
  if (magic != kMagic) {
    throw std::runtime_error("Magic number incorrect. Invalid .flo file");
  }
  int w, h;
  fd.read((char *)&w, sizeof(w));
  fd.read((char *)&h, sizeof(h));
  for (auto i = 0; i < h; i++) {
    for (auto j = 0; j < w; j++) {
      float x;
      fd.read((char *)&x, sizeof(x));
      u_.push_back(x);
      fd.read((char *)&x, sizeof(x));
      v_.push_back(x);
    }
  }
  width_ = w;
  height_ = h;
}

VizFlow::VizFlow() : width_(0), height_(0) {}

VizFlow::~VizFlow() {}

int32_t VizFlow::GetWidth() const { return width_; }
int32_t VizFlow::GetHeight() const { return height_; }

vector<Pixel> VizFlow::Visualize() {
  static ColorWheel wheel;
  vector<Pixel> viz;
  viz.reserve(width_ * height_);
  for (auto i = 0; i < height_; i++) {
    for (auto j = 0; j < width_; j++) {
      int index = i * width_ + j;
      viz.push_back(wheel.Color(u_[index], v_[index]));
    }
  }
  return move(viz);
}

static VizFlow g_flow;
namespace flo {
int Open(const char *file) {
  try {
    VizFlow flow(file);
    g_flow = flow;
  } catch (const std::runtime_error &ex) {
    return -1;
  }
  return 0;
}

int Close() { return 0; }

int Run(const char *bytes, char *image) {
  auto color = g_flow.Visualize();
  auto w = g_flow.GetWidth();
  auto h = g_flow.GetHeight();
  memcpy(image, color.data(), w * h * sizeof(Pixel));
  return 0;
}

int Info(int *w, int *h, void *info) {
  if (w) *w = g_flow.GetWidth();
  if (h) *h = g_flow.GetHeight();
  return 0;
}
}  // namespace flo
}  // namespace mt
