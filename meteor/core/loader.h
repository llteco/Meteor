/****************************************
 * Copyright (c) 2017-2019 Wenyi Tang
 * Author: Wenyi Tang
 * E-mail: wenyitang@outlook.com
 * Description: Texture loader
 ****************************************/
#ifndef METEOR_CORE_LOADER_H_
#define METEOR_CORE_LOADER_H_
#include <fstream>
#include <string>
#include <vector>

namespace mt {
class Loader {
 public:
  Loader(std::string path);  //< Open File
  virtual ~Loader();         //< Close File
  virtual int Tellg() = 0;
  virtual void Seekg(int n_frames) = 0;
  virtual void Change(float, int, int) = 0;
  virtual std::vector<int> Size() = 0;
  virtual int Length() = 0;
  virtual std::vector<char> ReadF() = 0;

 protected:
  std::ifstream fp_;
};
}  // namespace mt
#endif  // METEOR_CORE_LOADER_H_
