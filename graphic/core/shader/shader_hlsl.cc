/********************************************************************
Copyright 2018 Intel Corp. All Rights Reserved.
Description : HLSL shader implementation
Author      : Wenyi Tang
Email       : wenyi.tang@intel.com
Created     : Feb. 28th, 2018
********************************************************************/
#include "core/shader/shader_hlsl.h"
#include "errors/error.h"

namespace ixr {
namespace engine {
namespace core {

constexpr char HLSL_VS_TARGET[] = "vs_5_0";
constexpr char HLSL_PS_TARGET[] = "ps_5_0";

ShaderHLSL::ShaderHLSL() : entry_("main"), flag_(0) {
  blob_ = nullptr;
  compiled_ = false;
}

ShaderHLSL::~ShaderHLSL() {
  if (blob_) {
    blob_->Release();
    blob_ = nullptr;
  }
}

void ShaderHLSL::Compile(ShaderCompileTarget target) {
  const char DefaultVertexShader[] = R"(
    cbuffer MVPConstantBuffer : register(b0) {
      matrix mvp;
    }

    struct VertexShaderInput {
      float3 pos : POSITION;
      float2 tex0 : TEXCOORD0;
    };

    struct PixelShaderInput {
      float4 pos : SV_POSITION;
      float2 tex0 : TEXCOORD0;
    };

    PixelShaderInput main(VertexShaderInput vertex) {
      PixelShaderInput vsOut;
      vsOut.pos = mul(mvp, float4(vertex.pos, 1.0f));
      vsOut.tex0 = vertex.tex0;
      return vsOut;
    }
  )";
  const char DefaultPixelShader[] = R"(
    Texture2D texture0;
    SamplerState colorSampler;

    struct PixelShaderInput {
      float4 pos : SV_POSITION;
      float2 tex0 : TEXCOORD0;
    };

    float4 main(PixelShaderInput frag): SV_TARGET{
      float4 tex_rgb = texture0.Sample(colorSampler, frag.tex0);
      return tex_rgb;
    }
  )";
  switch (target) {
    case SHADER_TARGET_VERTEX:
      return Compile(DefaultVertexShader, strlen(DefaultVertexShader), target);
    case SHADER_TARGET_PIXEL:
      return Compile(DefaultPixelShader, strlen(DefaultPixelShader), target);
    default:
      ETHROW("Unknown shader target %d", target);
  }
}

void ShaderHLSL::Compile(String filename, ShaderCompileTarget target) {
  WString wfilename(filename.begin(), filename.end());
  String tg;
  switch (target) {
    case SHADER_TARGET_VERTEX:
      tg = HLSL_VS_TARGET;
      break;
    case SHADER_TARGET_PIXEL:
      tg = HLSL_PS_TARGET;
      break;
    default:
      ETHROW("Unknown shader target %d", target);
  }
  if (blob_) {
    blob_->Release();
    blob_ = nullptr;
  }
  HRESULT hr =
      D3DCompileFromFile(wfilename.c_str(), nullptr, nullptr, entry_.c_str(),
                         tg.c_str(), 0, 0, &blob_, nullptr);
  THROW_IF_FAIL(FAILED(hr), "Fail to compile shader from %s", filename.c_str());
  compiled_ = true;
  target_ = target;
}

void ShaderHLSL::Compile(const char *src, size_t size,
                         ShaderCompileTarget target) {
  String tg;
  switch (target) {
    case SHADER_TARGET_VERTEX:
      tg = HLSL_VS_TARGET;
      break;
    case SHADER_TARGET_PIXEL:
      tg = HLSL_PS_TARGET;
      break;
    default:
      ETHROW("Unknown shader target %d", target);
  }
  if (blob_) {
    blob_->Release();
    blob_ = nullptr;
  }
  HRESULT hr = D3DCompile(src, size, nullptr, nullptr, nullptr, entry_.c_str(),
                          tg.c_str(), 0, 0, &blob_, nullptr);
  THROW_IF_FAIL(FAILED(hr), "Fail to compile shader from source");
  compiled_ = true;
  target_ = target;
}

void ShaderHLSL::GetCodeBytes(void **bytes, size_t *size) {
  THROW_IF_FAIL(!bytes || !size, "Null pointers!");
  THROW_IF_FAIL(!compiled_, "Shader may not be compiled");
  *bytes = blob_->GetBufferPointer();
  *size = blob_->GetBufferSize();
}

void ShaderHLSL::SetEntryPoint(const char *entry) { entry_ = entry; }

void ShaderHLSL::SetFlag(const uint32_t flag) { flag_ = flag; }

ShaderCompileTarget ShaderHLSL::GetShaderTarget() const {
  return ShaderCompileTarget(target_);
}
}  // namespace core
}  // namespace engine
}  // namespace ixr
