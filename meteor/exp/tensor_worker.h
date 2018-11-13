#ifndef METEOR_EXP_TENSOR_WORKER_H_
#define METEOR_EXP_TENSOR_WORKER_H_
#include <string>
#include <vector>

struct TensorImage {
  char *imagebytes;
  int width;
  int height;
  int channel;
};

struct TensorWorkerParam {
  std::string url;
};

class TensorWorker {
 public:
  TensorWorker(const TensorWorkerParam &par);
  ~TensorWorker();

  TensorImage Execute(const TensorImage &image_in);

 private:
  const int kMaxBufferSize = 256 * 256 * 4;
  std::vector<char> buffer_;
};
#endif  // METEOR_EXP_TENSOR_WORKER_H_
