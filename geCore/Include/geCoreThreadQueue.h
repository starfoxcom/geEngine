/*****************************************************************************/
/**
 * @file    geCoreThreadQueue.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2017/11/29
 * @brief   Contains base functionality used for CoreThreadQueue.
 *
 * Contains base functionality used for CoreThreadQueue.
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
#include "geCommandQueue.h"
#include <geAsyncOp.h>

namespace geEngineSDK {
  using std::function;

  class GE_CORE_EXPORT CoreThreadQueueBase
  {
   public:
    CoreThreadQueueBase(CommandQueueBase* commandQueue);
    virtual ~CoreThreadQueueBase();

    /**
     * @brief Queues a new generic command that will be added to the command
     *        queue. Returns an async operation object that you may use to
     *        check if the operation has finished, and to retrieve the return
     *        value once finished.
     */
    AsyncOp
    queueReturnCommand(function<void(AsyncOp&)> commandCallback);

    /**
     * @brief Queues a new generic command that will be added to the command
     *        queue.
     */
    void
    queueCommand(function<void()> commandCallback);

    /**
     * @brief Makes all the currently queued commands available to the core
     *        thread. They will be executed as soon as the core thread is
     *        ready. All queued commands are removed from the queue.
     * @param[in] blockUntilComplete  If true, the calling thread will block
     *            until the core thread finished executing all currently queued
     *            commands. This is usually very expensive and should only be
     *            used in non-performance critical code.
     */
    void
    submitToCoreThread(bool blockUntilComplete = false);

    /**
     * @brief Cancels all commands in the queue.
     */
    void
    cancelAll();

   private:
    CommandQueueBase* m_commandQueue;
  };

  /**
   * @brief Queue that allows the calling thread to queue commands for execution on the core thread. Commands will only be executed after they have been submitted to the core thread.
   * @note  Queued commands are only executed after the call to submitToCoreThread(), in the order they were submitted.
   */
  template<class CommandQueueSyncPolicy = CommandQueueNoSync>
  class GE_CORE_EXPORT TCoreThreadQueue : public CoreThreadQueueBase
  {
   public:
    /**
     * @brief Constructor.
     * @param[in] threadId  Identifier for the thread that created the queue.
     */
    TCoreThreadQueue(ThreadId threadId)
      : CoreThreadQueueBase(ge_new<CommandQueue<CommandQueueSyncPolicy>>(threadId))
    {}
  };
}
