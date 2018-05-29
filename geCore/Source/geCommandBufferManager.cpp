/*****************************************************************************/
/**
 * @file    geCommandBufferManager.cpp
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2018/05/29
 * @brief   Handles creation of command buffers.
 *
 * Handles creation of command buffers. See CommandBuffer.
 *
 * @bug     No known bugs.
 */
/*****************************************************************************/

/*****************************************************************************/
/**
 * Includes
 */
/*****************************************************************************/
#include "geCommandBufferManager.h"

namespace geEngineSDK {
  namespace geCoreThread {
    SPtr<CommandBuffer>
    CommandBufferManager::create(GPU_QUEUE_TYPE::E type,
                                 uint32 deviceIdx,
                                 uint32 queueIdx,
                                 bool secondary) {
      GE_ASSERT(GE_MAX_DEVICES > deviceIdx);
      return createInternal(type, deviceIdx, queueIdx, secondary);
    }
  }
}
