/*****************************************************************************/
/**
 * @file    geCoreObjectCore.cpp
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2017/11/01
 * @brief   Counterpart of a CoreObject used specifically on the core thread.
 *
 * Core thread only.
 * Different CoreObject implementations should implement this class for their
 * own needs.
 *
 * @bug     No known bugs.
 */
/*****************************************************************************/

/*****************************************************************************/
/**
 * Includes
 */
/*****************************************************************************/
#include "geCoreObjectCore.h"
#include "geCoreThread.h"

namespace geEngineSDK {
  namespace geCoreThread {
    Signal CoreObject::m_coreGPUObjectLoadedCondition;
    Mutex CoreObject::m_coreGPUObjectLoadedMutex;

    CoreObject::CoreObject() : m_flags(0) {}

    CoreObject::~CoreObject() {
      THROW_IF_NOT_CORE_THREAD;
    }

    void
    CoreObject::initialize() {
      {
        Lock lock(m_coreGPUObjectLoadedMutex);
        setIsInitialized(true);
      }

      setScheduledToBeInitialized(false);
      m_coreGPUObjectLoadedCondition.notify_all();
    }

    void
    CoreObject::synchronize() {
      if (!isInitialized()) {
# if GE_DEBUG_MODE
        if (GE_THREAD_CURRENT_ID == CoreThread::instance().getCoreThreadId()) {
          GE_EXCEPT(InternalErrorException,
                    "You cannot call this method on the core thread."
                    "It will cause a deadlock!");
        }
# endif
        Lock lock(m_coreGPUObjectLoadedMutex);
        while (!isInitialized()) {
          if (!isScheduledToBeInitialized()) {
            GE_EXCEPT(InternalErrorException,
                      "Attempting to wait until initialization finishes but "
                      "object is not scheduled to be initialized.");
          }
          m_coreGPUObjectLoadedCondition.wait(lock);
        }
      }
    }

    void
    CoreObject::_setThisPtr(SPtr<CoreObject> ptrThis) {
      m_this = ptrThis;
    }
  }
}
