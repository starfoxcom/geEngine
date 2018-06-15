/*****************************************************************************/
/**
 * @file    geD3D11IndexBuffer.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2018/06/14
 * @brief   DirectX 11 implementation of an index buffer.
 *
 * DirectX 11 implementation of an index buffer.
 *
 * @bug     No known bugs.
 */
/*****************************************************************************/
#pragma once

/*****************************************************************************/
/**
 * Includes
 */
/*****************************************************************************/
#include "gePrerequisitesD3D11.h"
#include "geD3D11HardwareBuffer.h"

#include <geIndexBuffer.h>

namespace geEngineSDK {
  namespace geCoreThread {
    class D3D11IndexBuffer : public IndexBuffer
    {
     public:
      D3D11IndexBuffer(D3D11Device& device,
                       const INDEX_BUFFER_DESC& desc,
                       GPU_DEVICE_FLAGS::E deviceMask);

      ~D3D11IndexBuffer();

      /**
       * @copydoc IndexBuffer::readData
       */
      void
      readData(uint32 offset,
               uint32 length,
               void* dest,
               uint32 deviceIdx = 0,
               uint32 queueIdx = 0) override;

      /**
       * @copydoc IndexBuffer::writeData
       */
      void
      writeData(uint32 offset,
                uint32 length,
                const void* source,
                BUFFER_WRITE_TYPE::E writeFlags = BUFFER_WRITE_TYPE::kNORMAL,
                uint32 queueIdx = 0) override;

      /**
       * @copydoc IndexBuffer::copyData
       */
      void
      copyData(HardwareBuffer& srcBuffer,
               uint32 srcOffset,
               uint32 dstOffset,
               uint32 length,
               bool discardWholeBuffer = false,
               const SPtr<CommandBuffer>& commandBuffer = nullptr) override;

      /**
       * @brief Gets the internal DX11 index buffer object.
       */
      ID3D11Buffer*
      getD3DIndexBuffer() const {
        return m_buffer->getD3DBuffer();
      }

     protected:
      /**
       * @copydoc IndexBuffer::map
       */
      void*
      map(uint32 offset,
          uint32 length,
          GPU_LOCK_OPTIONS::E options,
          uint32 deviceIdx,
          uint32 queueIdx) override;

      /**
       * @copydoc IndexBuffer::unmap
       */
      void
      unmap() override;

      /**
       * @copydoc IndexBuffer::initialize
       */
      void
      initialize() override;

      D3D11HardwareBuffer* m_buffer;
      D3D11Device& m_device;
      GPU_BUFFER_USAGE::E m_usage;
    };
  }
}
