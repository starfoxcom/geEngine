/*****************************************************************************/
/**
 * @file    geD3D11CommandBufferManager.cpp
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2018/05/29
 * @brief   Handles creation of DirectX 11 command buffers. See CommandBuffer.
 *
 * Handles creation of DirectX 11 command buffers. See CommandBuffer.
 *
 * @bug     No known bugs.
 */
/*****************************************************************************/

/*****************************************************************************/
/**
 * Includes
 */
/*****************************************************************************/
#include "geD3D11CommandBufferManager.h"
#include "geD3D11CommandBuffer.h"

namespace geEngineSDK {
  namespace geCoreThread {
    SPtr<CommandBuffer>
    D3D11CommandBufferManager::createInternal(GPU_QUEUE_TYPE::E type,
                                              uint32 deviceIdx,
                                              uint32 queueIdx,
                                              bool secondary) {
      CommandBuffer* buffer = new (ge_alloc<D3D11CommandBuffer>())
        D3D11CommandBuffer(type, deviceIdx, queueIdx, secondary);
      return ge_shared_ptr(buffer);
    }
  }
}
