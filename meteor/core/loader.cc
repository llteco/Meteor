/****************************************
 * Copyright (c) 2017-2019 Wenyi Tang
 * Author: Wenyi Tang
 * E-mail: wenyitang@outlook.com
 * Description: Tex loader
 ****************************************/
#include "meteor/core/loader.h"
namespace mt {
Loader::Loader(std::string path) { fp_.open(path, std::ios::binary); }

Loader::~Loader() { fp_.close(); }
}  // namespace mt