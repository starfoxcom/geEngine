/*****************************************************************************/
/**
 * @file    geGameObjectManager.cpp
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2017/12/06
 * @brief   Tracks GameObject creation and destructions.
 *
 * Tracks GameObject creation and destructions. Also resolves GameObject
 * references from GameObject handles.
 *
 * @bug     No known bugs.
 */
/*****************************************************************************/

/*****************************************************************************/
/**
 * Includes
 */
/*****************************************************************************/
#include "geGameObjectManager.h"
#include "geGameObject.h"

namespace geEngineSDK {
  GameObjectManager::~GameObjectManager() {
    destroyQueuedObjects();
  }

  GameObjectHandleBase
  GameObjectManager::getObject(uint64 id) const {
    auto iterFind = m_objects.find(id);

    if (m_objects.end() != iterFind) {
      return iterFind->second;
    }

    return nullptr;
  }

  bool
  GameObjectManager::tryGetObject(uint64 id, GameObjectHandleBase& object) const {
    auto iterFind = m_objects.find(id);

    if (m_objects.end() != iterFind) {
      object = iterFind->second;
      return true;
    }

    return false;
  }

  bool
  GameObjectManager::objectExists(uint64 id) const {
    return m_objects.find(id) != m_objects.end();
  }

  void
  GameObjectManager::remapId(uint64 oldId, uint64 newId) {
    if (oldId == newId) {
      return;
    }

    m_objects[newId] = m_objects[oldId];
    m_objects.erase(oldId);
  }

  void
  GameObjectManager::queueForDestroy(const GameObjectHandleBase& object) {
    if (object.isDestroyed()) {
      return;
    }

    uint64 instanceId = object->getInstanceId();
    m_queuedForDestroy[instanceId] = object;
  }

  void
  GameObjectManager::destroyQueuedObjects() {
    for (auto& objPair : m_queuedForDestroy) {
      objPair.second->destroyInternal(objPair.second, true);
    }

    m_queuedForDestroy.clear();
  }

  GameObjectHandleBase
  GameObjectManager::registerObject(const SPtr<GameObject>& object, uint64 originalId) {
    object->initialize(object, m_nextAvailableID);

    //If deserialization is active we must ensure all handles pointing to the
    //same object share GameObjectHandleData, so check if any handles
    //referencing this object have been created. See ::registerUnresolvedHandle
    //for further explanation.
    if (m_isDeserializationActive) {
      GE_ASSERT(0 != originalId &&
                "You must provide an original ID when registering a "
                "deserialized game object.");

      auto iterFind = m_unresolvedHandleData.find(originalId);
      if (m_unresolvedHandleData.end() != iterFind) {
        GameObjectHandleBase handle;
        handle.m_data = iterFind->second;
        handle._setHandleData(object);

        m_objects[m_nextAvailableID] = handle;
        m_idMapping[originalId] = m_nextAvailableID;
        ++m_nextAvailableID;

        return handle;
      }
      else {
        GameObjectHandleBase handle(object);

        m_objects[m_nextAvailableID] = handle;
        m_idMapping[originalId] = m_nextAvailableID;
        ++m_nextAvailableID;

        return handle;
      }
    }

    GameObjectHandleBase handle(object);
    m_objects[m_nextAvailableID] = handle;
    m_nextAvailableID++;

    return handle;
  }

  void
  GameObjectManager::unregisterObject(GameObjectHandleBase& object) {
    m_objects.erase(object->getInstanceId());
    onDestroyed(object);
    object.destroy();
  }

  void
  GameObjectManager::startDeserialization() {
    GE_ASSERT(!m_isDeserializationActive);
    m_isDeserializationActive = true;
  }

  void
  GameObjectManager::endDeserialization() {
    GE_ASSERT(m_isDeserializationActive);

    for (auto& unresolvedHandle : m_unresolvedHandles) {
      resolveDeserializedHandle(unresolvedHandle, m_goDeserializationMode);
    }

    for (auto iter = m_endCallbacks.rbegin(); iter != m_endCallbacks.rend(); ++iter) {
      (*iter)();
    }

    m_isDeserializationActive = false;
    m_activeDeserializedObject = nullptr;
    m_idMapping.clear();
    m_unresolvedHandles.clear();
    m_endCallbacks.clear();
    m_unresolvedHandleData.clear();
  }

  void
  GameObjectManager::resolveDeserializedHandle(UnresolvedHandle& data, uint32 flags) {
    GE_ASSERT(m_isDeserializationActive);

    uint64 instanceId = data.originalInstanceId;

    bool isInternalReference = false;

    auto findIter = m_idMapping.find(instanceId);
    if (m_idMapping.end() != findIter) {
      if ((flags & GOHDM::kUseNewIds) != 0) {
        instanceId = findIter->second;
      }
      isInternalReference = true;
    }

    if (isInternalReference ||
        (!isInternalReference && (flags & GOHDM::kRestoreExternal) != 0)) {
      auto findIterObj = m_objects.find(instanceId);

      if (m_objects.end() != findIterObj) {
        data.handle._resolve(findIterObj->second);
      }
      else {
        if ((flags & GOHDM::kKeepMissing) == 0) {
          data.handle._resolve(nullptr);
        }
      }
    }
    else {
      if ((flags & GOHDM::kKeepMissing) == 0) {
        data.handle._resolve(nullptr);
      }
    }
  }

  void
  GameObjectManager::registerUnresolvedHandle(uint64 originalId,
                                              GameObjectHandleBase& object) {
# if GE_DEBUG_MODE
    if (!m_isDeserializationActive) {
      GE_EXCEPT(InvalidStateException,
                "Unresolved handle queue only be modified while "
                "deserialization is active.");
    }
# endif

    //All handles that are deserialized during a single
    //begin/endDeserialization session pointing to the same object must share
    //the same GameObjectHandleData as that makes certain operations in other
    //systems much simpler.
    //Therefore we store all the unresolved handles, and if a handle pointing
    //to the same object was already processed, or that object was already
    //created we replace the handle's internal GameObjectHandleData.

    //Update the provided handle to ensure all handles pointing to the same
    //object share the same handle data
    bool foundHandleData = false;

    //Search object that are currently being deserialized
    auto iterFind = m_idMapping.find(originalId);
    if (m_idMapping.end() != iterFind) {
      auto iterFind2 = m_objects.find(iterFind->second);
      if (m_objects.end() != iterFind2) {
        object.m_data = iterFind2->second.m_data;
        foundHandleData = true;
      }
    }

    //Search previously deserialized handles
    if (!foundHandleData) {
      auto iterFindUHD = m_unresolvedHandleData.find(originalId);
      if (m_unresolvedHandleData.end() != iterFindUHD) {
        object.m_data = iterFindUHD->second;
        foundHandleData = true;
      }
    }

    //If still not found, this is the first such handle so register its handle
    //data
    if (!foundHandleData) {
      m_unresolvedHandleData[originalId] = object.m_data;
    }
    m_unresolvedHandles.push_back({ originalId, object });
  }

  void
  GameObjectManager::registerOnDeserializationEndCallback(function<void()> callback) {
# if GE_DEBUG_MODE
    if (!m_isDeserializationActive) {
      GE_EXCEPT(InvalidStateException,
                "Callback queue only be modified while deserialization is "
                "active.");
    }
# endif
    m_endCallbacks.push_back(callback);
  }

  void
  GameObjectManager::setDeserializationMode(uint32 gameObjectDeserializationMode) {
# if GE_DEBUG_MODE
    if (m_isDeserializationActive) {
      GE_EXCEPT(InvalidStateException,
                "Deserialization modes can not be modified when "
                "deserialization is not active.");
    }
# endif
    m_goDeserializationMode = gameObjectDeserializationMode;
  }
}
