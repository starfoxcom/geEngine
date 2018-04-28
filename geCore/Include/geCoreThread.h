/*****************************************************************************/
/**
 * @file    geCoreThread.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2017/11/29
 * @brief   Manager for the core thread.
 *
 * Manager for the core thread. Takes care of starting, running, queuing
 * commands and shutting down the core thread.
 *
 * How threading works:
 * - Commands from various threads can be queued for execution on the core
 *   thread by calling queueCommand() or queueReturnCommand().
 * - Internally each thread maintains its own separate queue of commands, so
 *   you cannot interleave commands from different threads.
 * - There is also the internal command queue, which is the only queue directly
 *   visible from the core thread.
 * - Core thread continually polls the internal command queue for new commands,
 *   and executes them in order they were submitted.
 * - Commands queued on the per-thread queues are submitted to the internal
 *   command queue by calling submit(), at which point they are made visible to
 *   the core thread, and will begin executing.
 * - Commands can also be submitted directly to the internal command queue
 *   (via a special flag), but with a performance cost due to extra
 *   synchronization required.
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
#include "geCoreThreadQueue.h"
#include <geModule.h>
#include <geThreadPool.h>

namespace geEngineSDK {
  using std::function;

  /**
   * @brief Flags that control how is a command submitted to the command queue.
   */
  namespace CORE_THREAD_QUEUE_FLAGS {
    enum E {
      /**
       * Default flag, meaning the commands will be added to the per-thread queue
       * and only begin executing after submit() has been called.
       */
      kDefault = 0,

      /**
       * Specifies that the queued command should be executed on the internal
       * queue. Internal queue doesn't require a separate CoreThread::submit()
       * call, and the queued command is instead immediately visible to the core
       * thread. The downside is that the queue requires additional
       * synchronization and is slower than the normal queue.
       */
      kInternalQueue = 1 << 0,

      /**
       * If true, the method will block until the command finishes executing on
       * the core thread. Only relevant for the internal queue commands since
       * contents of the normal queue won't be submitted to the core thread until
       * the CoreThread::submit() call.
       */
      kBlockUntilComplete = 1 << 1
    };
  }

  using CTQF = CORE_THREAD_QUEUE_FLAGS::E;
  typedef Flags<CTQF> CoreThreadQueueFlags;
  GE_FLAGS_OPERATORS(CTQF)

  class GE_CORE_EXPORT CoreThread : public Module<CoreThread>
  {
    /**
     * @brief Contains data about a queue for an specific thread.
     */
    struct ThreadQueueContainer
    {
      SPtr<TCoreThreadQueue<CommandQueueNoSync>> queue;
      bool isMain;
    };

    /**
     * @brief Wrapper for the thread-local variable because MSVC can't deal
     *        with a thread-local variable marked with dllimport or dllexport,
     *        and we cannot use per-member dllimport/dllexport specifiers
     *        because Module's members will then not be exported and its static
     *        members will not have external linkage.
     */
    struct QueueData
    {
      static GE_THREADLOCAL ThreadQueueContainer* current;
    };

   public:
    CoreThread();
    ~CoreThread();

    /**
     * @brief Returns the id of the core thread.
     */
    ThreadId
    getCoreThreadId() {
      return m_coreThreadId;
    }

    /**
     * @brief Submits the commands from all queues and starts executing them on
     *        the core thread.
     */
    void
    submitAll(bool blockUntilComplete = false);

    /**
     * @brief Submits the commands from the current thread's queue and starts
     *        executing them on the core thread.
     */
    void
    submit(bool blockUntilComplete = false);

    /**
     * @brief Queues a new command that will be added to the command queue.
     *        Command returns a value.
     * @param[in] commandCallback Command to queue.
     * @param[in] flags Flags that further control command submission.
     * @return  Structure that can be used to check if the command completed
     *          execution, and to retrieve the return value once it has.
     * @see   CommandQueue::queueReturn()
     * @note  Thread safe
     */
    AsyncOp
    queueReturnCommand(function<void(AsyncOp&)> commandCallback,
                       CoreThreadQueueFlags flags = CTQF::kDefault);

    /**
     * @brief Queues a new command that will be added to the global command
     *        queue.
     * @param[in] commandCallback Command to queue.
     * @param[in] flags Flags that further control command submission.
     * @see CommandQueue::queue()
     * @note  Thread safe
     */
    void
    queueCommand(function<void()> commandCallback,
                 CoreThreadQueueFlags flags = CTQF::kDefault);

    /**
     * @brief Called once every frame.
     * @note  Must be called before sim thread schedules any core thread
     *        operations for the frame.
     */
    void
    update();

    /**
     * @brief Returns a frame allocator that should be used for allocating
     *        temporary data being passed to the core thread. As the name
     *        implies the data only lasts one frame, so you need to be careful
     *        not to use it for longer than that.
     * @note  Sim thread only.
     */
    FrameAlloc*
    getFrameAlloc() const;

    /**
     * @brief Returns number of buffers needed to sync data between core and
     *        sim thread. Currently the sim thread can be one frame ahead of
     *        the core thread, meaning we need two buffers. If this situation
     *        changes increase this number.
     * For example:
     * - Sim thread frame starts, it writes some data to buffer 0.
     * - Core thread frame starts, it reads some data from buffer 0.
     * - Sim thread frame finishes
     * - New sim thread frame starts, it writes some data to buffer 1.
     * - Core thread still working, reading from buffer 0. (If we were using
     *   just one buffer at this point core thread would be reading wrong data)
     * - Sim thread waiting for core thread (application defined that it cannot
     *   go ahead more than one frame)
     * - Core thread frame finishes.
     * - New core thread frame starts, it reads some data from buffer 1.
     * - ...
     */
    static const uint32 NUM_SYNC_BUFFERS = 2;

   private:
    /**
     * @brief Double buffered frame allocators. Means sim thread cannot be more
     *        than 1 frame ahead of core thread (If that changes you should be 
     *        able to easily add more).
     */
    FrameAlloc* m_frameAllocs[NUM_SYNC_BUFFERS];
    uint32 m_activeFrameAlloc;

    static QueueData m_perThreadQueue;
    Vector<ThreadQueueContainer*> m_allQueues;

    volatile bool m_coreThreadShutdown;

    HThread m_coreThread;
    bool m_coreThreadStarted;
    ThreadId m_simThreadId;
    ThreadId m_coreThreadId;
    Mutex m_commandQueueMutex;
    Mutex m_coreQueueMutex;
    Signal m_commandReadyCondition;
    Mutex m_commandNotifyMutex;
    Signal m_commandCompleteCondition;
    Mutex m_threadStartedMutex;
    Signal m_coreThreadStartedCondition;

    CommandQueue<CommandQueueSync>* m_commandQueue;

    /**
     * ID that will be assigned to the next command with a notifier callback.
     */
    uint32 m_maxCommandNotifyId;
    
    /**
     * Completed commands that have notifier callbacks set up
     */
    Vector<uint32> m_commandsCompleted;

    /**
     * @brief Starts the core thread worker method. Should only be called once.
     */
    void
    initCoreThread();

    /**
     * @brief Main worker method of the core thread.
     *        Called once thread is started.
     */
    void
    runCoreThread();

    /**
     * @brief Shutdowns the core thread. It will complete all ready commands
     *        before shutdown.
     */
    void
    shutdownCoreThread();

    /**
     * @brief Creates or retrieves a queue for the calling thread.
     */
    SPtr<TCoreThreadQueue<CommandQueueNoSync>>
    getQueue();

    /**
     * @brief Blocks the calling thread until the command with the specified ID
     *        completes. Make sure that the specified ID actually exists,
     *        otherwise this will block forever.
     */
    void
    blockUntilCommandCompleted(uint32 commandId);

    /**
     * @brief Callback called by the command list when a specific command
     *        finishes executing. This is only called on commands that have a
     *        special notify on complete flag set.
     * @param[in]  commandId	Identifier for the command.
     */
    void
    commandCompletedNotify(uint32 commandId);
  };

  /**
   * @brief Returns the core thread manager used for dealing with the core
   *        thread from external threads.
   * @see CoreThread
   */
  GE_CORE_EXPORT CoreThread&
  g_coreThread();

  /**
   * @brief Throws an exception if current thread isn't the core thread.
   */
  GE_CORE_EXPORT void
  throwIfNotCoreThread();

  /**
   * @brief Throws an exception if current thread is the core thread.
   */
  GE_CORE_EXPORT void
  throwIfCoreThread();

# if GE_DEBUG_MODE
#   define THROW_IF_NOT_CORE_THREAD throwIfNotCoreThread();
#   define THROW_IF_CORE_THREAD throwIfCoreThread();
# else
#   define THROW_IF_NOT_CORE_THREAD 
#   define THROW_IF_CORE_THREAD
# endif
}
