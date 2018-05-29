/*****************************************************************************/
/**
 * @file    geCommandBufferManager.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2018/05/29
 * @brief   Handles creation of command buffers.
 *
 * Handles creation of command buffers. See CommandBuffer.
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
#include "gePrerequisitesCore.h"

#include <geModule.h>

namespace geEngineSDK {
  namespace geCoreThread {
    /**
     * @brief Handles creation of command buffers. See CommandBuffer.
     * @note  Core thread only.
     */
    class GE_CORE_EXPORT CommandBufferManager : public Module<CommandBufferManager>
    {
     public:
      virtual ~CommandBufferManager() = default;

      /**
       * @copydoc CommandBuffer::create
       */
      SPtr<CommandBuffer>
      create(GPU_QUEUE_TYPE::E type,
             uint32 deviceIdx = 0,
             uint32 queueIdx = 0,
             bool secondary = false);

     protected:
      friend CommandBuffer;

      /**
       * @brief Creates a command buffer with the specified ID. See create().
       */
      virtual SPtr<CommandBuffer>
      createInternal(GPU_QUEUE_TYPE::E type,
                     uint32 deviceIdx = 0,
                     uint32 queueIdx = 0,
                     bool secondary = false) = 0;
    };
  }
}
