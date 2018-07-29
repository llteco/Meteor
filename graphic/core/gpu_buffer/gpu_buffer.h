/********************************************************************
Copyright 2018 Intel Corp. All Rights Reserved.
Description : gpu buffer
Author      : Wenyi Tang
Email       : wenyi.tang@intel.com
Created     : Mar. 6th, 2018
********************************************************************/
#ifndef IXR_ENGINE_CORE_GPU_BUFFER_GPU_BUFFER_H_
#define IXR_ENGINE_CORE_GPU_BUFFER_GPU_BUFFER_H_
#include "engine/types.h"
#include "platform/native_object.h"

namespace ixr {
namespace engine {
/** Enumerate buffer usage cases
 * \example: BUFFER_USAGE_CONSTANT | BUFFER_USAGE_CPUREAD
 */
enum BufferUsage {
  /** The buffer used as a vertex buffer */
  BUFFER_USAGE_VERTEX = 0x1,
  /** The buffer used as an index buffer */
  BUFFER_USAGE_INDICE = 0x2,
  /** The buffer used as a constant buffer */
  BUFFER_USAGE_CONSTANT = 0x4,
  /** The buffer can be read out by CPU */
  BUFFER_USAGE_CPUREAD = 0x10,
  /** The buffer can be writen in by CPU */
  BUFFER_USAGE_CPUWRITE = 0x20,

  BUFFER_USAGE_DEFAULT = BUFFER_USAGE_CONSTANT
};

/** Buffer description to create a buffer */
struct BufferDesc {
  /* Buffer usage */
  uint32_t usage;
  /* Bytes to initiate the buffer */
  void *bytes;
  /* Size of the bytes */
  size_t size;
  /* Always 0 */
  size_t pitch;
};

namespace core {
/** Mapped data structure. @see GpuBuffer::Map() */
class GpuBufferMapData {
 public:
  virtual ~GpuBufferMapData() {}

  virtual void UpdateFromRawData(const void *src, uint32_t offset,
                                 uint32_t length) = 0;
};

/** Abstract class for buffers on GPU */
class GpuBuffer : public Object {
 public:
  /** Create the gpu buffer
   * \param [in] desc: buffer description
   * \return error::Ok() if succeed.
   */
  virtual void CreateBuffer(const BufferDesc &desc) = 0;

  /** Get the native handle of the buffer
   * \note the actual type of Handle depends on buffer implementation,
   * for d3d11, it's ID3D11Buffer*.
   */
  virtual Handle GetHandle() const = 0;

  /** Update the buffer bytes from CPU
   * \param [in] src: bytes to be updated, should be equal size to the creation.
   */
  virtual void Update(const void *src) = 0;

  /**
   * @brief map the dynamic or staging buffer to cpu memory
   *
   * @param ptr pointer to cpu memory
   * @param size bytes length
   * @deprecated, use Map() instead.
   */
  virtual void Map(void **ptr, uint32_t *size) = 0;

  /**
   * @brief unmap cpu memory. Must call after Map.
   * @deprecated, use Map() instead.
   */
  virtual void Unmap() = 0;

  /**
   * @brief map the dynamic or staging buffer as an object.
   * the unmap is called automatically when object desctructed.
   *
   * @return GpuBufferMapData
   */
  virtual SRef<GpuBufferMapData> Map() = 0;
};
}  // namespace core
}  // namespace engine
}  // namespace ixr
#endif  // IXR_ENGINE_CORE_GPU_BUFFER_GPU_BUFFER_H_
