/*****************************************************************************/
/**
 * @file    geD3D11CommandBufferManager.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2018/05/29
 * @brief   Handles creation of DirectX 11 command buffers. See CommandBuffer.
 *
 * Handles creation of DirectX 11 command buffers. See CommandBuffer.
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

#include <geCommandBufferManager.h>

namespace geEngineSDK {
  namespace geCoreThread {
    /**
     * @brief Handles creation of DirectX 11 command buffers. See CommandBuffer.
     * @note  Core thread only.
     */
    class D3D11CommandBufferManager : public CommandBufferManager
    {
     public:
      /**
       * @copydoc CommandBufferManager::createInternal()
       */
      SPtr<CommandBuffer>
      createInternal(GPU_QUEUE_TYPE::E type,
                     uint32 deviceIdx = 0,
                     uint32 queueIdx = 0,
                     bool secondary = false) override;
    };
  }
}
