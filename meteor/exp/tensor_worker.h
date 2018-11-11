
#ifndef METEOR_EXP_TENSOR_WORKER_H_
#define METEOR_EXP_TENSOR_WORKER_H_
struct TensorImage {
  void *imagebytes;
  int width;
  int height;
  int channel;
};

struct TensorWorkerParam {

};

class TensorWorker {
 public:
  TensorWorker(const TensorWorkerParam &par);
  ~TensorWorker();

  TensorImage Execute(const TensorImage &image_in);
};
#endif  // METEOR_EXP_TENSOR_WORKER_H_
