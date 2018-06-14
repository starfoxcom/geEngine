/*****************************************************************************/
/**
 * @file    geCoreObject.cpp
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2017/11/29
 * @brief   Provides functionality for dealing with objects that need to exist
 *          on both simulation and core thread.
 *
 * Core objects provides functionality for dealing with objects that need to
 * exist on both simulation and core thread.
 * It handles cross-thread initialization, destruction as well as syncing data
 * between the two threads.
 *
 * It also provides a standardized way to initialize/destroy objects, and a way
 * to specify dependent CoreObject's. For those purposes it might also be used
 * for objects that only exist on the core thread.
 *
 * @note  geCoreThread::CoreObject is a counterpart to CoreObject that is used
 *        exclusively on the core thread. CoreObject on the other hand should
 *        be used exclusively on the simulation thread. Types that exist on
 *        both threads need to implement both of these.
 *
 * @bug     No known bugs.
 */
/*****************************************************************************/

/*****************************************************************************/
/**
 * Includes
 */
/*****************************************************************************/
#include "geCoreObject.h"
#include "geCoreObjectCore.h"
#include "geCoreThread.h"
#include "geCoreObjectManager.h"

namespace geEngineSDK {
  using namespace std::placeholders;
  using std::atomic_thread_fence;
  using std::memory_order_release;
  using std::function;
  using std::bind;

  CoreObject::CoreObject(bool initializeOnCoreThread)
    : m_flags(initializeOnCoreThread ? CGO_FLAGS::kInitOnCoreThread : 0),
      m_coreDirtyFlags(0),
      m_internalID(CoreObjectManager::instance().generateId())
  {}

  CoreObject::~CoreObject() {
    if (!isDestroyed()) {
      //Object must be released with destroy() otherwise engine can still try
      //to use it, even if it was destructed (e.g. if an object has one of its
      //methods queued in a command queue, and is destructed, you will be
      //accessing invalid memory)
      GE_EXCEPT(InternalErrorException,
                "Destructor called but object is not destroyed. This will "
                "result in nasty issues.");
    }

# if GE_DEBUG_MODE
    if (!m_this.expired()) {
      GE_EXCEPT(InternalErrorException,
                "Shared pointer to this object still has active references "
                "but the object is being deleted? You shouldn't delete "
                "CoreObjects manually.");
    }
# endif
  }

  void
  CoreObject::destroy() {
    CoreObjectManager::instance().unregisterObject(this);
    setIsDestroyed(true);

    if (requiresInitOnCoreThread()) {
      GE_ASSERT(GE_THREAD_CURRENT_ID != CoreThread::instance().getCoreThreadId() &&
                "Cannot destroy sim thead object from core thread.");

      //This will only destroy the geCoreThread::CoreObject if this was the
      //last reference
      queueDestroyGPUCommand(m_coreSpecific);
    }

    m_coreSpecific = nullptr;
  }

  void
  CoreObject::initialize() {
    m_coreSpecific = createCore();

    if (nullptr != m_coreSpecific) {
      if (requiresInitOnCoreThread()) {
        m_coreSpecific->setScheduledToBeInitialized(true);
        GE_ASSERT(GE_THREAD_CURRENT_ID != CoreThread::instance().getCoreThreadId() &&
                  "Cannot initialize sim thread object from core thread.");

        queueInitializeGPUCommand(m_coreSpecific);
      }
      else {
        m_coreSpecific->initialize();

        //Even though this object might not require initialization on the core
        //thread, it will be used on it, therefore do a memory barrier to
        //ensure any stores are finished before continuing (When it requires
        //init on core thread we use the core queue which uses a mutex, and
        //therefore executes all stores as well, so we don't need to do this
        //explicitly)
        
        //TODO: Need atomic variable, currently this does nothing
        atomic_thread_fence(memory_order_release);
      }
    }

    m_flags |= CGO_FLAGS::kInitialized;
    markDependenciesDirty();
  }

  void
  CoreObject::blockUntilCoreInitialized() const {
    if (nullptr != m_coreSpecific) {
      m_coreSpecific->synchronize();
    }
  }

  void
  CoreObject::syncToCore() {
    CoreObjectManager::instance().syncToCore(this);
  }

  void
  CoreObject::markCoreDirty(uint32 flags) {
    bool wasDirty = isCoreDirty();
    m_coreDirtyFlags |= flags;

    if (!wasDirty && isCoreDirty()) {
      CoreObjectManager::instance().notifyCoreDirty(this);
    }
  }

  void
  CoreObject::markDependenciesDirty() {
    CoreObjectManager::instance().notifyDependenciesDirty(this);
  }

  void
  CoreObject::_setThisPtr(SPtr<CoreObject> ptrThis) {
    m_this = ptrThis;
  }

  void
  CoreObject::queueGPUCommand(const SPtr<geCoreThread::CoreObject>& obj,
                              function<void()> func) {
    //We call another internal method and go through an additional layer of
    //abstraction in order to keep an active reference to the obj (saved in the
    //bound function).
    //We could have called the function directly using "this" pointer but then
    //we couldn't have used a shared_ptr for the object, in which case there is
    //a possibility that the object would be released and deleted while still
    //being in the command queue.
    g_coreThread().queueCommand(bind(&CoreObject::executeGPUCommand, obj, func));
  }

  AsyncOp
  CoreObject::queueReturnGPUCommand(const SPtr<geCoreThread::CoreObject>& obj,
                                    function<void(AsyncOp&)> func) {
    //See queueGPUCommand
    return g_coreThread().queueReturnCommand(
      bind(&CoreObject::executeReturnGPUCommand, obj, func, _1));
  }

  void
  CoreObject::queueInitializeGPUCommand(const SPtr<geCoreThread::CoreObject>& obj) {
    function<void()> func = bind(&geCoreThread::CoreObject::initialize, obj.get());
    CoreThread::instance().queueCommand(bind(&CoreObject::executeGPUCommand, obj, func),
                                        CTQF::kInternalQueue);
  }

  void
  CoreObject::queueDestroyGPUCommand(const SPtr<geCoreThread::CoreObject>& obj) {
    //Do nothing function. We just need the shared pointer to stay alive until
    //it reaches the core thread
    function<void()> func = [&](){};
    g_coreThread().queueCommand(bind(&CoreObject::executeGPUCommand, obj, func));
  }

  void
  CoreObject::executeGPUCommand(const SPtr<geCoreThread::CoreObject>& obj,
                                function<void()> func) {
    //Makes sure obj isn't optimized out?
    volatile const SPtr<geCoreThread::CoreObject>& objParam = obj;
    func();
  }

  void
  CoreObject::executeReturnGPUCommand(const SPtr<geCoreThread::CoreObject>& obj,
                                      function<void(AsyncOp&)> func,
                                      AsyncOp& op) {
    //Makes sure obj isn't optimized out?
    volatile const SPtr<geCoreThread::CoreObject>& objParam = obj;
    func(op);
  }
}
