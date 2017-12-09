/*****************************************************************************/
/**
 * @file    geResourceListenerManager.cpp
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2017/11/30
 * @brief   Handles all active implementations of IResourceListener interface
 *          and notifies them when events they're listening to occur.
 *
 * Handles all active implementations of IResourceListener interface and
 * notifies them when events they're listening to occur.
 *
 * @bug     No known bugs.
 */
/*****************************************************************************/

/*****************************************************************************/
/**
 * Includes
 */
/*****************************************************************************/
#include "geResourceListenerManager.h"
#include "geResources.h"
#include "geIResourceListener.h"

namespace geEngineSDK {
  using namespace std::placeholders;
  using std::bind;
  using std::find;

  ResourceListenerManager::ResourceListenerManager() {
    m_resourceLoadedConn = g_resources().onResourceLoaded.connect(
      bind(&ResourceListenerManager::onResourceLoaded, this, _1));
    m_resourceModifiedConn = g_resources().onResourceModified.connect(
      bind(&ResourceListenerManager::onResourceModified, this, _1));
  }

  ResourceListenerManager::~ResourceListenerManager() {
    GE_ASSERT(m_resourceToListenerMap.size() == 0 &&
              "Not all resource listeners had their resources unregistered "
              "properly.");

    m_resourceLoadedConn.disconnect();
    m_resourceModifiedConn.disconnect();
  }

  void
  ResourceListenerManager::registerListener(IResourceListener* listener) {
# if GE_DEBUG_MODE
    RecursiveLock lock(m_mutex);
    m_activeListeners.insert(listener);
# else
    GE_UNREFERENCED_PARAMETER(listener);
# endif
  }

  void
  ResourceListenerManager::unregisterListener(IResourceListener* listener) {
# if GE_DEBUG_MODE
    {
      RecursiveLock lock(m_mutex);
      m_activeListeners.erase(listener);
    }
# endif
    m_dirtyListeners.erase(listener);
    clearDependencies(listener);
  }

  void
  ResourceListenerManager::markListenerDirty(IResourceListener* listener) {
    m_dirtyListeners.insert(listener);
  }

  void
  ResourceListenerManager::update() {
    for (auto& listener : m_dirtyListeners) {
      clearDependencies(listener);
      addDependencies(listener);
    }

    m_dirtyListeners.clear();
    {
      RecursiveLock lock(m_mutex);

      for (auto& entry : m_loadedResources) {
        sendResourceLoaded(entry.second);
      }

      for (auto& entry : m_modifiedResources) {
        sendResourceModified(entry.second);
      }

      m_loadedResources.clear();
      m_modifiedResources.clear();
    }
  }

  void
  ResourceListenerManager::notifyListeners(const UUID& resourceUUID) {
    RecursiveLock lock(m_mutex);

    auto iterFindLoaded = m_loadedResources.find(resourceUUID);
    if (iterFindLoaded != m_loadedResources.end()) {
      sendResourceLoaded(iterFindLoaded->second);
      m_loadedResources.erase(iterFindLoaded);
    }

    auto iterFindModified = m_modifiedResources.find(resourceUUID);
    if (iterFindModified != m_modifiedResources.end()) {
      sendResourceModified(iterFindModified->second);
      m_modifiedResources.erase(iterFindModified);
    }
  }

  void
  ResourceListenerManager::onResourceLoaded(const HResource& resource) {
    RecursiveLock lock(m_mutex);
    m_loadedResources[resource.getUUID()] = resource;
  }

  void
  ResourceListenerManager::onResourceModified(const HResource& resource) {
    RecursiveLock lock(m_mutex);
    m_modifiedResources[resource.getUUID()] = resource;
  }

  void
  ResourceListenerManager::sendResourceLoaded(const HResource& resource) {
    uint64 handleId = (uint64)resource.getHandleData().get();

    auto iterFind = m_resourceToListenerMap.find(handleId);
    if (iterFind == m_resourceToListenerMap.end()) {
      return;
    }

    const Vector<IResourceListener*> relevantListeners = iterFind->second;
    for (auto& listener : relevantListeners) {
# if GE_DEBUG_MODE
      GE_ASSERT(m_activeListeners.find(listener) != m_activeListeners.end() &&
                "Attempting to notify a destroyed IResourceListener");
# endif
      listener->notifyResourceLoaded(resource);
    }
  }

  void
  ResourceListenerManager::sendResourceModified(const HResource& resource) {
    uint64 handleId = (uint64)resource.getHandleData().get();

    auto iterFind = m_resourceToListenerMap.find(handleId);
    if (iterFind == m_resourceToListenerMap.end()) {
      return;
    }

    const Vector<IResourceListener*> relevantListeners = iterFind->second;
    for (auto& listener : relevantListeners) {
# if GE_DEBUG_MODE
      GE_ASSERT(m_activeListeners.find(listener) != m_activeListeners.end() &&
                "Attempting to notify a destroyed IResourceListener");
# endif
      listener->notifyResourceChanged(resource);
    }
  }

  void
  ResourceListenerManager::clearDependencies(IResourceListener* listener) {
    auto iterFind = m_listenerToResourceMap.find(listener);
    if (iterFind == m_listenerToResourceMap.end()) {
      return;
    }

    const Vector<uint64>& dependantResources = iterFind->second;
    for (auto& resourceHandleId : dependantResources) {
      auto iterFind2 = m_resourceToListenerMap.find(resourceHandleId);
      if (iterFind2 != m_resourceToListenerMap.end()) {
        Vector<IResourceListener*>& listeners = iterFind2->second;
        auto iterFind3 = find(listeners.begin(), listeners.end(), listener);

        if (iterFind3 != listeners.end()) {
          listeners.erase(iterFind3);
        }

        if (listeners.size() == 0) {
          m_resourceToListenerMap.erase(iterFind2);
        }
      }
    }

    m_listenerToResourceMap.erase(iterFind);
  }

  void
  ResourceListenerManager::addDependencies(IResourceListener* listener) {
    listener->getListenerResources(m_tempResourceBuffer);

    if (m_tempResourceBuffer.size() > 0) {
      Vector<uint64> resourceHandleIds(m_tempResourceBuffer.size());
      uint32 idx = 0;
      for (auto& resource : m_tempResourceBuffer) {
        uint64 handleId = (uint64)resource.getHandleData().get();
        resourceHandleIds[idx] = handleId;
        m_resourceToListenerMap[handleId].push_back(listener);
        idx++;
      }

      m_listenerToResourceMap[listener] = resourceHandleIds;
    }

    m_tempResourceBuffer.clear();
  }
}
