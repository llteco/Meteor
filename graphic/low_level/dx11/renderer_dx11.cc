/********************************************************************
Copyright 2018 Intel Corp. All Rights Reserved.
Description : dx11 implement
Author      : Wenyi Tang
Email       : wenyi.tang@intel.com
Created     : Feb. 28th, 2018
********************************************************************/
#include "low_level/dx11/renderer_dx11.h"
#include "engine/engine.h"
#include "errors/error.h"
#include "low_level/dx11/mapper_dx11.h"
#include "low_level/dx11/texture_dx11.h"

namespace ixr {
namespace engine {
namespace dx11 {
using namespace core;

Dx11Renderer::Dx11Renderer(engine::Env *e) : env_(e) {}

Dx11Renderer::~Dx11Renderer() {}

void Dx11Renderer::CreateDevice(uint32_t id) {
  CPtr<IDXGIFactory2> factory;
  HRESULT hr = CreateDXGIFactory1(IID_PPV_ARGS(&factory));
  THROW_IF_FAIL(FAILED(hr), "Failed to create IDXGIFactory1");
  uint32_t adap_index = 0;
  CPtr<IDXGIAdapter1> adapter;
  // enumerate all adapters
  while (SUCCEEDED(hr)) {
    hr = factory->EnumAdapters1(adap_index, &adapter);
    if (SUCCEEDED(hr)) {
      DXGI_ADAPTER_DESC1 adap_desc{};
      adapter->GetDesc1(&adap_desc);
      if (adap_desc.VendorId == id) {
        vid_ = static_cast<AdapterVendorId>(id);
        break;
      }
      adap_index++;
    }
  }
  UINT flag = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
#ifdef _DEBUG
  // flag |= D3D11_CREATE_DEVICE_DEBUG;
#endif
  if (adapter) {
    hr = D3D11CreateDevice(adapter.Get(), D3D_DRIVER_TYPE_UNKNOWN, 0, flag,
                           nullptr, 0, D3D11_SDK_VERSION, &device_, nullptr,
                           &context_);
    THROW_IF_FAIL(FAILED(hr), "Failed to create main dx11 device");
  } else {
    hr = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, 0, flag, nullptr,
                           0, D3D11_SDK_VERSION, &device_, nullptr, &context_);
    THROW_IF_FAIL(FAILED(hr), "Failed to create main dx11 device");
    CPtr<IDXGIDevice2> dxgidev;
    CPtr<IDXGIAdapter> dxgiada;
    device_.As(&dxgidev);
    dxgidev->GetAdapter(&dxgiada);
    DXGI_ADAPTER_DESC adap_desc{};
    dxgiada->GetDesc(&adap_desc);
    vid_ = static_cast<AdapterVendorId>(adap_desc.VendorId);
  }
  env_->ChangeEnv({device_.Get(), vid_, GRAPHIC_API_DX11});
}

void Dx11Renderer::SetInputLayout(const VertexElements &v,
                                  Shader *vertex_shader) {
  std::vector<D3D11_INPUT_ELEMENT_DESC> input_elements;
  std::map<String, uint32_t> semantic_index;
  uint32_t offset = 0;
  for (auto &v_ : v) {
    if (semantic_index.find(v_.GetName()) != semantic_index.end()) {
      semantic_index[v_.GetName()]++;
    } else {
      semantic_index[v_.GetName()] = 0;
    }
    input_elements.push_back({v_.GetName().c_str(),
                              semantic_index[v_.GetName()],
                              ToDxgiFormat(v_.GetType()), v_.GetSlot(), offset,
                              D3D11_INPUT_PER_VERTEX_DATA, 0});
    offset += v_.GetSize();
  }
  void *code_bytes = nullptr;
  size_t code_size = 0;
  vertex_shader->GetCodeBytes(&code_bytes, &code_size);
  CPtr<ID3D11InputLayout> input_layout;
  HRESULT hr = device_->CreateInputLayout(
      input_elements.data(), static_cast<uint32_t>(input_elements.size()),
      code_bytes, code_size, &input_layout);
  THROW_IF_FAIL(FAILED(hr), "Fail to create input layout");
  Cache(input_layout);
  context_->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
  context_->IASetInputLayout(input_layout.Get());
}

void Dx11Renderer::SetVertexBySlot(const SlotVertex &slots) {
  ETHROW("Unimplemented!");
}

void Dx11Renderer::SetVertexAndIndex(GpuBuffer *vertice, uint32_t stride,
                                     GpuBuffer *indice) {
  auto pv = vertice->GetHandle();
  THROW_IF_FAIL(!pv, "Invalid vertex buffer!");
  ID3D11Buffer *vertex_buffer = static_cast<ID3D11Buffer *>(pv);
  const uint32_t _offset = 0;
  const uint32_t _stride = stride;
  context_->IASetVertexBuffers(0, 1, &vertex_buffer, &_stride, &_offset);
  auto pi = indice->GetHandle();
  THROW_IF_FAIL(!pi, "No vertex buffer!");
  ID3D11Buffer *index_buffer = static_cast<ID3D11Buffer *>(pi);
  context_->IASetIndexBuffer(index_buffer, DXGI_FORMAT_R16_UINT, 0);
}

void Dx11Renderer::SetShader(Shader *shader) {
  auto shader_target = shader->GetShaderTarget();
  void *code_bytes = nullptr;
  size_t code_size = 0;
  shader->GetCodeBytes(&code_bytes, &code_size);
  HRESULT hr;
  switch (shader_target) {
    case SHADER_TARGET_VERTEX: {
      CPtr<ID3D11VertexShader> vs;
      hr = device_->CreateVertexShader(code_bytes, code_size, nullptr, &vs);
      THROW_IF_FAIL(FAILED(hr), "Fail to create vs");
      context_->VSSetShader(vs.Get(), nullptr, 0);
      Cache(vs);
      break;
    }
    case SHADER_TARGET_PIXEL: {
      CPtr<ID3D11PixelShader> ps;
      hr = device_->CreatePixelShader(code_bytes, code_size, nullptr, &ps);
      THROW_IF_FAIL(FAILED(hr), "Fail to create ps");
      context_->PSSetShader(ps.Get(), nullptr, 0);
      Cache(ps);
      break;
    }
    default:
      ETHROW("Unknown shader target %d", shader_target);
  }
}

void Dx11Renderer::SetSampler() {
  D3D11_SAMPLER_DESC ssd{};
  ssd.AddressU = ssd.AddressV = ssd.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
  ssd.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
  ssd.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
  ssd.MaxAnisotropy = 0;
  ssd.MaxLOD = 0;
  CPtr<ID3D11SamplerState> ss;
  HRESULT hr = device_->CreateSamplerState(&ssd, &ss);
  THROW_IF_FAIL(FAILED(hr), "Fail to create sampler state");
  context_->PSSetSamplers(0, 1, &ss);
  Cache(ss);
}

void Dx11Renderer::SetRasterizer(uint32_t rasterizer_desc) {
  D3D11_RASTERIZER_DESC rsd = ToRasterizerDesc(RasterizerDesc(rasterizer_desc));
  CPtr<ID3D11RasterizerState> rs;
  HRESULT hr = device_->CreateRasterizerState(&rsd, &rs);
  THROW_IF_FAIL(FAILED(hr), "Fail to create rasterizer state");
  context_->RSSetState(rs.Get());
  Cache(rs);
}

void Dx11Renderer::SetViewport(float w, float h, float x, float y, float min,
                               float max) {
  D3D11_VIEWPORT vp{};
  vp.Width = w;
  vp.Height = h;
  vp.TopLeftX = x;
  vp.TopLeftY = y;
  vp.MinDepth = min;
  vp.MaxDepth = max;
  context_->RSSetViewports(1, &vp);
}

void Dx11Renderer::SetScissor(int32_t left, int32_t top, int32_t right,
                              int32_t bottom) {
  CD3D11_RECT rect(left, top, right, bottom);
  context_->RSSetScissorRects(1, &rect);
}

void Dx11Renderer::ClearRenderTarget(Texture *tex, const float color[4]) {
  THROW_IF_FAIL(!tex, "No texture in rendertarget");
  Handle view = tex->AsRenderTarget();
  ID3D11RenderTargetView *rtv = static_cast<ID3D11RenderTargetView *>(view);
  context_->ClearRenderTargetView(rtv, color);
}

void Dx11Renderer::ClearDepthStencil(Texture *depth) {
  Dx11Texture *t = dynamic_cast<Dx11Texture *>(&(*depth));
  Handle view = t->AsDepthStencil();
  ID3D11DepthStencilView *dsv = static_cast<ID3D11DepthStencilView *>(view);
  context_->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
                                  1.0f, 0);
}

void Dx11Renderer::SetRenderTargets(const RenderTargets &rt, Texture *depth) {
  Dx11Texture *d = dynamic_cast<Dx11Texture *>(&(*depth));
  ID3D11DepthStencilView *dsv = nullptr;
  if (d) {
    Handle view = d->AsDepthStencil();
    dsv = static_cast<ID3D11DepthStencilView *>(view);
  }
  std::vector<ID3D11RenderTargetView *> rtvs;
  for (auto &r : rt) {
    Handle view = r->AsRenderTarget();
    rtvs.push_back(static_cast<ID3D11RenderTargetView *>(view));
  }
  uint32_t num_views = static_cast<uint32_t>(rtvs.size());
  context_->OMSetRenderTargets(num_views, rtvs.data(), dsv);
}

void Dx11Renderer::SetDepthStencil() {
  D3D11_DEPTH_STENCIL_DESC dsd{};
  dsd.DepthEnable = TRUE;
  dsd.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
  dsd.DepthFunc = D3D11_COMPARISON_ALWAYS;
  dsd.StencilEnable = FALSE;
  dsd.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
  dsd.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
  dsd.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
  dsd.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
  dsd.BackFace = dsd.FrontFace;
  CPtr<ID3D11DepthStencilState> ds;
  HRESULT hr = device_->CreateDepthStencilState(&dsd, &ds);
  THROW_IF_FAIL(FAILED(hr), "Fail to create depth stencil state");
  context_->OMSetDepthStencilState(ds.Get(), 0);
  Cache(ds);
}

void Dx11Renderer::SetShaderResources(Texture *tex) {
  Handle view = tex->AsShaderResource();
  ID3D11ShaderResourceView *srv = static_cast<ID3D11ShaderResourceView *>(view);
  context_->PSSetShaderResources(0, 1, &srv);
}

void Dx11Renderer::SetBlendState(const float (&factor)[4], uint32_t mask) {
  D3D11_BLEND_DESC bld{};
  bld.AlphaToCoverageEnable = FALSE;
  bld.IndependentBlendEnable = FALSE;
  bld.RenderTarget[0].BlendEnable = TRUE;
  bld.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
  bld.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
  bld.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
  bld.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
  bld.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
  bld.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
  bld.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
  bld.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
  bld.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
  bld.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
  bld.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
  bld.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
  bld.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
  bld.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
  CPtr<ID3D11BlendState> bs;
  HRESULT hr = device_->CreateBlendState(&bld, &bs);
  THROW_IF_FAIL(FAILED(hr), "Fail to create blend state");
  context_->OMSetBlendState(bs.Get(), factor, mask);
  Cache(bs);
}

void Dx11Renderer::SetModelViewPerspectiveProjection(float mvp[16]) {
  CPtr<ID3D11Buffer> cb;
  context_->VSGetConstantBuffers(0, 1, &cb);
  if (cb) {
    /* We don't use context_->UpdateSubresource() because such call
     * could delay one frame to take effect. */
    auto data = mvp_cbuffer_->Map();
    data->UpdateFromRawData(mvp, 0, sizeof(float) * 16);
  } else {
    BufferDesc bd{};
    bd.bytes = mvp;
    bd.size = 16 * sizeof(float);
    bd.usage = BUFFER_USAGE_CONSTANT | BUFFER_USAGE_CPUWRITE;
    mvp_cbuffer_ = env_->NewGpuBuffer(bd);
    auto mvp_handle = static_cast<ID3D11Buffer *>(mvp_cbuffer_->GetHandle());
    context_->VSSetConstantBuffers(0, 1, &mvp_handle);
  }
}

void Dx11Renderer::Draw(int32_t count, int32_t offset, int32_t base) {
  context_->DrawIndexed(count, offset, base);
}

void Dx11Renderer::SetBarrier() {
  // Create a query event to sync gpu commands
  D3D11_QUERY_DESC qd{D3D11_QUERY_EVENT, 0};
  THROW_IF_FAIL(FAILED(device_->CreateQuery(&qd, &query_)),
                "Failed to create QUERY_EVENT");
  context_->End(query_.Get());
  context_->Flush();
}

void Dx11Renderer::SyncBarrier() {
  HRESULT hr;
  do {
    hr = context_->GetData(query_.Get(), NULL, 0, 0);
  } while (FAILED(hr));
  query_.Reset();
}

bool Dx11Renderer::IsCached(const CPtr<IUnknown> &p) {
  auto key = reinterpret_cast<uint64_t>(p.Get());
  return IsCached(key);
}

bool Dx11Renderer::IsCached(const uint64_t &key) {
  if (cached_coms_.find(key) == cached_coms_.end()) {
    return false;
  }
  return true;
}

void Dx11Renderer::Cache(const CPtr<IUnknown> &p, uint64_t key) {
  uint64_t k = reinterpret_cast<uint64_t>(p.Get());
  if (key != std::numeric_limits<uint64_t>::max()) {
    k = key;
  }
  if (IsCached(k)) {
    cached_coms_.erase(k);
  }
  cached_coms_[k] = p;
}
}  // namespace dx11
}  // namespace engine
}  // namespace ixr
