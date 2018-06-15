/*****************************************************************************/
/**
 * @file    geD3D11IndexBuffer.cpp
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2018/06/14
 * @brief   DirectX 11 implementation of an index buffer.
 *
 * DirectX 11 implementation of an index buffer.
 *
 * @bug     No known bugs.
 */
/*****************************************************************************/

/*****************************************************************************/
/**
 * Includes
 */
/*****************************************************************************/
#include "geD3D11IndexBuffer.h"
#include "geD3D11Device.h"

#include <geRenderStats.h>

namespace geEngineSDK {
  using std::ref;

  namespace geCoreThread {
    D3D11IndexBuffer::D3D11IndexBuffer(D3D11Device& device,
                                       const INDEX_BUFFER_DESC& desc,
                                       GPU_DEVICE_FLAGS::E deviceMask)
      : IndexBuffer(desc, deviceMask),
        m_buffer(nullptr),
        m_device(device),
        m_usage(desc.usage) {
      GE_ASSERT((GPU_DEVICE_FLAGS::kDEFAULT == deviceMask ||
                 GPU_DEVICE_FLAGS::kPRIMARY == deviceMask) &&
                "Multiple GPUs not supported natively on DirectX.");
    }

    D3D11IndexBuffer::~D3D11IndexBuffer() {
      if (nullptr != m_buffer) {
        ge_delete(m_buffer);
      }

      GE_INC_RENDER_STAT_CAT(ResDestroyed, RENDER_STAT_RESOURCE_TYPE::kIndexBuffer);
    }

    void
    D3D11IndexBuffer::initialize() {
      m_buffer = ge_new<D3D11HardwareBuffer>(BUFFER_TYPE::kINDEX,
                                            m_usage,
                                            1,
                                            m_size,
                                            ref(m_device),
                                            false);

      GE_INC_RENDER_STAT_CAT(ResCreated, RENDER_STAT_RESOURCE_TYPE::kIndexBuffer);
      IndexBuffer::initialize();
    }

    void*
    D3D11IndexBuffer::map(uint32 offset,
                          uint32 length,
                          GPU_LOCK_OPTIONS::E options,
                          uint32 /*deviceIdx*/,
                          uint32 /*queueIdx*/) {
#if GE_PROFILING_ENABLED
      if (GPU_LOCK_OPTIONS::kREAD_ONLY == options ||
          GPU_LOCK_OPTIONS::kREAD_WRITE == options) {
        GE_INC_RENDER_STAT_CAT(ResRead, RENDER_STAT_RESOURCE_TYPE::kIndexBuffer);
      }

      if (GPU_LOCK_OPTIONS::kREAD_WRITE == options ||
          GPU_LOCK_OPTIONS::kWRITE_ONLY == options ||
          GPU_LOCK_OPTIONS::kWRITE_ONLY_DISCARD == options ||
          GPU_LOCK_OPTIONS::kWRITE_ONLY_NO_OVERWRITE == options) {
        GE_INC_RENDER_STAT_CAT(ResWrite, RENDER_STAT_RESOURCE_TYPE::kIndexBuffer);
      }
#endif
      return m_buffer->lock(offset, length, options);
    }

    void
    D3D11IndexBuffer::unmap() {
      m_buffer->unlock();
    }

    void
    D3D11IndexBuffer::readData(uint32 offset,
                               uint32 length,
                               void* dest,
                               uint32 /*deviceIdx*/,
                               uint32 /*queueIdx*/) {
      m_buffer->readData(offset, length, dest);
      GE_INC_RENDER_STAT_CAT(ResRead, RENDER_STAT_RESOURCE_TYPE::kIndexBuffer);
    }

    void
    D3D11IndexBuffer::writeData(uint32 offset,
                                uint32 length,
                                const void* pSource,
                                BUFFER_WRITE_TYPE::E writeFlags,
                                uint32 /*queueIdx*/) {
      m_buffer->writeData(offset, length, pSource, writeFlags);
      GE_INC_RENDER_STAT_CAT(ResWrite, RENDER_STAT_RESOURCE_TYPE::kIndexBuffer);
    }

    void
    D3D11IndexBuffer::copyData(HardwareBuffer& srcBuffer,
                               uint32 srcOffset,
                               uint32 dstOffset,
                               uint32 length,
                               bool discardWholeBuffer,
                               const SPtr<CommandBuffer>& commandBuffer) {
      m_buffer->copyData(srcBuffer,
                         srcOffset,
                         dstOffset,
                         length,
                         discardWholeBuffer,
                         commandBuffer);
    }
  }
}
