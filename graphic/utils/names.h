/********************************************************************
Copyright 2018 Intel Corp. All Rights Reserved.
Description : generate names
Author      : Wenyi Tang
Email       : wenyi.tang@intel.com
Created     : Feb. 28th, 2018
********************************************************************/
#ifndef IXR_ENGINE_UTILS_NAMES_H_
#define IXR_ENGINE_UTILS_NAMES_H_
#include <sstream>
#include "engine/types.h"

namespace ixr {
namespace engine {
namespace utils {
/* A class generates name string */
class Names {
 public:
  Names() = default;
  ~Names() = default;

  Names(String name, uint32_t postfix = -1) : name_(name), postfix_(postfix) {}

  String str() {
    std::stringstream ss;
    ss << name_;
    if (postfix_ != -1) {
      ss << postfix_;
    }
    return ss.str();
  }

  void AdvancePostfix(uint32_t step = 1) {
    if (postfix_ == -1) return;
    postfix_ += step;
  }

 private:
  String name_;
  uint32_t postfix_;
};
}  // namespace utils
}  // namespace engine
}  // namespace ixr
#endif  // IXR_ENGINE_UTILS_NAMES_H_
