/********************************************************************
Copyright 2018 Intel Corp. All Rights Reserved.
Description : swapchain interface
Author      : Wenyi Tang
Email       : wenyi.tang@intel.com
Created     : Mar. 6th, 2018
********************************************************************/
#include "low_level/dx11/swapchain_dx11.h"
#include "engine/engine.h"
#include "low_level/dx11/mapper_dx11.h"

namespace ixr {
namespace engine {
namespace dx11 {
using namespace core;

Dx11SwapChain::Dx11SwapChain(Env *e) : env_(e) {}

Dx11SwapChain::~Dx11SwapChain() {}

void Dx11SwapChain::CreateSwapChain(const SwapChainDesc &sd) {
  DXGI_SWAP_CHAIN_DESC1 desc{};
  desc.BufferCount = sd.buffer_count;
  desc.Format = ToDxgiFormat(sd.format);
  desc.Width = sd.width;
  desc.Height = sd.height;
  desc.Stereo = FALSE;
  desc.SampleDesc.Count = sd.sample_count;
  desc.SampleDesc.Quality = 0;
  desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
  desc.Scaling = DXGI_SCALING_STRETCH;
  desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
  desc.AlphaMode = DXGI_ALPHA_MODE_IGNORE;
  desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

  DXGI_SWAP_CHAIN_FULLSCREEN_DESC fullscreen{};
  fullscreen.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
  fullscreen.RefreshRate.Denominator = 1;
  fullscreen.RefreshRate.Numerator = sd.refresh_rate;
  fullscreen.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
  fullscreen.Windowed = sd.windowed;

  HWND window = static_cast<HWND>(sd.window_handle);
  ID3D11Device *dev = static_cast<ID3D11Device *>(env_->GetHandle().pthis);
  CPtr<IDXGIDevice> dxgi_device;
  CPtr<IDXGIAdapter> dxgi_adapter;
  CPtr<IDXGIFactory2> factory;
  HRESULT hr = CreateDXGIFactory1(IID_PPV_ARGS(&factory));
  THROW_IF_FAIL(FAILED(hr), "Fail to create dxgi factory2");
  hr = factory->CreateSwapChainForHwnd(dev, window, &desc, &fullscreen, nullptr,
                                       &swapchain_);
  THROW_IF_FAIL(FAILED(hr), "Fail to create swapchain");
}

Texture *Dx11SwapChain::GetBuffer(uint32_t index) {
  CPtr<ID3D11Texture2D> backbuffer;
  HRESULT hr = swapchain_->GetBuffer(index, IID_PPV_ARGS(&backbuffer));
  THROW_IF_FAIL(FAILED(hr), "Fail to get buffer from swapchain");
  TextureDesc td;
  td.ref = backbuffer.Get();
  td.type = TEXTURE_TYPE_2D;
  auto p = env_->NewTexture(td);
  return p;
}

void Dx11SwapChain::Present(bool vsync) {
  HRESULT hr = swapchain_->Present(vsync ? 1 : 0, 0);
  THROW_IF_FAIL(FAILED(hr), "Fail to present");
}

void Dx11SwapChain::Resize(uint32_t w, uint32_t h) {
  UINT flag = 0;
  HRESULT hr = swapchain_->ResizeBuffers(0, w, h, DXGI_FORMAT_UNKNOWN, flag);
  THROW_IF_FAIL(FAILED(hr), "Fail to resize swapchain buffer");
}

void Dx11SwapChain::GetSize(uint32_t *w, uint32_t *h, float *ratio) {
  CPtr<ID3D11Texture2D> backbuffer;
  HRESULT hr = swapchain_->GetBuffer(0, IID_PPV_ARGS(&backbuffer));
  THROW_IF_FAIL(FAILED(hr), "Fail to get buffer from swapchain");
  D3D11_TEXTURE2D_DESC desc;
  backbuffer->GetDesc(&desc);
  if (w && h) {
    *w = desc.Width;
    *h = desc.Height;
  }
  if (ratio) {
    *ratio = float(desc.Width) / float(desc.Height);
  }
}
}
}
}
