/********************************************************************
Copyright 2018 Intel Corp. All Rights Reserved.
Description : texture
Author      : Wenyi Tang
Email       : wenyi.tang@intel.com
Created     : Feb. 28th, 2018
********************************************************************/
#include "low_level/dx11/texture_dx11.h"
#include "engine/engine.h"
#include "errors/error.h"
#include "low_level/dx11/mapper_dx11.h"

#include <algorithm>

namespace ixr {
namespace engine {
namespace dx11 {
using namespace core;

inline uint32_t getPitchBy(DXGI_FORMAT f, uint32_t width) {
  switch (f) {
    case DXGI_FORMAT_R32G32B32A32_TYPELESS:
    case DXGI_FORMAT_R32G32B32A32_FLOAT:
    case DXGI_FORMAT_R32G32B32A32_UINT:
    case DXGI_FORMAT_R32G32B32A32_SINT:
      return width * 16;
    case DXGI_FORMAT_R32G32B32_TYPELESS:
    case DXGI_FORMAT_R32G32B32_FLOAT:
    case DXGI_FORMAT_R32G32B32_UINT:
    case DXGI_FORMAT_R32G32B32_SINT:
      return width * 12;
    case DXGI_FORMAT_R16G16B16A16_TYPELESS:
    case DXGI_FORMAT_R16G16B16A16_FLOAT:
    case DXGI_FORMAT_R16G16B16A16_UNORM:
    case DXGI_FORMAT_R16G16B16A16_UINT:
    case DXGI_FORMAT_R16G16B16A16_SNORM:
    case DXGI_FORMAT_R16G16B16A16_SINT:
    case DXGI_FORMAT_R32G32_TYPELESS:
    case DXGI_FORMAT_R32G32_FLOAT:
    case DXGI_FORMAT_R32G32_UINT:
    case DXGI_FORMAT_R32G32_SINT:
    case DXGI_FORMAT_R32G8X24_TYPELESS:
    case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
    case DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS:
    case DXGI_FORMAT_X32_TYPELESS_G8X24_UINT:
      return width * 8;
    case DXGI_FORMAT_R10G10B10A2_TYPELESS:
    case DXGI_FORMAT_R10G10B10A2_UNORM:
    case DXGI_FORMAT_R10G10B10A2_UINT:
    case DXGI_FORMAT_R11G11B10_FLOAT:
    case DXGI_FORMAT_R8G8B8A8_TYPELESS:
    case DXGI_FORMAT_R8G8B8A8_UNORM:
    case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
    case DXGI_FORMAT_R8G8B8A8_UINT:
    case DXGI_FORMAT_R8G8B8A8_SNORM:
    case DXGI_FORMAT_R8G8B8A8_SINT:
    case DXGI_FORMAT_R16G16_TYPELESS:
    case DXGI_FORMAT_R16G16_FLOAT:
    case DXGI_FORMAT_R16G16_UNORM:
    case DXGI_FORMAT_R16G16_UINT:
    case DXGI_FORMAT_R16G16_SNORM:
    case DXGI_FORMAT_R16G16_SINT:
    case DXGI_FORMAT_R32_TYPELESS:
    case DXGI_FORMAT_D32_FLOAT:
    case DXGI_FORMAT_R32_FLOAT:
    case DXGI_FORMAT_R32_UINT:
    case DXGI_FORMAT_R32_SINT:
    case DXGI_FORMAT_R24G8_TYPELESS:
    case DXGI_FORMAT_D24_UNORM_S8_UINT:
    case DXGI_FORMAT_R24_UNORM_X8_TYPELESS:
    case DXGI_FORMAT_X24_TYPELESS_G8_UINT:
    case DXGI_FORMAT_R8G8_B8G8_UNORM:
    case DXGI_FORMAT_G8R8_G8B8_UNORM:
    case DXGI_FORMAT_B8G8R8A8_UNORM:
    case DXGI_FORMAT_B8G8R8X8_UNORM:
    case DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM:
    case DXGI_FORMAT_B8G8R8A8_TYPELESS:
    case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
    case DXGI_FORMAT_B8G8R8X8_TYPELESS:
    case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB:
      return width * 4;
    case DXGI_FORMAT_R8G8_TYPELESS:
    case DXGI_FORMAT_R8G8_UNORM:
    case DXGI_FORMAT_R8G8_UINT:
    case DXGI_FORMAT_R8G8_SNORM:
    case DXGI_FORMAT_R8G8_SINT:
    case DXGI_FORMAT_R16_TYPELESS:
    case DXGI_FORMAT_R16_FLOAT:
    case DXGI_FORMAT_D16_UNORM:
    case DXGI_FORMAT_R16_UNORM:
    case DXGI_FORMAT_R16_UINT:
    case DXGI_FORMAT_R16_SNORM:
    case DXGI_FORMAT_R16_SINT:
    case DXGI_FORMAT_B5G6R5_UNORM:
    case DXGI_FORMAT_B5G5R5A1_UNORM:
    case DXGI_FORMAT_B4G4R4A4_UNORM:
      return width * 2;
    case DXGI_FORMAT_R8_TYPELESS:
    case DXGI_FORMAT_R8_UNORM:
    case DXGI_FORMAT_R8_UINT:
    case DXGI_FORMAT_R8_SNORM:
    case DXGI_FORMAT_R8_SINT:
    case DXGI_FORMAT_A8_UNORM:
      return width;
    default:
      ETHROW("invalid format");
      break;
  }
  return 0;
}

Dx11Texture::Dx11Texture(Env *e) : env_(e) {}

Dx11Texture::~Dx11Texture() {}

Handle Dx11Texture::GetHandle() { return resource_.Get(); }

TextureDesc Dx11Texture::GetDesc() const {
  D3D11_RESOURCE_DIMENSION rd;
  resource_->GetType(&rd);
  switch (rd) {
    case D3D11_RESOURCE_DIMENSION_UNKNOWN:
      break;
    case D3D11_RESOURCE_DIMENSION_BUFFER:
      break;
    case D3D11_RESOURCE_DIMENSION_TEXTURE1D:
      break;
    case D3D11_RESOURCE_DIMENSION_TEXTURE2D: {
      D3D11_TEXTURE2D_DESC td;
      tex2d_->GetDesc(&td);
      return {};
    }
    case D3D11_RESOURCE_DIMENSION_TEXTURE3D:
      break;
    default:
      break;
  }
  return TextureDesc();
}

void Dx11Texture::CreateTexture(TextureDesc desc) {
  ID3D11Device *dev = reinterpret_cast<ID3D11Device *>(env_->GetHandle().pthis);
  HRESULT hr = E_NOT_SET;
  switch (desc.type) {
    case TEXTURE_TYPE_1D:
      break;
    case TEXTURE_TYPE_2D: {
      D3D11_TEXTURE2D_DESC td{};
      td.ArraySize = desc.array_slices;
      td.Format = ToDxgiFormat(desc.format);
      td.Width = desc.width;
      td.Height = desc.height;
      td.MipLevels = desc.mips;
      td.SampleDesc.Count = desc.sample_count;
      td.SampleDesc.Quality = 0;
      td.BindFlags = ToBindFlag(static_cast<TextureUsage>(desc.usage));
      td.CPUAccessFlags = ToCpuAccess(static_cast<TextureUsage>(desc.usage));
      td.Usage = ToUsage(static_cast<TextureUsage>(desc.usage));
      if (desc.data) {
        D3D11_SUBRESOURCE_DATA data{};
        data.pSysMem = desc.data;
        data.SysMemPitch = getPitchBy(td.Format, td.Width);
        hr = dev->CreateTexture2D(&td, &data, &tex2d_);
      } else {
        hr = dev->CreateTexture2D(&td, nullptr, &tex2d_);
      }
      tex2d_.As(&resource_);
      tex2d_->GetDesc(&desc2d_);
      break;
    }
    case TEXTURE_TYPE_3D:
      break;
  }
  THROW_IF_FAIL(FAILED(hr), "Fail to create texture");
}

void Dx11Texture::CreateReferenceFrom(Handle texture, TextureType type) {
  // texture handle can be either view or resource
  ID3D11View *view = static_cast<ID3D11View *>(texture);
  ID3D11Resource *resource = static_cast<ID3D11Resource *>(texture);
  HRESULT hr = S_OK;
  D3D11_RESOURCE_DIMENSION resource_type;
  switch (type) {
    case TEXTURE_TYPE_1D:
      hr = resource->QueryInterface(IID_PPV_ARGS(&tex1d_));
      THROW_IF_FAIL(FAILED(hr), "Fail to query texture1d");
      tex1d_.As(&resource_);
      tex1d_->GetDesc(&desc1d_);
      break;
    case TEXTURE_TYPE_2D:
      hr = resource->QueryInterface(IID_PPV_ARGS(&tex2d_));
      THROW_IF_FAIL(FAILED(hr), "Fail to query texture2d");
      tex2d_.As(&resource_);
      tex2d_->GetDesc(&desc2d_);
      break;
    case TEXTURE_TYPE_3D:
      hr = resource->QueryInterface(IID_PPV_ARGS(&tex3d_));
      THROW_IF_FAIL(FAILED(hr), "Fail to query texture3d");
      tex3d_.As(&resource_);
      tex3d_->GetDesc(&desc3d_);
      break;
    case TEXTURE_TYPE_VIEW:
      hr = view->QueryInterface(IID_PPV_ARGS(&rtv_));
      if (FAILED(hr)) {
        hr = view->QueryInterface(IID_PPV_ARGS(&srv_));
      }
      THROW_IF_FAIL(FAILED(hr), "Fail to query texture view");
      view->GetResource(&resource_);
      resource_->GetType(&resource_type);
      switch (resource_type) {
        case D3D11_RESOURCE_DIMENSION_TEXTURE1D:
          resource_.As(&tex1d_);
          tex1d_->GetDesc(&desc1d_);
          break;
        case D3D11_RESOURCE_DIMENSION_TEXTURE2D:
          resource_.As(&tex2d_);
          tex2d_->GetDesc(&desc2d_);
          break;
        case D3D11_RESOURCE_DIMENSION_TEXTURE3D:
          resource_.As(&tex3d_);
          tex3d_->GetDesc(&desc3d_);
          break;
        case D3D11_RESOURCE_DIMENSION_UNKNOWN:
        case D3D11_RESOURCE_DIMENSION_BUFFER:
        default:
          ETHROW("Unsupported type %d", resource_type);
          break;
      }
      break;
    default:
      ETHROW("Unknown texture type %d", type);
  }
}

void Dx11Texture::CopyTo(Texture *dst) {
  ID3D11Device *dev = device();
  CPtr<ID3D11DeviceContext> context;
  dev->GetImmediateContext(&context);
  ID3D11Resource *dst_d3d = static_cast<ID3D11Resource *>(dst->GetHandle());
  context->CopyResource(dst_d3d, resource_.Get());
}

void Dx11Texture::Map(void **data, uint32_t *pitch, uint32_t flag) {
  ID3D11Device *dev = device();
  CPtr<ID3D11DeviceContext> context;
  dev->GetImmediateContext(&context);
  D3D11_MAPPED_SUBRESOURCE texture_data;
  HRESULT hr;
  if (flag & TEX_MAP_DO_NOT_WAIT) {
    D3D11_MAP type = static_cast<D3D11_MAP>(flag ^ TEX_MAP_DO_NOT_WAIT);
    hr = context->Map(resource_.Get(), 0, type, D3D11_MAP_FLAG_DO_NOT_WAIT,
                      &texture_data);
    if (hr == DXGI_ERROR_WAS_STILL_DRAWING) {
      *data = nullptr;
      *pitch = 0;
      return;
    }
  } else {
    D3D11_MAP type = static_cast<D3D11_MAP>(flag);
    hr = context->Map(resource_.Get(), 0, type, 0, &texture_data);
  }
  THROW_IF_FAIL(FAILED(hr), "Fail to map texture");
  *pitch = texture_data.RowPitch;
  *data = texture_data.pData;
}

void Dx11Texture::Unmap() {
  ID3D11Device *dev = device();
  CPtr<ID3D11DeviceContext> context;
  dev->GetImmediateContext(&context);
  context->Unmap(resource_.Get(), 0);
}

SRef<core::TextureMapData> Dx11Texture::Map(uint32_t flag) {
  ID3D11Device *dev = device();
  CPtr<ID3D11DeviceContext> context;
  dev->GetImmediateContext(&context);
  return MakeSRef<Dx11TextureMapData>(resource_, context, flag);
}

Handle Dx11Texture::AsShaderResource() {
  ID3D11Device *dev = device();
  if (srv_) {
    return srv_.Get();
  }
  HRESULT hr = dev->CreateShaderResourceView(tex2d_.Get(), nullptr, &srv_);
  THROW_IF_FAIL(FAILED(hr), "Fail to create shader resource view");
  return srv_.Get();
}

Handle Dx11Texture::AsRenderTarget() {
  ID3D11Device *dev = device();
  if (rtv_) {
    return rtv_.Get();
  }
  HRESULT hr = dev->CreateRenderTargetView(tex2d_.Get(), nullptr, &rtv_);
  THROW_IF_FAIL(FAILED(hr), "Fail to create render target view");
  return rtv_.Get();
}

Handle Dx11Texture::AsDepthStencil() {
  ID3D11Device *dev = device();
  if (dsv_) {
    return dsv_.Get();
  }
  HRESULT hr = dev->CreateDepthStencilView(tex2d_.Get(), nullptr, &dsv_);
  THROW_IF_FAIL(FAILED(hr), "Fail to create depth stencil view");
  return dsv_.Get();
}

ID3D11Device *Dx11Texture::device() const {
  return reinterpret_cast<ID3D11Device *>(env_->GetHandle().pthis);
}

Dx11TextureMapData::Dx11TextureMapData(
    Dx11Texture::CPtr<ID3D11Resource> tex,
    Dx11Texture::CPtr<ID3D11DeviceContext> ctx, uint32_t flag)
    : res_(tex), ctx_(ctx), valid_(false) {
  HRESULT hr;
  if (flag & TEX_MAP_DO_NOT_WAIT) {
    D3D11_MAP type = static_cast<D3D11_MAP>(flag ^ TEX_MAP_DO_NOT_WAIT);
    hr = ctx_->Map(res_.Get(), 0, type, D3D11_MAP_FLAG_DO_NOT_WAIT, &data_);
    if (hr == DXGI_ERROR_WAS_STILL_DRAWING) {
      return;
    }
  } else {
    D3D11_MAP type = static_cast<D3D11_MAP>(flag);
    hr = ctx_->Map(res_.Get(), 0, type, 0, &data_);
  }
  THROW_IF_FAIL(FAILED(hr), "Fail to map texture");
  valid_ = true;
}

Dx11TextureMapData::~Dx11TextureMapData() {
  if (valid_) {
    ctx_->Unmap(res_.Get(), 0);
  }
}

void Dx11TextureMapData::UpdateFromRawData(const void *raw, uint32_t pitch) {
  D3D11_RESOURCE_DIMENSION resource_type;
  Dx11Texture::CPtr<ID3D11Texture1D> tex1d;
  Dx11Texture::CPtr<ID3D11Texture2D> tex2d;
  Dx11Texture::CPtr<ID3D11Texture3D> tex3d;
  union {
    D3D11_TEXTURE1D_DESC desc1d;
    D3D11_TEXTURE2D_DESC desc2d;
    D3D11_TEXTURE3D_DESC desc3d;
  };
  res_->GetType(&resource_type);
  switch (resource_type) {
    case D3D11_RESOURCE_DIMENSION_TEXTURE1D:
      res_.As(&tex1d);
      tex1d->GetDesc(&desc1d);
      ETHROW("Unsupported type %d", resource_type);
      break;
    case D3D11_RESOURCE_DIMENSION_TEXTURE2D:
      res_.As(&tex2d);
      tex2d->GetDesc(&desc2d);
      for (uint32_t row = 0; row < desc2d.Height; row++) {
        uint32_t src_pitch =
            pitch ? pitch : getPitchBy(desc2d.Format, desc2d.Width);
        char *dst = static_cast<char *>(data_.pData) + row * data_.RowPitch;
        const char *src = static_cast<const char *>(raw) + row * src_pitch;
        memcpy_s(dst, data_.RowPitch, src, src_pitch);
      }
      break;
    case D3D11_RESOURCE_DIMENSION_TEXTURE3D:
      res_.As(&tex3d);
      tex3d->GetDesc(&desc3d);
      ETHROW("Unsupported type %d", resource_type);
      break;
    case D3D11_RESOURCE_DIMENSION_UNKNOWN:
    case D3D11_RESOURCE_DIMENSION_BUFFER:
    default:
      ETHROW("Unsupported type %d", resource_type);
      break;
  }
}

void Dx11TextureMapData::ReadIntoMemory(void *raw, uint32_t pitch) {
  D3D11_RESOURCE_DIMENSION resource_type;
  Dx11Texture::CPtr<ID3D11Texture1D> tex1d;
  Dx11Texture::CPtr<ID3D11Texture2D> tex2d;
  Dx11Texture::CPtr<ID3D11Texture3D> tex3d;
  union {
    D3D11_TEXTURE1D_DESC desc1d;
    D3D11_TEXTURE2D_DESC desc2d;
    D3D11_TEXTURE3D_DESC desc3d;
  };
  res_->GetType(&resource_type);
  switch (resource_type) {
    case D3D11_RESOURCE_DIMENSION_TEXTURE1D:
      res_.As(&tex1d);
      tex1d->GetDesc(&desc1d);
      ETHROW("Unsupported type %d", resource_type);
      break;
    case D3D11_RESOURCE_DIMENSION_TEXTURE2D:
      res_.As(&tex2d);
      tex2d->GetDesc(&desc2d);
      for (uint32_t row = 0; row < desc2d.Height; row++) {
        uint32_t dst_pitch =
            pitch ? pitch : getPitchBy(desc2d.Format, desc2d.Width);
        char *src = static_cast<char *>(data_.pData) + row * data_.RowPitch;
        char *dst = static_cast<char *>(raw) + row * dst_pitch;
        memcpy_s(dst, dst_pitch, src, std::min(data_.RowPitch, dst_pitch));
      }
      break;
    case D3D11_RESOURCE_DIMENSION_TEXTURE3D:
      res_.As(&tex3d);
      tex3d->GetDesc(&desc3d);
      ETHROW("Unsupported type %d", resource_type);
      break;
    case D3D11_RESOURCE_DIMENSION_UNKNOWN:
    case D3D11_RESOURCE_DIMENSION_BUFFER:
    default:
      ETHROW("Unsupported type %d", resource_type);
      break;
  }
}

bool Dx11TextureMapData::IsValid() const { return valid_; }

}  // namespace dx11
}  // namespace engine
}  // namespace ixr
