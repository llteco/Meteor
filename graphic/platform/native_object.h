/********************************************************************
Copyright 2018 Intel Corp. All Rights Reserved.
Description : basic object
Author      : Wenyi Tang
Email       : wenyi.tang@intel.com
Created     : Feb. 28th, 2018
********************************************************************/
#ifndef IXR_ENGINE_PLATFORM_NATIVE_OBJECT_H_
#define IXR_ENGINE_PLATFORM_NATIVE_OBJECT_H_
namespace ixr {
namespace engine {
class Object {
 public:
  void Release() { delete this; }
  virtual ~Object(){};
};
}  // namespace engine
}  // namespace ixr
#endif  // IXR_ENGINE_PLATFORM_NATIVE_OBJECT_H_
