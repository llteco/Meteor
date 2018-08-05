#include "meteor/core/pic_loader.h"

PicLoader::PicLoader(std::string path, const Decoder &d) : Loader(path) {}

PicLoader::~PicLoader() {}

int PicLoader::Tellg() { return fp_.tellg() ? 1 : 0; }

void PicLoader::Seekg(int n_frames) {
  if (n_frames > 0) return;
  fp_.seekg(0);
}

void PicLoader::Change(float, int, int) {}

std::vector<int> PicLoader::Size() { return {}; }

int PicLoader::Length() { return 1; }

std::vector<char> PicLoader::ReadF() {
  return {};
}