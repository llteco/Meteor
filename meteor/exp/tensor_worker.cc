#include "meteor/exp/tensor_worker.h"

TensorWorker::TensorWorker(const TensorWorkerParam &par) {
  buffer_.resize(kMaxBufferSize);
}

TensorWorker::~TensorWorker() {}

TensorImage TensorWorker::Execute(const TensorImage &image_in) {
  TensorImage image_out;
  image_out.imagebytes = buffer_.data();
  return image_out;
}