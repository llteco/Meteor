/****************************************
 * Copyright (c) 2019 Wenyi Tang
 * Author: Wenyi Tang
 * E-mail: wenyitang@outlook.com
 * Description:
 ****************************************/
#ifndef METEOR_CORE_PIC_LOADER_H_
#define METEOR_CORE_PIC_LOADER_H_
#include "meteor/core/decoder.h"
#include "meteor/core/loader.h"

namespace mt {
class PicLoader : public Loader {
 public:
  PicLoader(std::string path, const Decoder &d);

  virtual ~PicLoader();

  virtual int Tellg() override;

  virtual void Seekg(int n_frames) override;

  virtual void Change(float cmode, int width, int height) override;

  virtual std::vector<int> Size() override;

  virtual int Length() override;

  virtual std::vector<char> ReadF() override;

 private:
  Decoder d_;
};
}  // namespace mt
#endif  // METEOR_CORE_PIC_LOADER_H_
