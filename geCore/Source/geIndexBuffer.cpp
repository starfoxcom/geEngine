/*****************************************************************************/
/**
 * @file    geIndexBuffer.cpp
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2018/06/08
 * @brief   Hardware buffer that hold indices of vertices in a vertex buffer.
 *
 * Hardware buffer that hold indices of vertices in a vertex buffer.
 *
 * @bug     No known bugs.
 */
/*****************************************************************************/

/*****************************************************************************/
/**
 * Includes
 */
/*****************************************************************************/
#include "geIndexBuffer.h"
#include "geHardwareBufferManager.h"
#include "geRenderAPI.h"

namespace geEngineSDK {
  using std::static_pointer_cast;

  uint32
  calcIndexSize(IndexType type) {
    switch (type)
    {
      case IT_16BIT:
        return sizeof(uint16);
      default:
      case IT_32BIT:
        return sizeof(uint32);
    }
  }

  IndexBufferProperties::IndexBufferProperties(IndexType idxType,
                                               uint32 numIndices)
    : mIndexType(idxType),
      mNumIndices(numIndices),
      mIndexSize(calcIndexSize(idxType))
  {}

  IndexBuffer::IndexBuffer(const INDEX_BUFFER_DESC& desc)
    : mProperties(desc.indexType, desc.numIndices),
      mUsage(desc.usage)
  {}

  SPtr<geCoreThread::IndexBuffer>
  IndexBuffer::getCore() const {
    return static_pointer_cast<geCoreThread::IndexBuffer>(m_coreSpecific);
  }

  SPtr<geCoreThread::CoreObject>
  IndexBuffer::createCore() const {
    INDEX_BUFFER_DESC desc;
    desc.indexType = mProperties.mIndexType;
    desc.numIndices = mProperties.mNumIndices;
    desc.usage = mUsage;

    return geCoreThread::HardwareBufferManager::instance().createIndexBufferInternal(desc);
  }

  SPtr<IndexBuffer>
  IndexBuffer::create(const INDEX_BUFFER_DESC& desc) {
    return HardwareBufferManager::instance().createIndexBuffer(desc);
  }

  namespace geCoreThread {
    IndexBuffer::IndexBuffer(const INDEX_BUFFER_DESC& desc,
                             GPU_DEVICE_FLAGS::E /*deviceMask*/)
      : HardwareBuffer(calcIndexSize(desc.indexType) * desc.numIndices),
        mProperties(desc.indexType, desc.numIndices)
    {}

    SPtr<IndexBuffer>
    IndexBuffer::create(const INDEX_BUFFER_DESC& desc,
                        GPU_DEVICE_FLAGS::E deviceMask) {
      return HardwareBufferManager::instance().createIndexBuffer(desc, deviceMask);
    }
  }
}
