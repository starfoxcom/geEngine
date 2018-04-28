/*****************************************************************************/
/**
 * @file    geResourceHandle.cpp
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2017/11/23
 * @brief   Represents a handle to a resource.
 *
 * Represents a handle to a resource. Handles are similar to smart pointers,
 * but they have two advantages:
 * - When loading a resource asynchronously you can be immediately returned the
 *   handle that you may use throughout the engine. The handle will be made
 *   valid as soon as the resource is loaded.
 * - Handles can be serialized and deserialized, therefore saving / restoring
 *   references to their original resource.
 *
 * @bug     No known bugs.
 */
/*****************************************************************************/

/*****************************************************************************/
/**
 * Includes
 */
/*****************************************************************************/
#include "gePrerequisitesCore.h"
#include "geResourceHandle.h"
#include "geResource.h"
#include "geResourceHandleRTTI.h"
#include "geResources.h"
#include "geResourceListenerManager.h"

namespace geEngineSDK {
  Signal ResourceHandleBase::m_resourceCreatedCondition;
  Mutex ResourceHandleBase::m_resourceCreatedMutex;

  bool
  ResourceHandleBase::isLoaded(bool checkDependencies) const {
    bool bIsLoaded = (nullptr != m_data &&
                      m_data->m_isCreated &&
                      nullptr != m_data->m_ptr);

    if (checkDependencies && bIsLoaded) {
      bIsLoaded = m_data->m_ptr->areDependenciesLoaded();
    }

    return bIsLoaded;
  }

  void
  ResourceHandleBase::blockUntilLoaded(bool waitForDependencies) const {
    if (nullptr == m_data) {
      return;
    }

    if (!m_data->m_isCreated) {
      Lock lock(m_resourceCreatedMutex);
      while (!m_data->m_isCreated) {
        m_resourceCreatedCondition.wait(lock);
      }

      //Send out ResourceListener events right away, as whatever called this
      //method probably also expects the listener events to trigger immediately
      //as well
      ResourceListenerManager::instance().notifyListeners(m_data->m_uuid);
    }

    if (waitForDependencies) {
      ge_frame_mark();
      {
        FrameVector<HResource> dependencies;
        m_data->m_ptr->getResourceDependencies(dependencies);

        for (auto& dependency : dependencies) {
          dependency.blockUntilLoaded(waitForDependencies);
        }
      }
      ge_frame_clear();
    }
  }

  void
  ResourceHandleBase::release() {
    g_resources().release(*this);
  }

  void
  ResourceHandleBase::destroy() {
    g_resources().destroy(*this);
  }

  void
  ResourceHandleBase::setHandleData(const SPtr<Resource>& ptr,
                                    const UUID& uuid) {
    m_data->m_ptr = ptr;

    if (m_data->m_ptr) {
      m_data->m_uuid = uuid;

      if (!m_data->m_isCreated) {
        Lock lock(m_resourceCreatedMutex);
        {
          m_data->m_isCreated = true;
        }
        m_resourceCreatedCondition.notify_all();
      }
    }
  }

  void
  ResourceHandleBase::clearHandleData() {
    m_data->m_ptr = nullptr;
    Lock lock(m_resourceCreatedMutex);
    m_data->m_isCreated = false;
  }

  void
  ResourceHandleBase::addInternalRef() {
    m_data->m_refCount.fetch_add(1, memory_order_relaxed);
  }

  void
  ResourceHandleBase::removeInternalRef() {
    m_data->m_refCount.fetch_sub(1, memory_order_relaxed);
  }

  void
  ResourceHandleBase::throwIfNotLoaded() const {
#if GE_DEBUG_MODE
    if (!isLoaded(false)) {
      GE_EXCEPT(InternalErrorException,
                "Trying to access a resource that hasn't been loaded yet.");
    }
#endif
  }

  RTTITypeBase*
  TResourceHandleBase<true>::getRTTIStatic() {
    return WeakResourceHandleRTTI::instance();
  }

  RTTITypeBase*
  TResourceHandleBase<true>::getRTTI() const {
    return getRTTIStatic();
  }

  RTTITypeBase*
  TResourceHandleBase<false>::getRTTIStatic() {
    return ResourceHandleRTTI::instance();
  }

  RTTITypeBase*
  TResourceHandleBase<false>::getRTTI() const {
    return getRTTIStatic();
  }
}
