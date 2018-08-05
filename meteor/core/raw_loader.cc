#include "meteor/core/raw_loader.h"

RawLoader::RawLoader(std::string path, float cbytes, int width, int height)
    : Loader(path) {
  Change(cbytes, width, height);
}

RawLoader::~RawLoader() {}

void RawLoader::Change(float cbytes, int width, int height) {
  frame_size_ = width * height * cbytes;
  size_[0] = width;
  size_[1] = height;
  fp_.seekg(0);
}

int RawLoader::Tellg() {
  return fp_.tellg() / frame_size_;
}

void RawLoader::Seekg(int n_frames) {
  int pos = n_frames * frame_size_;
  fp_.seekg(pos, std::ios::beg);
}

std::vector<int> RawLoader::Size() {
  return {size_[0], size_[1]};
}

int RawLoader::Length() {
  int cur = fp_.tellg();
  fp_.seekg(0, std::ios::end);
  int size = fp_.tellg() / frame_size_;
  fp_.seekg(cur);
  return size;
}

std::vector<char> RawLoader::ReadF() {
  std::vector<char> buf(frame_size_);
  fp_.read(buf.data(), frame_size_);
  return std::move(buf);
}