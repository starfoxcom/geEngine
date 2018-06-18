/*****************************************************************************/
/**
 * @file    geD3D11GPUParamBlockBuffer.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2018/06/16
 * @brief   DirectX 11 implementation of a parameter block buffer.
 *
 * DirectX 11 implementation of a parameter block buffer
 * (constant buffer in DX11 lingo).
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

#include <geGPUParamBlockBuffer.h>

namespace geEngineSDK {
  namespace geCoreThread {
    class D3D11GPUParamBlockBuffer : public GPUParamBlockBuffer
    {
     public:
      D3D11GPUParamBlockBuffer(uint32 size,
                               GPU_PARAM_BLOCK_USAGE::E usage,
                               GPU_DEVICE_FLAGS::E deviceMask);
      ~D3D11GPUParamBlockBuffer();

      /**
       * @copydoc GPUParamBlockBuffer::writeToGPU
       */
      void
      writeToGPU(const uint8* data, uint32 queueIdx = 0) override;

      /**
       * @brief Returns internal DX11 buffer object.
       */
      ID3D11Buffer*
      getD3D11Buffer() const;

     protected:

      /**
       * @copydoc GPUParamBlockBuffer::initialize
       */
      void
      initialize() override;

     private:
      D3D11HardwareBuffer * m_buffer;
    };
  }
}
