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
    Signal CoreObject::mCoreGpuObjectLoadedCondition;
    Mutex CoreObject::mCoreGpuObjectLoadedMutex;

    CoreObject::CoreObject() : mFlags(0) {}

    CoreObject::~CoreObject() {
      THROW_IF_NOT_CORE_THREAD;
    }

    void
    CoreObject::initialize() {
      {
        Lock lock(mCoreGpuObjectLoadedMutex);
        setIsInitialized(true);
      }

      setScheduledToBeInitialized(false);
      mCoreGpuObjectLoadedCondition.notify_all();
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
        Lock lock(mCoreGpuObjectLoadedMutex);
        while (!isInitialized()) {
          if (!isScheduledToBeInitialized()) {
            GE_EXCEPT(InternalErrorException,
                      "Attempting to wait until initialization finishes but "
                      "object is not scheduled to be initialized.");
          }
          mCoreGpuObjectLoadedCondition.wait(lock);
        }
      }
    }

    void
    CoreObject::_setThisPtr(SPtr<CoreObject> ptrThis) {
      mThis = ptrThis;
    }
  }
}
