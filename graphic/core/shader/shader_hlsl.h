/********************************************************************
Copyright 2018 Intel Corp. All Rights Reserved.
Description : HLSL Shader
Author      : Wenyi Tang
Email       : wenyi.tang@intel.com
Created     : Feb. 28th, 2018
********************************************************************/
#ifndef IXR_ENGINE_CORE_SHADER_SHADER_HLSL_H_
#define IXR_ENGINE_CORE_SHADER_SHADER_HLSL_H_
#include <d3dcompiler.h>
#include "core/shader/shader.h"

namespace ixr {
namespace engine {
namespace core {
/* Compile, generate HLSL shader */
class ShaderHLSL : public Shader {
 public:
  ShaderHLSL();
  ~ShaderHLSL();

  virtual void Compile(ShaderCompileTarget target) override;

  virtual void Compile(String filename, ShaderCompileTarget target) override;

  virtual void Compile(const char *src, size_t size,
                       ShaderCompileTarget target) override;

  virtual void GetCodeBytes(void **bytes, size_t *size) override;

  virtual void SetEntryPoint(const char *entry) override;

  virtual void SetFlag(const uint32_t flag) override;

  virtual ShaderCompileTarget GetShaderTarget() const override;

 private:
  String entry_;
  uint32_t target_;
  uint32_t flag_;
  bool compiled_;
  ID3DBlob *blob_;
};
}  // namespace core
}  // namespace engine
}  // namespace ixr

#endif  // IXR_ENGINE_CORE_SHADER_SHADER_HLSL_H_
