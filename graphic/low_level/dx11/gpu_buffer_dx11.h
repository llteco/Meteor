/********************************************************************
Copyright 2018 Intel Corp. All Rights Reserved.
Description : gpu buffer
Author      : Wenyi Tang
Email       : wenyi.tang@intel.com
Created     : Mar. 6th, 2018
********************************************************************/
#ifndef IXR_ENGINE_LOW_LEVEL_DX11_GPU_BUFFER_DX11_H_
#define IXR_ENGINE_LOW_LEVEL_DX11_GPU_BUFFER_DX11_H_
#include <d3d11.h>
#include <wrl/client.h>
#include "core/gpu_buffer/gpu_buffer.h"

namespace ixr {
namespace engine {
namespace dx11 {

class Dx11GpuBuffer : public core::GpuBuffer {
 public:
  template <typename T>
  using CPtr = Microsoft::WRL::ComPtr<T>;

  Dx11GpuBuffer(const NativeDeviceHandle &h);

  virtual ~Dx11GpuBuffer();

  virtual void CreateBuffer(const BufferDesc &desc);

  virtual Handle GetHandle() const override;

  virtual void Update(const void *src) override;

  virtual void Map(void **ptr, uint32_t *size) override;

  virtual void Unmap() override;

  virtual SRef<core::GpuBufferMapData> Map() override;

 private:
  NativeDeviceHandle handle_;
  CPtr<ID3D11Buffer> buffer_;
};

class Dx11GpuBufferMapData : public core::GpuBufferMapData {
 public:
  Dx11GpuBufferMapData(Dx11GpuBuffer::CPtr<ID3D11Buffer> bf,
                       Dx11GpuBuffer::CPtr<ID3D11DeviceContext> ctx);
  virtual ~Dx11GpuBufferMapData();
  virtual void UpdateFromRawData(const void *src, uint32_t offset,
                                 uint32_t length) override;

 private:
  Dx11GpuBuffer::CPtr<ID3D11Buffer> object_;
  Dx11GpuBuffer::CPtr<ID3D11DeviceContext> context_;
  D3D11_MAPPED_SUBRESOURCE data_;
};
}  // namespace dx11
}  // namespace engine
}  // namespace ixr
#endif  // IXR_ENGINE_LOW_LEVEL_DX11_GPU_BUFFER_DX11_H_
