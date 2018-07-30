/********************************************************************
Copyright 2018 Intel Corp. All Rights Reserved.
Description : vertex element
Author      : Wenyi Tang
Email       : wenyi.tang@intel.com
Created     : Feb. 28th, 2018
********************************************************************/
#ifndef IXR_ENGINE_CORE_VERTEX_VERTEX_ELEMENT_H_
#define IXR_ENGINE_CORE_VERTEX_VERTEX_ELEMENT_H_
#include "engine/types.h"

namespace ixr {
namespace engine {

/**	Types used to identify base types of vertex element contents. */
enum VertexElementType {
  VET_FLOAT1 = 0, /**< 1D floating point value */
  VET_FLOAT2 = 1, /**< 2D floating point value */
  VET_FLOAT3 = 2, /**< 3D floating point value */
  VET_FLOAT4 = 3, /**< 4D floating point value */
  VET_UINT1 = 21, /**< 1D 32-bit signed integer value */
  VET_UINT2 = 22, /**< 2D 32-bit signed integer value */
  VET_UINT3 = 23, /**< 3D 32-bit signed integer value */
  VET_UCHAR4 = 28, /**< R8G8B8A8 color value */
  VET_UNKNOWN = 0xffff
};

namespace core {
/** VertexElement is the layout of the vertex input */
class VertexElement {
 public:
  VertexElement(String name, VertexElementType type, uint32_t slot = 0)
      : name_(name), type_(type), slot_(slot) {
    switch (type_) {
      case VET_UINT1:
      case VET_FLOAT1:
        size_ = 4;
        break;
      case VET_UINT2:
      case VET_FLOAT2:
        size_ = 8;
        break;
      case VET_UINT3:
      case VET_FLOAT3:
        size_ = 12;
        break;
      case VET_FLOAT4:
        size_ = 16;
        break;
      case VET_UNKNOWN:
      default:
        size_ = 0;
        break;
    }
  }

  const String &GetName() const { return name_; }

  VertexElementType GetType() const { return type_; }

  uint32_t GetSize() const { return size_; }

  uint32_t GetSlot() const { return slot_; }

 private:
  String name_;
  VertexElementType type_;
  uint32_t size_;
  uint32_t slot_;
};
}  // namespace core
}  // namespace engine
}  // namespace ixr
#endif  // IXR_ENGINE_CORE_VERTEX_VERTEX_ELEMENT_H_
