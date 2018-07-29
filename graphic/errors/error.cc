/********************************************************************
Copyright 2018 Intel Corp. All Rights Reserved.
Description : claim error class
Author      : Wenyi Tang
Email       : wenyi.tang@intel.com
Created     : Feb. 28th, 2018
********************************************************************/
#include "errors/error.h"
#include <functional>

namespace ixr {
namespace engine {
namespace errors {
std::function<void(const char *)> MsgLogger = [](const char *) {};
}
}  // namespace engine
}  // namespace ixr
