#ifndef METEOR_CORE_DECODER_H_
#define METEOR_CORE_DECODER_H_
#include <memory>
#include <string>
#include <vector>


typedef int (*mpOpen)(const char *file);
typedef int (*mpClose)();
typedef int (*mpRun)(const char *bytes, char *image);
typedef int (*mpInfo)(int *w, int *h, void *info);

class Decoder {
 public:
  Decoder();
  ~Decoder();

  int Load(std::string file,
           std::vector<std::string> fname = {"Open", "Close", "Run", "Info"});
  std::vector<char> Decode(const std::vector<char> &bytes);
  int Width() const;
  int Height() const;

 private:
  mpOpen fp_open_;
  mpClose fp_close_;
  mpRun fp_run_;
  mpInfo fp_info_;
  struct ImageInfo {
    int w;
    int h;
  } info_;
  void *dlib_;
};
#endif  // METEOR_CORE_DECODER_H_
