/********************************************************************
Copyright 2018 Intel Corp. All Rights Reserved.
Description : gpu buffer
Author      : Wenyi Tang
Email       : wenyi.tang@intel.com
Created     : Mar. 6th, 2018
********************************************************************/
#include "low_level/dx11/gpu_buffer_dx11.h"
#include "errors/error.h"
#include "low_level/dx11/mapper_dx11.h"

namespace ixr {
namespace engine {
namespace dx11 {
Dx11GpuBuffer::Dx11GpuBuffer(const NativeDeviceHandle &h) : handle_(h) {}

Dx11GpuBuffer::~Dx11GpuBuffer() {}

void Dx11GpuBuffer::CreateBuffer(const BufferDesc &desc) {
  ID3D11Device *dev = static_cast<ID3D11Device *>(handle_.pthis);
  D3D11_BUFFER_DESC dc{};
  dc.ByteWidth = static_cast<uint32_t>(desc.size);
  dc.CPUAccessFlags = ToCpuAccess(BufferUsage(desc.usage));
  dc.BindFlags = ToBindFlag(BufferUsage(desc.usage));
  dc.Usage = ToUsage(BufferUsage(desc.usage));
  std::unique_ptr<D3D11_SUBRESOURCE_DATA> data;
  if (desc.bytes) {
    data.reset(new D3D11_SUBRESOURCE_DATA());
    data->pSysMem = desc.bytes;
    data->SysMemPitch = static_cast<uint32_t>(desc.pitch);
  }
  THROW_IF_FAIL(FAILED(dev->CreateBuffer(&dc, data.get(), &buffer_)),
                "Fail to create buffer");
}

Handle Dx11GpuBuffer::GetHandle() const { return buffer_.Get(); }

void Dx11GpuBuffer::Update(const void *src) {
  ID3D11Device *dev = static_cast<ID3D11Device *>(handle_.pthis);
  CPtr<ID3D11DeviceContext> context;
  dev->GetImmediateContext(&context);
  context->UpdateSubresource(buffer_.Get(), 0, nullptr, src, 0, 0);
}

void Dx11GpuBuffer::Map(void **ptr, uint32_t *size) {
  ID3D11Device *dev = static_cast<ID3D11Device *>(handle_.pthis);
  CPtr<ID3D11DeviceContext> context;
  dev->GetImmediateContext(&context);
  D3D11_MAPPED_SUBRESOURCE res;
  THROW_IF_FAIL(
      FAILED(context->Map(buffer_.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &res)),
      "Fail to map gpu buffer");
  *ptr = res.pData;
  *size = res.RowPitch;
}

void Dx11GpuBuffer::Unmap() {
  ID3D11Device *dev = static_cast<ID3D11Device *>(handle_.pthis);
  CPtr<ID3D11DeviceContext> context;
  dev->GetImmediateContext(&context);
  context->Unmap(buffer_.Get(), 0);
}

SRef<core::GpuBufferMapData> Dx11GpuBuffer::Map() {
  ID3D11Device *dev = static_cast<ID3D11Device *>(handle_.pthis);
  CPtr<ID3D11DeviceContext> context;
  dev->GetImmediateContext(&context);
  return MakeSRef<Dx11GpuBufferMapData>(buffer_, context);
}

Dx11GpuBufferMapData::Dx11GpuBufferMapData(
    Dx11GpuBuffer::CPtr<ID3D11Buffer> bf,
    Dx11GpuBuffer::CPtr<ID3D11DeviceContext> ctx)
    : object_(bf), context_(ctx) {
  // @Todo GpuBuffer always map to write.
  THROW_IF_FAIL(FAILED(context_->Map(object_.Get(), 0, D3D11_MAP_WRITE_DISCARD,
                                     0, &data_)),
                "Fail to map gpu buffer");
}

Dx11GpuBufferMapData::~Dx11GpuBufferMapData() {
  context_->Unmap(object_.Get(), 0);
}

void Dx11GpuBufferMapData::UpdateFromRawData(const void *src, uint32_t offset,
                                             uint32_t length) {
  memcpy_s(static_cast<char *>(data_.pData) + offset, data_.RowPitch - offset,
           src, length);
}
}  // namespace dx11
}  // namespace engine
}  // namespace ixr