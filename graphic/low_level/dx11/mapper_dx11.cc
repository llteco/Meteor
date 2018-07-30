/********************************************************************
Copyright 2018 Intel Corp. All Rights Reserved.
Description : format mapper
Author      : Wenyi Tang
Email       : wenyi.tang@intel.com
Created     : Feb. 28th, 2018
********************************************************************/
#include "low_level/dx11/mapper_dx11.h"

namespace ixr {
namespace engine {
namespace dx11 {

DXGI_FORMAT ToDxgiFormat(PixelFormat fmt) {
  switch (fmt) {
    case PF_FLOAT32:
      return DXGI_FORMAT_R32_FLOAT;
    case PF_RGBA32_UNORM:
      return DXGI_FORMAT_R8G8B8A8_UNORM;
    case PF_BGRA32_UNORM:
      return DXGI_FORMAT_B8G8R8A8_UNORM;
    case PF_R8_UNORM:
      return DXGI_FORMAT_R8_UNORM;
    case PF_R8G8_UNORM:
      return DXGI_FORMAT_R8G8_UNORM;
    case PF_DEPTHSTENCIL:
      return DXGI_FORMAT_D24_UNORM_S8_UINT;
    default:
      return DXGI_FORMAT_UNKNOWN;
  }
}

DXGI_FORMAT ToDxgiFormat(VertexElementType type) {
  switch (type) {
    case VET_FLOAT1:
      return DXGI_FORMAT_D32_FLOAT;
    case VET_FLOAT2:
      return DXGI_FORMAT_R32G32_FLOAT;
    case VET_FLOAT3:
      return DXGI_FORMAT_R32G32B32_FLOAT;
    case VET_FLOAT4:
      return DXGI_FORMAT_R32G32B32A32_FLOAT;
    case VET_UINT1:
      return DXGI_FORMAT_R32_UINT;
    case VET_UINT2:
      return DXGI_FORMAT_R32G32_UINT;
    case VET_UINT3:
      return DXGI_FORMAT_R32G32B32_UINT;
    case VET_UCHAR4:
      return DXGI_FORMAT_R8G8B8A8_UNORM;
    case VET_UNKNOWN:
    default:
      return DXGI_FORMAT_UNKNOWN;
  }
}

D3D11_USAGE ToUsage(TextureUsage usage) {
  if (usage & TU_STATIC) {
    return D3D11_USAGE_DEFAULT;
  } else if (usage & TU_STAGING) {
    return D3D11_USAGE_STAGING;
  } else if (usage & TU_DYNAMIC) {
    return D3D11_USAGE_DYNAMIC;
  } else {
    return D3D11_USAGE_DEFAULT;
  }
}

D3D11_USAGE ToUsage(BufferUsage usage) {
  if (usage & (BUFFER_USAGE_CPUWRITE | BUFFER_USAGE_CPUREAD)) {
    return D3D11_USAGE_DYNAMIC;
  }
  return D3D11_USAGE_DEFAULT;
}

D3D11_BIND_FLAG ToBindFlag(TextureUsage usage) {
  uint32_t flags = 0;
  if (usage & TU_SHADERRESOURCE) {
    flags |= D3D11_BIND_SHADER_RESOURCE;
  }
  if (usage & TU_RENDERTARGET) {
    flags |= D3D11_BIND_RENDER_TARGET;
  }
  if (usage & TU_DEPTHSTENCIL) {
    flags |= D3D11_BIND_DEPTH_STENCIL;
  }
  if (usage & TU_LOADSTORE) {
    flags |= D3D11_BIND_UNORDERED_ACCESS;
  }
  return static_cast<D3D11_BIND_FLAG>(flags);
}

D3D11_BIND_FLAG ToBindFlag(BufferUsage usage) {
  uint32_t flags = 0;
  if (usage & BUFFER_USAGE_VERTEX) {
    flags |= D3D11_BIND_VERTEX_BUFFER;
  } else if (usage & BUFFER_USAGE_INDICE) {
    flags |= D3D11_BIND_INDEX_BUFFER;
  } else if (usage & BUFFER_USAGE_CONSTANT) {
    flags |= D3D11_BIND_CONSTANT_BUFFER;
  }
  return D3D11_BIND_FLAG(flags);
}

D3D11_CPU_ACCESS_FLAG ToCpuAccess(TextureUsage usage) {
  uint32_t cpu_access = 0;
  if (usage & TU_CPUREADABLE) {
    cpu_access |= D3D11_CPU_ACCESS_READ;
  }
  if (usage & TU_CPUWRITABLE) {
    cpu_access |= D3D11_CPU_ACCESS_WRITE;
  }
  return static_cast<D3D11_CPU_ACCESS_FLAG>(cpu_access);
}
D3D11_CPU_ACCESS_FLAG ToCpuAccess(BufferUsage usage) {
  uint32_t cpu_access = 0;
  if (usage & BUFFER_USAGE_CPUREAD) {
    cpu_access |= D3D11_CPU_ACCESS_READ;
  }
  if (usage & BUFFER_USAGE_CPUWRITE) {
    cpu_access |= D3D11_CPU_ACCESS_WRITE;
  }
  return D3D11_CPU_ACCESS_FLAG(cpu_access);
}

D3D11_RASTERIZER_DESC ToRasterizerDesc(RasterizerDesc rd) {
  D3D11_RASTERIZER_DESC rsd{};
  if (rd & RS_CULL_BACK) {
    rsd.CullMode = D3D11_CULL_BACK;
  } else if (rd & RS_CULL_FRONT) {
    rsd.CullMode = D3D11_CULL_FRONT;
  } else if (rd & RS_CULL_NONE) {
    rsd.CullMode = D3D11_CULL_NONE;
  }
  if (rd & RS_FILL_SOLID) {
    rsd.FillMode = D3D11_FILL_SOLID;
  } else if (rd & RS_FILL_WIRE) {
    rsd.FillMode = D3D11_FILL_WIREFRAME;
  }
  if (rd & RS_SCISSOR_ENABLE) {
    rsd.ScissorEnable = true;
  }
  return rsd;
}
}  // namespace dx11
}  // namespace engine
}  // namespace ixr
