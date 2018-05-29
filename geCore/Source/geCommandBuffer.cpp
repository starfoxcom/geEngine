/*****************************************************************************/
/**
 * @file    geCommandBuffer.cpp
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2018/05/29
 * @brief   A list of render commands that can be queued for execution on GPU.
 *
 * Contains a list of render API commands that can be queued for execution on
 * the GPU. User is allowed to populate the command buffer from any thread,
 * ensuring render API command generation can be multi-threaded. Command
 * buffers must always be created on the core thread. Same command buffer
 * cannot be used on multiple threads simultaneously without external
 * synchronization.
 *
 * @bug     No known bugs.
 */
/*****************************************************************************/

/*****************************************************************************/
/**
 * Includes
 */
/*****************************************************************************/
#include "geCommandBuffer.h"
#include "geCommandBufferManager.h"

namespace geEngineSDK {
  namespace geCoreThread {
    void
    CommandSyncMask::addDependency(const SPtr<CommandBuffer>& buffer) {
      if (nullptr == buffer) {
        return;
      }
      m_mask |= getGlobalQueueMask(buffer->getType(), buffer->getQueueIdx());
    }

    uint32
    CommandSyncMask::getGlobalQueueMask(GPU_QUEUE_TYPE::E type,
                                        uint32 queueIdx) {
      uint32 bitShift = 0;
      switch (type)
      {
        case GPU_QUEUE_TYPE::kGRAPHICS:
          break;
        case GPU_QUEUE_TYPE::kCOMPUTE:
          bitShift = 8;
          break;
        case GPU_QUEUE_TYPE::kUPLOAD:
          bitShift = 16;
          break;
        default: break;
      }
      return (1 << queueIdx) << bitShift;
    }

    uint32
    CommandSyncMask::getGlobalQueueIdx(GPU_QUEUE_TYPE::E type,
                                       uint32 queueIdx) {
      switch (type)
      {
        case GPU_QUEUE_TYPE::kCOMPUTE:
          return 8 + queueIdx;
        case GPU_QUEUE_TYPE::kUPLOAD:
          return 16 + queueIdx;
        default:
          return queueIdx;
      }
    }

    uint32
    CommandSyncMask::getQueueIdxAndType(uint32 globalQueueIdx,
                                        GPU_QUEUE_TYPE::E& type) {
      if (16 <= globalQueueIdx) {
        type = GPU_QUEUE_TYPE::kUPLOAD;
        return globalQueueIdx - 16;
      }

      if (8 <= globalQueueIdx)
      {
        type = GPU_QUEUE_TYPE::kCOMPUTE;
        return globalQueueIdx - 8;
      }

      type = GPU_QUEUE_TYPE::kGRAPHICS;
      return globalQueueIdx;
    }

    CommandBuffer::CommandBuffer(GPU_QUEUE_TYPE::E type,
                                 uint32 deviceIdx,
                                 uint32 queueIdx,
                                 bool secondary)
      : m_type(type),
        m_deviceIdx(deviceIdx),
        m_queueIdx(queueIdx),
        m_isSecondary(secondary)
    {}

    SPtr<CommandBuffer>
    CommandBuffer::create(GPU_QUEUE_TYPE::E type,
                          uint32 deviceIdx,
                          uint32 queueIdx,
                          bool secondary) {
      return CommandBufferManager::instance().create(type,
                                                     deviceIdx,
                                                     queueIdx,
                                                     secondary);
    }
  }
}
