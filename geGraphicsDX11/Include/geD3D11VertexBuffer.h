/*****************************************************************************/
/**
 * @file    geD3D11VertexBuffer.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2018/06/13
 * @brief   DirectX 11 implementation of a vertex buffer.
 *
 * DirectX 11 implementation of a vertex buffer.
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

#include <geVertexBuffer.h>

namespace geEngineSDK {
  namespace geCoreThread {
    class D3D11VertexBuffer : public VertexBuffer
    {
     public:
      D3D11VertexBuffer(D3D11Device& device,
                        const VERTEX_BUFFER_DESC& desc,
                        GPU_DEVICE_FLAGS::E deviceMask);

      ~D3D11VertexBuffer();

      /**
       * @copydoc VertexBuffer::readData
       */
      void
      readData(uint32 offset,
               uint32 length,
               void* dest,
               uint32 deviceIdx = 0,
               uint32 queueIdx = 0) override;

      /**
       * @copydoc VertexBuffer::writeData
       */
      void
      writeData(uint32 offset,
                uint32 length,
                const void* source,
                BUFFER_WRITE_TYPE::E writeFlags = BUFFER_WRITE_TYPE::kNORMAL,
                uint32 queueIdx = 0) override;

      /**
       * @copydoc VertexBuffer::copyData
       */
      void
      copyData(HardwareBuffer& srcBuffer,
               uint32 srcOffset,
               uint32 dstOffset,
               uint32 length,
               bool discardWholeBuffer = false,
               const SPtr<CommandBuffer>& commandBuffer = nullptr) override;

      /**
       * @brief Get the D3D-specific index buffer
       */
      ID3D11Buffer*
      getD3DVertexBuffer() const {
        return m_buffer->getD3DBuffer();
      }

     protected:
      /**
       * @copydoc VertexBuffer::map
       */
      void*
      map(uint32 offset,
          uint32 length,
          GPU_LOCK_OPTIONS::E options,
          uint32 deviceIdx,
          uint32 queueIdx) override;

      /**
       * @copydoc VertexBuffer::unmap
       */
      void
      unmap(void) override;

      /**
       * @copydoc VertexBuffer::initialize
       */
      void
      initialize() override;

      D3D11HardwareBuffer* m_buffer;
      D3D11Device& m_device;
      bool m_streamOut;
      GPU_BUFFER_USAGE::E m_usage;
    };
  }
}
