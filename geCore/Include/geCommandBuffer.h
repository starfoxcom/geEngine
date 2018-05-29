/*****************************************************************************/
/**
 * @file    geCommandBuffer.h
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
#pragma once

/*****************************************************************************/
/**
 * Includes
 */
/*****************************************************************************/
#include "gePrerequisitesCore.h"

namespace geEngineSDK {
  namespace geCoreThread {
    /**
     * @brief Mask that determines synchronization between command buffers
     *        executing on different hardware queues.
     */
    class GE_CORE_EXPORT CommandSyncMask
    {
     public:
      /**
       * @brief Registers a dependency on a command buffer. Use getMask() to
       *        get the new mask value after registering all dependencies.
       */
      void
      addDependency(const SPtr<CommandBuffer>& buffer);

      /**
       * @brief Returns a combined mask that contains all the required
       *        dependencies.
       */
      uint32
      getMask() const {
        return m_mask;
      }

      /**
       * @brief Uses the queue type and index to generate a mask with a bit set
       *        for that queue's global index.
       */
      static uint32
      getGlobalQueueMask(GPU_QUEUE_TYPE::E type, uint32 queueIdx);

      /**
       * @brief Uses the queue type and index to generate a global queue index.
       */
      static uint32
      getGlobalQueueIdx(GPU_QUEUE_TYPE::E type, uint32 queueIdx);

      /**
       * @brief Uses the global queue index to retrieve local queue index and
       *        queue type.
       */
      static uint32
      getQueueIdxAndType(uint32 globalQueueIdx, GPU_QUEUE_TYPE::E& type);

     private:
      uint32 m_mask = 0;
    };

    class GE_CORE_EXPORT CommandBuffer
    {
     public:
      virtual ~CommandBuffer() = default;

      /**
       * @brief Creates a new CommandBuffer.
       * @param[in] type      Determines what type of commands can be added to
       *                      the command buffer.
       * @param[in] deviceIdx Index of the GPU the command buffer will be used
       *                      to queue commands on. 0 is always the primary
       *                      available GPU.
       * @param[in] queueIdx  Index of the GPU queue the command buffer will be
       *                      used on. Command buffers with the same index will
       *                      execute sequentially, but command buffers with
       *                      different queue indices may execute in parallel,
       *                      for a potential performance improvement.
       *
       * Caller must ensure to synchronize operations executing on different
       * queues via sync masks. Command buffer dependant on another command
       * buffer should provide a sync mask when being submitted
       * (see RenderAPI::executeCommands).
       *
       * Queue indices are unique per buffer type (e.g. upload index 0 and
       * graphics index 0 may map to different queues internally).
       * Must be in range [0, 7].
       *
       * @param[in] secondary If true the command buffer will not be allowed to
       *                      execute on its own, but it can be appended to a
       *                      primary command buffer.
       *
       * @return  New CommandBuffer instance.
       */
      static SPtr<CommandBuffer>
      create(GPU_QUEUE_TYPE::E type,
             uint32 deviceIdx = 0,
             uint32 queueIdx = 0,
             bool secondary = false);

      /**
       * @brief Returns the type of queue the command buffer will execute on.
       */
      GPU_QUEUE_TYPE::E
      getType() const {
        return m_type;
      }

      /**
       * @brief Returns the index of the queue the command buffer will execute
       *        on.
       */
      uint32
      getQueueIdx() const {
        return m_queueIdx;
      }

      /**
       * @brief Returns the device index this buffer will execute on.
       */
      uint32
      getDeviceIdx() const {
        return m_deviceIdx;
      }

     protected:
      CommandBuffer(GPU_QUEUE_TYPE::E type,
                    uint32 deviceIdx,
                    uint32 queueIdx,
                    bool secondary);

      GPU_QUEUE_TYPE::E m_type;
      uint32 m_deviceIdx;
      uint32 m_queueIdx;
      bool m_isSecondary;
    };
  }
}
