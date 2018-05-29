/*****************************************************************************/
/**
 * @file    geCoreThread.cpp
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

/*****************************************************************************/
/**
 * Includes
 */
/*****************************************************************************/
#include "geCoreThread.h"
#include "geCoreApplication.h"

#include <geThreadPool.h>
#include <geTaskScheduler.h>
#include <geFrameAlloc.h>
#include <geNumericLimits.h>

namespace geEngineSDK {
  using namespace std::placeholders;
  using std::bind;

  CoreThread::QueueData CoreThread::m_perThreadQueue;
  GE_THREADLOCAL CoreThread::ThreadQueueContainer* CoreThread::QueueData::current = nullptr;

  CoreThread::CoreThread()
    : m_activeFrameAlloc(0),
      m_coreThreadShutdown(false),
      m_coreThreadStarted(false),
      m_commandQueue(nullptr),
      m_maxCommandNotifyId(0) {
    for(auto& frameAlloc : m_frameAllocs) {
      frameAlloc = ge_new<FrameAlloc>();
      frameAlloc->setOwnerThread(GE_THREAD_CURRENT_ID); //Sim thread
    }

    m_simThreadId = GE_THREAD_CURRENT_ID;
    m_coreThreadId = m_simThreadId; //For now
    m_commandQueue = ge_new<CommandQueue<CommandQueueSync>>(GE_THREAD_CURRENT_ID);

    initCoreThread();
  }

  CoreThread::~CoreThread() {
    //TODO: What if something gets queued between the queued call to
    //destroy_internal and this!?
    shutdownCoreThread();
    {
      Lock lock(m_coreQueueMutex);
      for (auto& queue : m_allQueues) {
        ge_delete(queue);
      }
      m_allQueues.clear();
    }

    if (nullptr != m_commandQueue) {
      ge_delete(m_commandQueue);
      m_commandQueue = nullptr;
    }

    for (auto& frameAlloc : m_frameAllocs) {
      frameAlloc->setOwnerThread(GE_THREAD_CURRENT_ID); //Sim thread
      ge_delete(frameAlloc);
    }
  }

  void
  CoreThread::initCoreThread() {
# if !GE_FORCE_SINGLETHREADED_RENDERING
#   if GE_THREAD_SUPPORT
    m_coreThread = ThreadPool::instance().run("Core", bind(&CoreThread::runCoreThread, this));

    //Need to wait to unsure thread ID is correctly set before continuing
    Lock lock(m_threadStartedMutex);

    while (!m_coreThreadStarted) {
      m_coreThreadStartedCondition.wait(lock);
    }
#   else
    GE_EXCEPT(InternalErrorException,
              "Attempting to start a core thread but application isn't "
              "compiled with thread support.");
#   endif
# endif
  }

  void
  CoreThread::runCoreThread() {
# if !GE_FORCE_SINGLETHREADED_RENDERING
    //One less worker because we are reserving one core for this thread
    TaskScheduler::instance().removeWorker();
    {
      Lock lock(m_threadStartedMutex);
      m_coreThreadStarted = true;
      m_coreThreadId = GE_THREAD_CURRENT_ID;
    }

    m_coreThreadStartedCondition.notify_one();

    while (true) {
      //Wait until we get some ready commands
      Queue<QueuedCommand>* commands = nullptr;
      {
        Lock lock(m_commandQueueMutex);

        while (m_commandQueue->isEmpty()) {
          if (m_coreThreadShutdown) {
            TaskScheduler::instance().addWorker();
            return;
          }

          //Do something else while we wait, otherwise this core will be unused
          TaskScheduler::instance().addWorker();
          m_commandReadyCondition.wait(lock);
          TaskScheduler::instance().removeWorker();
        }
        commands = m_commandQueue->flush();
      }

      //Play commands
      m_commandQueue->playbackWithNotify(commands,
        bind(&CoreThread::commandCompletedNotify, this, _1));
    }
# endif
  }

  void
  CoreThread::shutdownCoreThread() {
# if !GE_FORCE_SINGLETHREADED_RENDERING
    {
      Lock lock(m_commandQueueMutex);
      m_coreThreadShutdown = true;
    }

    //Wake all threads. They will quit after they see the shutdown flag
    m_commandReadyCondition.notify_all();

    m_coreThreadId = GE_THREAD_CURRENT_ID;

    m_coreThread.blockUntilComplete();
# endif
  }

  SPtr<TCoreThreadQueue<CommandQueueNoSync>>
  CoreThread::getQueue() {
    if (nullptr == m_perThreadQueue.current) {
      SPtr<TCoreThreadQueue<CommandQueueNoSync>>
      newQueue = ge_shared_ptr_new
                  <TCoreThreadQueue<CommandQueueNoSync>>(GE_THREAD_CURRENT_ID);
      m_perThreadQueue.current = ge_new<ThreadQueueContainer>();
      m_perThreadQueue.current->queue = newQueue;
      m_perThreadQueue.current->isMain = GE_THREAD_CURRENT_ID == m_simThreadId;

      Lock lock(m_coreQueueMutex);
      m_allQueues.push_back(m_perThreadQueue.current);
    }

    return m_perThreadQueue.current->queue;
  }

  void
  CoreThread::submitAll(bool blockUntilComplete) {
    Vector<ThreadQueueContainer*> queueCopies;
    {
      Lock lock(m_coreQueueMutex);
      queueCopies = m_allQueues;
    }

    //Submit workers first
    ThreadQueueContainer* mainQueue = nullptr;
    for (auto& queue : queueCopies) {
      if (!queue->isMain) {
        queue->queue->submitToCoreThread(blockUntilComplete);
      }
      else {
        mainQueue = queue;
      }
    }

    //Then main
    if (nullptr != mainQueue) {
      mainQueue->queue->submitToCoreThread(blockUntilComplete);
    }
  }

  void
  CoreThread::submit(bool blockUntilComplete) {
    getQueue()->submitToCoreThread(blockUntilComplete);
  }

  AsyncOp
  CoreThread::queueReturnCommand(function<void(AsyncOp&)> commandCallback,
                                 CoreThreadQueueFlags flags) {
    GE_ASSERT(GE_THREAD_CURRENT_ID != getCoreThreadId() &&
              "Cannot queue commands on the core thread for the core thread");

    if (!flags.isSet(CTQF::kInternalQueue)) {
      return getQueue()->queueReturnCommand(commandCallback);
    }
    else {
      bool blockUntilComplete = flags.isSet(CTQF::kBlockUntilComplete);

      AsyncOp op;
      uint32 commandId = NumLimit::MAX_UINT32;
      {
        Lock lock(m_commandQueueMutex);
        if (blockUntilComplete) {
          commandId = m_maxCommandNotifyId++;
          op = m_commandQueue->queueReturn(commandCallback, true, commandId);
        }
        else {
          op = m_commandQueue->queueReturn(commandCallback);
        }
      }

      m_commandReadyCondition.notify_all();

      if (blockUntilComplete) {
        blockUntilCommandCompleted(commandId);
      }

      return op;
    }
  }

  void
  CoreThread::queueCommand(function<void()> commandCallback,
                           CoreThreadQueueFlags flags) {
    GE_ASSERT(GE_THREAD_CURRENT_ID != getCoreThreadId() &&
              "Cannot queue commands on the core thread for the core thread");

    if (!flags.isSet(CTQF::kInternalQueue)) {
      getQueue()->queueCommand(commandCallback);
    }
    else {
      bool blockUntilComplete = flags.isSet(CTQF::kBlockUntilComplete);

      uint32 commandId = NumLimit::MAX_UINT32;
      {
        Lock lock(m_commandQueueMutex);
        if (blockUntilComplete) {
          commandId = m_maxCommandNotifyId++;
          m_commandQueue->queue(commandCallback, true, commandId);
        }
        else {
          m_commandQueue->queue(commandCallback);
        }
      }

      m_commandReadyCondition.notify_all();

      if (blockUntilComplete) {
        blockUntilCommandCompleted(commandId);
      }
    }
  }

  void
  CoreThread::update() {
    for (auto& frameAlloc : m_frameAllocs) {
      frameAlloc->setOwnerThread(m_coreThreadId);
    }

    m_activeFrameAlloc = (m_activeFrameAlloc + 1) % 2;
    m_frameAllocs[m_activeFrameAlloc]->setOwnerThread(GE_THREAD_CURRENT_ID); // Sim thread
    m_frameAllocs[m_activeFrameAlloc]->clear();
  }

  FrameAlloc*
  CoreThread::getFrameAlloc() const {
    return m_frameAllocs[m_activeFrameAlloc];
  }

  void
  CoreThread::blockUntilCommandCompleted(uint32 commandId) {
# if !GE_FORCE_SINGLETHREADED_RENDERING
    Lock lock(m_commandNotifyMutex);

    while (true) {
      //Check if our command id is in the completed list
      auto iter = m_commandsCompleted.begin();
      for (; iter != m_commandsCompleted.end(); ++iter) {
        if (*iter == commandId) {
          break;
        }
      }

      if (iter != m_commandsCompleted.end()) {
        m_commandsCompleted.erase(iter);
        break;
      }

      m_commandCompleteCondition.wait(lock);
    }
# endif
  }

  void
  CoreThread::commandCompletedNotify(uint32 commandId) {
    {
      Lock lock(m_commandNotifyMutex);
      m_commandsCompleted.push_back(commandId);
    }
    m_commandCompleteCondition.notify_all();
  }

  CoreThread&
  g_coreThread() {
    return CoreThread::instance();
  }

  void
  throwIfNotCoreThread() {
# if !GE_FORCE_SINGLETHREADED_RENDERING
    if (GE_THREAD_CURRENT_ID != CoreThread::instance().getCoreThreadId())
      GE_EXCEPT(InternalErrorException,
                "This method can only be accessed from the core thread.");
# endif
  }

  void
  throwIfCoreThread() {
# if !GE_FORCE_SINGLETHREADED_RENDERING
    if (GE_THREAD_CURRENT_ID == CoreThread::instance().getCoreThreadId())
      GE_EXCEPT(InternalErrorException,
                "This method cannot be accessed from the core thread.");
# endif
  }
}
