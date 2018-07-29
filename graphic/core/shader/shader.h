/********************************************************************
Copyright 2018 Intel Corp. All Rights Reserved.
Description : abstract shader class
Author      : Wenyi Tang
Email       : wenyi.tang@intel.com
Created     : Feb. 28th, 2018
********************************************************************/
#ifndef IXR_ENGINE_CORE_SHADER_SHADER_H_
#define IXR_ENGINE_CORE_SHADER_SHADER_H_
#include "engine/types.h"
#include "platform/native_object.h"

namespace ixr {
namespace engine {
/* Enumerate shader types */
enum ShaderCompileTarget {
  SHADER_TARGET_NONE,
  SHADER_TARGET_VERTEX,
  SHADER_TARGET_COMPUTE,
  SHADER_TARGET_GEOMETRY,
  SHADER_TARGET_PIXEL,
};

namespace core {
/** An abstract class for shader */
class Shader : public Object {
 public:
  /** Compile a default built-in shader */
  virtual void Compile(ShaderCompileTarget target) = 0;

  /** Compile a shader from the source file */
  virtual void Compile(String filename, ShaderCompileTarget target) = 0;

  /** Compile a shader from memory */
  virtual void Compile(const char *src, size_t size,
                       ShaderCompileTarget target) = 0;

  /** Get the compiled shader code */
  virtual void GetCodeBytes(void **bytes, size_t *size) = 0;

  /** Set the specified shader entry.
   * This call is optional, the default entry is "main".
   */
  virtual void SetEntryPoint(const char *entry) = 0;

  /** Set the compile flag */
  virtual void SetFlag(const uint32_t flag) = 0;

  virtual ShaderCompileTarget GetShaderTarget() const = 0;
};
}  // namespace core
}  // namespace engine
}  // namespace ixr

#endif  // IXR_ENGINE_CORE_SHADER_SHADER_H_
