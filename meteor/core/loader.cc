#include "meteor/core/loader.h"

Loader::Loader(std::string path) {
  fp_.open(path, std::ios::binary);
}

Loader::~Loader() {
  fp_.close();
}