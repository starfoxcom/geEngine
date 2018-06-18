/*****************************************************************************/
/**
 * @file    geD3D11GPUParamBlockBuffer.cpp
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

/*****************************************************************************/
/**
 * Includes
 */
/*****************************************************************************/
#include "geD3D11GPUParamBlockBuffer.h"
#include "geD3D11HardwareBuffer.h"
#include "geD3D11RenderAPI.h"
#include "geD3D11Device.h"

#include <geRenderStats.h>

namespace geEngineSDK {
  using std::ref;

  namespace geCoreThread {
    D3D11GPUParamBlockBuffer::D3D11GPUParamBlockBuffer(uint32 size,
                                                       GPU_PARAM_BLOCK_USAGE::E usage,
                                                       GPU_DEVICE_FLAGS::E deviceMask)
      : GPUParamBlockBuffer(size, usage, deviceMask),
        m_buffer(nullptr) {
      GE_ASSERT((GPU_DEVICE_FLAGS::kDEFAULT == deviceMask||
                 GPU_DEVICE_FLAGS::kPRIMARY == deviceMask) &&
                "Multiple GPUs not supported natively on DirectX 11.");
    }

    D3D11GPUParamBlockBuffer::~D3D11GPUParamBlockBuffer() {
      if (nullptr != m_buffer) {
        ge_delete(m_buffer);
      }

      GE_INC_RENDER_STAT_CAT(ResDestroyed, RENDER_STAT_RESOURCE_TYPE::kGPUParamBuffer);
    }

    void
    D3D11GPUParamBlockBuffer::initialize() {
      auto d3d11rs = static_cast<D3D11RenderAPI*>(RenderAPI::instancePtr());
      D3D11Device& device = d3d11rs->getPrimaryDevice();

      if (GPU_PARAM_BLOCK_USAGE::kSTATIC == m_usage) {
        m_buffer = ge_new<D3D11HardwareBuffer>(BUFFER_TYPE::kCONSTANT,
                                               GPU_BUFFER_USAGE::kSTATIC,
                                               1,
                                               m_size,
                                               ref(device));
      }
      else if (GPU_PARAM_BLOCK_USAGE::kDYNAMIC == m_usage) {
        m_buffer = ge_new<D3D11HardwareBuffer>(BUFFER_TYPE::kCONSTANT,
                                               GPU_BUFFER_USAGE::kDYNAMIC,
                                               1,
                                               m_size,
                                               ref(device));
      }
      else {
        GE_EXCEPT(InternalErrorException, "Invalid GPU param block usage.");
      }

      GE_INC_RENDER_STAT_CAT(ResCreated, RENDER_STAT_RESOURCE_TYPE::kGPUParamBuffer);

      GPUParamBlockBuffer::initialize();
    }


    ID3D11Buffer*
    D3D11GPUParamBlockBuffer::getD3D11Buffer() const {
      return m_buffer->getD3DBuffer();
    }

    void
    D3D11GPUParamBlockBuffer::writeToGPU(const uint8* data, uint32 /*queueIdx*/) {
      m_buffer->writeData(0, m_size, data, BUFFER_WRITE_TYPE::kDISCARD);
      GE_INC_RENDER_STAT_CAT(ResWrite, RENDER_STAT_RESOURCE_TYPE::kGPUParamBuffer);
    }
  }
}
