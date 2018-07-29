/********************************************************************
Copyright 2018 Intel Corp. All Rights Reserved.
Description : texture dx11
Author      : Wenyi Tang
Email       : wenyi.tang@intel.com
Created     : Feb. 28th, 2018
********************************************************************/
#ifndef IXR_ENGINE_LOW_LEVEL_DX11_TEXTURE_DX11_H_
#define IXR_ENGINE_LOW_LEVEL_DX11_TEXTURE_DX11_H_
#include <d3d11.h>
#include <wrl/client.h>
#include "core/texture/texture.h"

namespace ixr {
namespace engine {
class Env;

namespace dx11 {
/** Dx11 texture implementation */
class Dx11Texture : public core::Texture {
 public:
  template <typename T>
  using CPtr = Microsoft::WRL::ComPtr<T>;

  Dx11Texture(engine::Env *e);
  virtual ~Dx11Texture();

  virtual Handle GetHandle() override;

  virtual TextureDesc GetDesc() const override;

  virtual void CreateTexture(TextureDesc desc) override;

  virtual void CreateReferenceFrom(Handle texture, TextureType type) override;

  virtual void CopyTo(Texture *dst) override;

  virtual void Map(void **data, uint32_t *pitch, uint32_t flag) override;

  virtual void Unmap() override;

  virtual SRef<core::TextureMapData> Map(uint32_t flag) override;

  virtual Handle AsShaderResource() override;

  virtual Handle AsRenderTarget() override;

  virtual Handle AsDepthStencil();

 private:
  ID3D11Device *device() const;

 private:
  engine::Env *env_;
  CPtr<ID3D11Texture1D> tex1d_;
  CPtr<ID3D11Texture2D> tex2d_;
  CPtr<ID3D11Texture3D> tex3d_;
  CPtr<ID3D11Resource> resource_;
  CPtr<ID3D11ShaderResourceView> srv_;
  CPtr<ID3D11RenderTargetView> rtv_;
  CPtr<ID3D11DepthStencilView> dsv_;
  union {
    D3D11_TEXTURE1D_DESC desc1d_;
    D3D11_TEXTURE2D_DESC desc2d_;
    D3D11_TEXTURE3D_DESC desc3d_;
  };
};

class Dx11TextureMapData : public core::TextureMapData {
 public:
  Dx11TextureMapData(Dx11Texture::CPtr<ID3D11Resource> tex,
                     Dx11Texture::CPtr<ID3D11DeviceContext> ctx, uint32_t flag);
  virtual ~Dx11TextureMapData();
  virtual void UpdateFromRawData(const void *raw, uint32_t pitch) override;
  virtual void ReadIntoMemory(void *dst, uint32_t pitch) override;
  virtual bool IsValid() const override;

 private:
  Dx11Texture::CPtr<ID3D11Resource> res_;
  Dx11Texture::CPtr<ID3D11DeviceContext> ctx_;
  D3D11_MAPPED_SUBRESOURCE data_;
  bool valid_;
};
}  // namespace dx11
}  // namespace engine
}  // namespace ixr
#endif  // IXR_ENGINE_LOW_LEVEL_DX11_TEXTURE_DX11_H_
