/*****************************************************************************/
/**
 * @file    geCoreThreadQueue.cpp
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2017/11/29
 * @brief   Contains base functionality used for CoreThreadQueue.
 *
 * Contains base functionality used for CoreThreadQueue.
 *
 * @bug     No known bugs.
 */
/*****************************************************************************/

/*****************************************************************************/
/**
 * Includes
 */
/*****************************************************************************/
#include "geCoreThreadQueue.h"
#include "geCommandQueue.h"
//#include "geMaterial.h"
#include "geCoreThread.h"

namespace geEngineSDK {
  using std::bind;

  CoreThreadQueueBase::CoreThreadQueueBase(CommandQueueBase* commandQueue)
    : m_commandQueue(commandQueue)
  {}

  CoreThreadQueueBase::~CoreThreadQueueBase() {
    ge_delete(m_commandQueue);
  }

  AsyncOp
  CoreThreadQueueBase::queueReturnCommand(function<void(AsyncOp&)> commandCallback) {
    return m_commandQueue->queueReturn(commandCallback);
  }

  void
  CoreThreadQueueBase::queueCommand(function<void()> commandCallback) {
    m_commandQueue->queue(commandCallback);
  }

  void
  CoreThreadQueueBase::submitToCoreThread(bool /*blockUntilComplete*/) {
    Queue<QueuedCommand>* commands = m_commandQueue->flush();

    g_coreThread().queueCommand(bind(&CommandQueueBase::playback,
                                     m_commandQueue,
                                     commands),
                                CTQF::kInternalQueue | CTQF::kBlockUntilComplete);
  }

  void
  CoreThreadQueueBase::cancelAll() {
    //Note that this won't free any Frame data allocated for all the canceled
    //commands since frame data will only get cleared at frame start
    m_commandQueue->cancelAll();
  }
}
