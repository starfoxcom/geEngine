/*****************************************************************************/
/**
 * @file    geCommandQueue.cpp
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2017/11/01
 * @brief   Manages a list of commands that can be queued for later execution.
 *
 * Manages a list of commands that can be queued for later execution on the
 * core thread.
 *
 * @bug     No known bugs.
 */
/*****************************************************************************/

/*****************************************************************************/
/**
 * Includes
 */
/*****************************************************************************/
#include "geCommandQueue.h"
#include "geCoreThread.h"
#include <geException.h>
#include <geDebug.h>

namespace geEngineSDK {
# if GE_DEBUG_MODE
  CommandQueueBase::CommandQueueBase(ThreadId threadId)
    : m_myThreadId(threadId),
      m_maxDebugIdx(0) {
    m_asyncOpSyncData = ge_shared_ptr_new<AsyncOpSyncData>();
    m_commands = ge_new<geEngineSDK::Queue<QueuedCommand>>();
    {
      Lock lock(commandQueueBreakpointMutex);
      m_commandQueueIdx = maxCommandQueueIdx++;
    }
  }
# else
  CommandQueueBase::CommandQueueBase(ThreadId threadId)
    : m_myThreadId(threadId) {
    m_asyncOpSyncData = ge_shared_ptr_new<AsyncOpSyncData>();
    m_commands = ge_new<bs::Queue<QueuedCommand>>();
  }
# endif

  CommandQueueBase::~CommandQueueBase() {
    if (nullptr != m_commands) {
      ge_delete(m_commands);
    }

    while (!m_emptyCommandQueues.empty()) {
      ge_delete(m_emptyCommandQueues.top());
      m_emptyCommandQueues.pop();
    }
  }

  AsyncOp
  CommandQueueBase::queueReturn(std::function<void(AsyncOp&)> commandCallback,
                                bool _notifyWhenComplete,
                                uint32 _callbackId) {
# if GE_DEBUG_MODE
    breakIfNeeded(m_commandQueueIdx, m_maxDebugIdx);
    QueuedCommand newCommand(commandCallback,
                             m_maxDebugIdx++,
                             m_asyncOpSyncData,
                             _notifyWhenComplete,
                             _callbackId);
# else
    QueuedCommand newCommand(commandCallback,
                             m_asyncOpSyncData,
                             _notifyWhenComplete,
                             _callbackId);
# endif
    m_commands->push(newCommand);
# if GE_FORCE_SINGLETHREADED_RENDERING
    Queue<QueuedCommand>* commands = flush();
    playback(commands);
# endif
    return newCommand.asyncOp;
  }

  void
  CommandQueueBase::queue(std::function<void()> commandCallback,
                          bool _notifyWhenComplete,
                          uint32 _callbackId) {
# if GE_DEBUG_MODE
    breakIfNeeded(m_commandQueueIdx, m_maxDebugIdx);
    QueuedCommand newCommand(commandCallback,
                             m_maxDebugIdx++,
                             _notifyWhenComplete,
                             _callbackId);
# else
    QueuedCommand newCommand(commandCallback,
                             _notifyWhenComplete,
                             _callbackId);
# endif
    m_commands->push(newCommand);
# if GE_FORCE_SINGLETHREADED_RENDERING
    Queue<QueuedCommand>* commands = flush();
    playback(commands);
# endif
  }

  geEngineSDK::Queue<QueuedCommand>*
  CommandQueueBase::flush() {
    geEngineSDK::Queue<QueuedCommand>* oldCommands = m_commands;

    if (!m_emptyCommandQueues.empty()) {
      m_commands = m_emptyCommandQueues.top();
      m_emptyCommandQueues.pop();
    }
    else {
      m_commands = ge_new<geEngineSDK::Queue<QueuedCommand>>();
    }

    return oldCommands;
  }

  void
  CommandQueueBase::playbackWithNotify(geEngineSDK::Queue<QueuedCommand>* commands,
                                       std::function<void(uint32)> notifyCallback) {
    THROW_IF_NOT_CORE_THREAD;

    if (nullptr == commands) {
      return;
    }

    while (!commands->empty()) {
      QueuedCommand& command = commands->front();

      if (command.returnsValue) {
        AsyncOp& op = command.asyncOp;
        command.callbackWithReturnValue(op);

        if (!command.asyncOp.hasCompleted()) {
          LOGDBG("Async operation return value wasn't resolved properly. "
                 "Resolving automatically to nullptr. Make sure to complete "
                 "the operation before returning from the command callback "
                 "method.");
          command.asyncOp._completeOperation(nullptr);
        }
      }
      else {
        command.callback();
      }

      if (command.notifyWhenComplete && nullptr != notifyCallback) {
        notifyCallback(command.callbackId);
      }

      commands->pop();
    }

    m_emptyCommandQueues.push(commands);
  }

  void
  CommandQueueBase::playback(geEngineSDK::Queue<QueuedCommand>* commands) {
    playbackWithNotify(commands, std::function<void(uint32)>());
  }

  void
  CommandQueueBase::cancelAll() {
    geEngineSDK::Queue<QueuedCommand>* commands = flush();

    while (!commands->empty()) {
      commands->pop();
    }

    m_emptyCommandQueues.push(commands);
  }

  bool
  CommandQueueBase::isEmpty() {
    if (nullptr != m_commands && m_commands->size() > 0) {
      return false;
    }
    return true;
  }

  void
  CommandQueueBase::throwInvalidThreadException(const String& message) const {
    GE_EXCEPT(InternalErrorException, message);
  }

# if GE_DEBUG_MODE
  Mutex CommandQueueBase::commandQueueBreakpointMutex;
  uint32 CommandQueueBase::maxCommandQueueIdx = 0;

  UnorderedSet<CommandQueueBase::QueueBreakpoint,
               CommandQueueBase::QueueBreakpoint::HashFunction,
               CommandQueueBase::QueueBreakpoint::EqualFunction>
               CommandQueueBase::setBreakpoints;

  inline size_t
  CommandQueueBase::QueueBreakpoint::
  HashFunction::operator()(const QueueBreakpoint& v) const {
    size_t seed = 0;
    hash_combine(seed, v.queueIdx);
    hash_combine(seed, v.commandIdx);
    return seed;
  }

  inline bool
  CommandQueueBase::QueueBreakpoint::
  EqualFunction::operator()(const QueueBreakpoint &a, const QueueBreakpoint &b) const {
    return a.queueIdx == b.queueIdx && a.commandIdx == b.commandIdx;
  }

  void
  CommandQueueBase::addBreakpoint(uint32 queueIdx, uint32 commandIdx) {
    Lock lock(commandQueueBreakpointMutex);
    setBreakpoints.insert(QueueBreakpoint(queueIdx, commandIdx));
  }

  void
  CommandQueueBase::breakIfNeeded(uint32 queueIdx, uint32 commandIdx) {
    //I purposely don't use a mutex here, as this gets called very often.
    //Generally breakpoints will only be added at the start of the application,
    //so race conditions should not occur.
    auto iterFind = setBreakpoints.find(QueueBreakpoint(queueIdx, commandIdx));
    if (iterFind != setBreakpoints.end()) {
      GE_ASSERT(false && "Command queue breakpoint triggered!");
    }
  }
# else
  void
  CommandQueueBase::addBreakpoint(uint32 queueIdx, uint32 commandIdx) {
    //Do nothing, no breakpoints in release
  }
# endif
}
