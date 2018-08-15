#include "meteor/core/decoder.h"
#include <Windows.h>


inline std::string ParseExt(std::string filename) {
  auto pos = filename.find_last_of('.');
  if (pos < filename.size()) {
    std::string ext(filename.begin() + pos + 1, filename.end());
    return std::move(ext);
  }
  return "";
}

Decoder::Decoder() {
  fp_open_ = nullptr;
  fp_close_ = nullptr;
  fp_run_ = nullptr;
  fp_info_ = nullptr;
  dlib_ = nullptr;
  info_ = ImageInfo{};
}

Decoder::~Decoder() {}

int Decoder::Load(std::string file, std::vector<std::string> fname) {
  if (dlib_) FreeLibrary((HMODULE)dlib_);
  dlib_ = LoadLibraryA(ParseExt(file).c_str());
  if (!dlib_) {
    throw std::runtime_error("Can't find library");
  }
  if (fname.size() != 4) {
    throw std::logic_error("Wrong functions quantities! Should be 4");
  }
  HMODULE dll = (HMODULE)(dlib_);
  fp_open_ = (mpOpen)GetProcAddress(dll, fname[0].c_str());
  fp_close_ = (mpClose)GetProcAddress(dll, fname[1].c_str());
  fp_run_ = (mpRun)GetProcAddress(dll, fname[2].c_str());
  fp_info_ = (mpInfo)GetProcAddress(dll, fname[3].c_str());
  if (fp_open_ && fp_close_ && fp_run_ && fp_info_) {
    if (FAILED(fp_open_(file.c_str()))) throw std::runtime_error("Failed Open");
  } else {
    throw std::runtime_error("Can't find functions");
  }
  fp_info_(&info_.w, &info_.h, &info_);
}

int Decoder::Width() const { return info_.w; }

int Decoder::Height() const { return info_.h; }

std::vector<char> Decoder::Decode(const std::vector<char> &bytes) {
  if (fp_run_) {
    int frame_size = info_.w * info_.h * 4;  // RGBA
    std::vector<char> buf(frame_size);
    fp_run_(bytes.data(), buf.data());
    return std::move(buf);
  }
  return {};
}
