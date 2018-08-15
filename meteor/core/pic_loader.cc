#include "meteor/core/pic_loader.h"

PicLoader::PicLoader(std::string path, const Decoder &d)
    : Loader(path), d_(d) {}

PicLoader::~PicLoader() {}

int PicLoader::Tellg() { return fp_.tellg() ? 1 : 0; }

void PicLoader::Seekg(int n_frames) {
  if (n_frames > 0) return;
  fp_.clear();
  fp_.seekg(0);
}

void PicLoader::Change(float, int, int) {}

std::vector<int> PicLoader::Size() { return {d_.Width(), d_.Height()}; }

int PicLoader::Length() { return 1; }

std::vector<char> PicLoader::ReadF() {
  fp_.seekg(0, std::ios::end);
  std::vector<char> buf(fp_.tellg());
  fp_.seekg(0);
  fp_.read(buf.data(), buf.size());
  return std::move(d_.Decode(buf));
}
