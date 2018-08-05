#ifndef METEOR_CORE_RAW_LOADER_H_
#define METEOR_CORE_RAW_LOADER_H_
#include <string>
#include <fstream>
#include "meteor/core/loader.h"

class RawLoader : public Loader {
 public:
  RawLoader(std::string path, float cbytes, int width, int height);

  virtual ~RawLoader();

  virtual int Tellg() override;

  virtual void Seekg(int n_frames) override;

  virtual void Change(float cbytes, int width, int height) override;

  virtual std::vector<int> Size() override;

  virtual int Length() override;

  virtual std::vector<char> ReadF() override;

 private:
  int frame_size_;
  int size_[2];
};

#endif  // METEOR_CORE_RAW_LOADER_H_
