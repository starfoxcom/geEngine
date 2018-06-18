/*****************************************************************************/
/**
 * @file    geD3D11HardwareBufferManager.cpp
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2018/06/16
 * @brief   Handles creation of DirectX 11 hardware buffers.
 *
 * Handles creation of DirectX 11 hardware buffers.
 *
 * @bug     No known bugs.
 */
/*****************************************************************************/

/*****************************************************************************/
/**
 * Includes
 */
/*****************************************************************************/
#include "geD3D11HardwareBufferManager.h"
#include "geD3D11VertexBuffer.h"
#include "geD3D11IndexBuffer.h"
#include "geD3D11GPUBuffer.h"
#include "geD3D11GPUParamBlockBuffer.h"

#include <geGPUParamDesc.h>

namespace geEngineSDK {
  namespace geCoreThread {
    D3D11HardwareBufferManager::D3D11HardwareBufferManager(D3D11Device& device)
      : m_device(device)
    {}

    SPtr<VertexBuffer>
    D3D11HardwareBufferManager::createVertexBufferInternal(const VERTEX_BUFFER_DESC& desc,
                                                           GPU_DEVICE_FLAGS::E deviceMask) {
      auto ret = ge_shared_ptr_new<D3D11VertexBuffer>(m_device, desc, deviceMask);
      ret->_setThisPtr(ret);

      return ret;
    }

    SPtr<IndexBuffer>
    D3D11HardwareBufferManager::createIndexBufferInternal(const INDEX_BUFFER_DESC& desc,
                                                          GPU_DEVICE_FLAGS::E deviceMask) {
      auto ret = ge_shared_ptr_new<D3D11IndexBuffer>(m_device, desc, deviceMask);
      ret->_setThisPtr(ret);

      return ret;
    }

    SPtr<GPUParamBlockBuffer>
    D3D11HardwareBufferManager::createGPUParamBlockBufferInternal(uint32 size,
                                                             GPU_PARAM_BLOCK_USAGE::E usage,
                                                             GPU_DEVICE_FLAGS::E deviceMask) {
      auto paramBlockBuffer = GE_PVT_NEW(D3D11GPUParamBlockBuffer, size, usage, deviceMask);
      auto paramBlockBufferPtr = ge_shared_ptr<D3D11GPUParamBlockBuffer>(paramBlockBuffer);
      paramBlockBufferPtr->_setThisPtr(paramBlockBufferPtr);

      return paramBlockBufferPtr;
    }

    SPtr<GPUBuffer>
    D3D11HardwareBufferManager::createGPUBufferInternal(const GPU_BUFFER_DESC& desc,
                                                        GPU_DEVICE_FLAGS::E deviceMask) {
      auto buffer = GE_PVT_NEW(D3D11GPUBuffer, desc, deviceMask);
      auto bufferPtr = ge_shared_ptr<D3D11GPUBuffer>(buffer);
      bufferPtr->_setThisPtr(bufferPtr);

      return bufferPtr;
    }
  }
}
