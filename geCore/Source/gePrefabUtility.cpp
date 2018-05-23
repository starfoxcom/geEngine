/*****************************************************************************/
/**
 * @file    gePrefabUtility.cpp
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2018/05/05
 * @brief   Performs various prefab specific operations.
 *
 * Performs various prefab specific operations.
 *
 * @bug     No known bugs.
 */
/*****************************************************************************/

/*****************************************************************************/
/**
 * Includes
 */
/*****************************************************************************/
#include "gePrefabUtility.h"
#include "gePrefabDiff.h"
#include "gePrefab.h"
#include "geSceneObject.h"
#include "geResources.h"

#include <geNumericLimits.h>

namespace geEngineSDK {
  void
  PrefabUtility::revertToPrefab(const HSceneObject& so) {
    UUID prefabLinkUUID = so->getPrefabLink();
    HPrefab prefabLink =
      static_resource_cast<Prefab>(g_resources().loadFromUUID(prefabLinkUUID,
                                                              false,
                                                              RLF::kNone));

    if (!prefabLink.isLoaded(false)) {
      return;
    }

    //Save IDs, destroy original, create new, restore IDs
    SceneObjectProxy soProxy;
    UnorderedMap<uint32, GameObjectInstanceDataPtr> linkedInstanceData;
    recordInstanceData(so, soProxy, linkedInstanceData);

    HSceneObject parent = so->getParent();

    //This will destroy the object but keep it in the parent's child list
    HSceneObject currentSO = so;
    so->destroyInternal(currentSO, true);

    HSceneObject newInstance = prefabLink->instantiate();

    //Remove default parent, and replace with original one
    newInstance->m_parent->removeChild(newInstance);
    newInstance->m_parent = parent;

    restoreLinkedInstanceData(newInstance, soProxy, linkedInstanceData);
  }

  void
  PrefabUtility::updateFromPrefab(const HSceneObject& so) {
    HSceneObject topLevelObject = so;

    while (nullptr != topLevelObject) {
      if (!topLevelObject->m_prefabLinkUUID.empty())
        break;

      if (nullptr != topLevelObject->m_parent) {
        topLevelObject = topLevelObject->m_parent;
      }
      else {
        topLevelObject = nullptr;
      }
    }

    if (nullptr == topLevelObject) {
      topLevelObject = so;
    }

    Stack<HSceneObject> todo;
    todo.push(topLevelObject);

    //Find any prefab instances
    Vector<HSceneObject> prefabInstanceRoots;

    while (!todo.empty()) {
      HSceneObject current = todo.top();
      todo.pop();

      if (!current->m_prefabLinkUUID.empty()) {
        prefabInstanceRoots.push_back(current);
      }

      uint32 childCount = current->getNumChildren();
      for (uint32 i = 0; i < childCount; ++i) {
        HSceneObject child = current->getChild(i);
        todo.push(child);
      }
    }

    //Stores data about the new prefab instance and its original parent and
    //link id (as those aren't stored in the prefab diff)
    struct RestoredPrefabInstance
    {
      HSceneObject newInstance;
      HSceneObject originalParent;
      SPtr<PrefabDiff> diff;
      uint32 originalLinkId;
    };

    Vector<RestoredPrefabInstance> newPrefabInstanceData;

    //For each prefab instance load its reference prefab from the disk and
    //check if it changed. If it has changed instantiate the prefab and destroy
    //the current instance. Then apply instance specific changes stored in a
    //prefab diff, if any, as well as restore the original parent and link id
    //(link id of the root prefab instance belongs to the parent prefab if any)
    //Finally fix any handles pointing to the old objects so that they now
    //point to the newly instantiated objects. To the outside world it should
    //be transparent that we just destroyed and then re-created from scratch
    //the entire hierarchy.

    //Need to do this bottom up to ensure I don't destroy the parents before
    //children
    for (auto iter = prefabInstanceRoots.rbegin();
         iter != prefabInstanceRoots.rend();
         ++iter) {
      HSceneObject current = *iter;
      HPrefab prefabLink = static_resource_cast<Prefab>(
        g_resources().loadFromUUID(current->m_prefabLinkUUID, false, RLF::kNone)
      );

      if (prefabLink.isLoaded(false) &&
          prefabLink->getHash() != current->m_prefabHash) {
        //Save IDs, destroy original, create new, restore IDs
        SceneObjectProxy soProxy;
        UnorderedMap<uint32, GameObjectInstanceDataPtr> linkedInstanceData;
        recordInstanceData(current, soProxy, linkedInstanceData);

        HSceneObject parent = current->getParent();
        SPtr<PrefabDiff> prefabDiff = current->m_prefabDiff;

        current->destroy(true);
        HSceneObject newInstance = prefabLink->_clone();

        //When restoring instance IDs it is important to make all the new
        //handles point to the old GameObjectInstanceData.
        //This is because old handles will have different GameObjectHandleData
        //and we have no easy way of accessing it to change to which
        //GameObjectInstanceData it points. But the GameObjectManager ensures
        //that all handles deserialized at once (i.e. during the ::_clone()
        //call above) will share GameObjectHandleData so we can simply replace
        //to what they point to, affecting all of the handles to that object.
        //(In another words, we can modify the new handles at this point, but
        //old ones must keep referencing what they already were.)
        restoreLinkedInstanceData(newInstance, soProxy, linkedInstanceData);
        restoreUnlinkedInstanceData(newInstance, soProxy);

        newPrefabInstanceData.push_back({ newInstance,
                                          parent,
                                          prefabDiff,
                                          newInstance->getLinkId() });
      }
    }

    //Once everything is cloned, apply diffs, restore old parents & link IDs
    //for root.
    for (auto& entry : newPrefabInstanceData) {
      //Diffs must be applied after everything is instantiated and instance
      //data restored since it may contain game object handles within or
      //external to its prefab instance.
      if (nullptr != entry.diff) {
        entry.diff->apply(entry.newInstance);
      }

      entry.newInstance->m_prefabDiff = entry.diff;

      entry.newInstance->setParent(entry.originalParent, false);
      entry.newInstance->m_linkId = entry.originalLinkId;
    }

    //Finally, instantiate everything if the top scene object is live
    //(instantiated)
    if (topLevelObject->isInstantiated()) {
      for (auto& entry : newPrefabInstanceData) {
        entry.newInstance->_instantiate(true);
      }
    }

    g_resources().unloadAllUnused();
  }

  void
  PrefabUtility::generatePrefabIds(const HSceneObject& sceneObject) {
    uint32 startingId = 0;

    Stack<HSceneObject> todo;
    todo.push(sceneObject);

    while (!todo.empty()) {
      HSceneObject currentSO = todo.top();
      todo.pop();

      for (auto& component : currentSO->m_components) {
        if (component->getLinkId() != NumLimit::MAX_UINT32) {
          startingId = std::max(component->m_linkId + 1, startingId);
        }
      }

      uint32 numChildren = currentSO->getNumChildren();
      for (uint32 i = 0; i < numChildren; ++i) {
        HSceneObject child = currentSO->getChild(i);

        if (!child->hasFlag(SCENE_OBJECT_FLAGS::kDontSave)) {
          if (child->getLinkId() != NumLimit::MAX_UINT32) {
            startingId = std::max(child->m_linkId + 1, startingId);
          }
          if (child->m_prefabLinkUUID.empty()) {
            todo.push(currentSO->getChild(i));
          }
        }
      }
    }

    uint32 currentId = startingId;
    todo.push(sceneObject);

    while (!todo.empty()) {
      HSceneObject currentSO = todo.top();
      todo.pop();

      for (auto& component : currentSO->m_components) {
        if (component->getLinkId() == NumLimit::MAX_UINT32) {
          component->m_linkId = currentId++;
        }
      }

      uint32 numChildren = currentSO->getNumChildren();
      for (uint32 i = 0; i < numChildren; ++i) {
        HSceneObject child = currentSO->getChild(i);

        if (!child->hasFlag(SCENE_OBJECT_FLAGS::kDontSave)) {
          if (child->getLinkId() == NumLimit::MAX_UINT32) {
            child->m_linkId = currentId++;
          }
          if (child->m_prefabLinkUUID.empty()) {
            todo.push(currentSO->getChild(i));
          }
        }
      }
    }

    if (currentId < startingId) {
      GE_EXCEPT(InternalErrorException,
        "Prefab ran out of IDs to assign. Consider increasing the size of "
        "the prefab ID data type.");
    }
  }

  void
  PrefabUtility::clearPrefabIds(const HSceneObject& sceneObject,
                                bool recursive,
                                bool clearRoot) {
    Stack<HSceneObject> todo;
    todo.push(sceneObject);

    if (clearRoot) {
      sceneObject->m_linkId = NumLimit::MAX_UINT32;
    }

    while (!todo.empty()) {
      HSceneObject currentSO = todo.top();
      todo.pop();

      for (auto& component : currentSO->m_components) {
        component->m_linkId = NumLimit::MAX_UINT32;
      }

      if (recursive) {
        uint32 numChildren = currentSO->getNumChildren();
        for (uint32 i = 0; i < numChildren; ++i) {
          HSceneObject child = currentSO->getChild(i);
          child->m_linkId = NumLimit::MAX_UINT32;

          if (child->m_prefabLinkUUID.empty()) {
            todo.push(child);
          }
        }
      }
    }
  }

  void
  PrefabUtility::recordPrefabDiff(const HSceneObject& sceneObject) {
    HSceneObject topLevelObject = sceneObject;

    while (nullptr != topLevelObject) {
      if (!topLevelObject->m_prefabLinkUUID.empty()) {
        break;
      }

      if (nullptr != topLevelObject->m_parent) {
        topLevelObject = topLevelObject->m_parent;
      }
      else {
        topLevelObject = nullptr;
      }
    }

    if (nullptr == topLevelObject) {
      topLevelObject = sceneObject;
    }

    Stack<HSceneObject> todo;
    todo.push(topLevelObject);

    while (!todo.empty()) {
      HSceneObject current = todo.top();
      todo.pop();

      if (!current->m_prefabLinkUUID.empty()) {
        current->m_prefabDiff = nullptr;

        HPrefab prefabLink =
          static_resource_cast<Prefab>(g_resources().loadFromUUID(current->m_prefabLinkUUID,
                                                                  false,
                                                                  RLF::kNone));
        if (prefabLink.isLoaded(false)) {
          current->m_prefabDiff = PrefabDiff::create(prefabLink->_getRoot(),
                                                     current->getHandle());
        }
      }

      uint32 childCount = current->getNumChildren();
      for (uint32 i = 0; i < childCount; ++i) {
        HSceneObject child = current->getChild(i);
        todo.push(child);
      }
    }

    g_resources().unloadAllUnused();
  }

  void
  PrefabUtility::recordInstanceData(const HSceneObject& so,
                                    SceneObjectProxy& output,
                                    UnorderedMap<uint32, GameObjectInstanceDataPtr>&
                                    linkedInstanceData) {
    struct StackData
    {
      HSceneObject so;
      SceneObjectProxy* proxy;
    };

    Stack<StackData> todo;
    todo.push({ so, &output });

    output.instanceData = so->_getInstanceData();
    output.linkId = NumLimit::MAX_UINT32;

    while (!todo.empty()) {
      StackData curData = todo.top();
      todo.pop();

      const Vector<HComponent>& components = curData.so->getComponents();
      for (auto& component : components) {
        curData.proxy->components.emplace_back();

        ComponentProxy& componentProxy = curData.proxy->components.back();
        componentProxy.instanceData = component->_getInstanceData();
        componentProxy.linkId = component->getLinkId();

        linkedInstanceData[componentProxy.linkId] = componentProxy.instanceData;
      }

      uint32 numChildren = curData.so->getNumChildren();
      curData.proxy->children.resize(numChildren);

      for (uint32 i = 0; i < numChildren; ++i) {
        HSceneObject child = curData.so->getChild(i);

        SceneObjectProxy& childProxy = curData.proxy->children[i];

        childProxy.instanceData = child->_getInstanceData();
        childProxy.linkId = child->getLinkId();

        linkedInstanceData[childProxy.linkId] = childProxy.instanceData;

        if (child->m_prefabLinkUUID.empty()) {
          todo.push({ child, &curData.proxy->children[i] });
        }
      }
    }
  }

  void
  PrefabUtility::restoreLinkedInstanceData(const HSceneObject& so,
                                           SceneObjectProxy& /*proxy*/,
                                           UnorderedMap<uint32, GameObjectInstanceDataPtr>&
                                           linkedInstanceData) {
    Stack<HSceneObject> todo;
    todo.push(so);

    while (!todo.empty()) {
      HSceneObject current = todo.top();
      todo.pop();

      Vector<HComponent>& components = current->m_components;
      for (auto& component : components) {
        if (component->getLinkId() != NumLimit::MAX_UINT32) {
          auto iterFind = linkedInstanceData.find(component->getLinkId());
          if (iterFind != linkedInstanceData.end()) {
            component->_setInstanceData(iterFind->second);
            component._setHandleData(component.getInternalPtr());
          }
        }
      }

      uint32 numChildren = current->getNumChildren();
      for (uint32 i = 0; i < numChildren; ++i) {
        HSceneObject child = current->getChild(i);

        if (child->getLinkId() != NumLimit::MAX_UINT32) {
          auto iterFind = linkedInstanceData.find(child->getLinkId());
          if (iterFind != linkedInstanceData.end()) {
            child->_setInstanceData(iterFind->second);
          }
        }

        if (child->m_prefabLinkUUID.empty()) {
          todo.push(child);
        }
      }
    }
  }

  void
  PrefabUtility::restoreUnlinkedInstanceData(const HSceneObject& so,
                                             SceneObjectProxy& proxy) {
    struct StackEntry
    {
      HSceneObject so;
      SceneObjectProxy* proxy;
    };

    Stack<StackEntry> todo;
    todo.push(StackEntry());

    StackEntry& topEntry = todo.top();
    topEntry.so = so;
    topEntry.proxy = &proxy;

    while (!todo.empty()) {
      StackEntry current = todo.top();
      todo.pop();

      if (current.proxy->linkId == NumLimit::MAX_UINT32) {
        current.so->_setInstanceData(current.proxy->instanceData);
      }

      Vector<HComponent>& components = current.so->m_components;
      uint32 componentProxyIdx = 0;
      uint32 numComponentProxies = static_cast<uint32>(current.proxy->components.size());
      for (auto& component : components) {
        if (component->getLinkId() == NumLimit::MAX_UINT32) {
          bool foundInstanceData = false;
          (void)foundInstanceData;

          for (; componentProxyIdx < numComponentProxies; ++componentProxyIdx) {
            if (NumLimit::MAX_UINT32 != current.proxy->components[componentProxyIdx].linkId) {
              continue;
            }

            component->_setInstanceData(current.proxy->
                                          components[componentProxyIdx].instanceData);
            component._setHandleData(component.getInternalPtr());

            foundInstanceData = true;
            break;
          }

          GE_ASSERT(foundInstanceData);
        }
      }

      uint32 numChildren = current.so->getNumChildren();
      uint32 childProxyIdx = 0;
      uint32 numChildProxies = static_cast<uint32>(current.proxy->children.size());
      for (uint32 i = 0; i < numChildren; ++i) {
        HSceneObject child = current.so->getChild(i);

        if (child->getLinkId() == NumLimit::MAX_UINT32) {
          bool foundInstanceData = false;
          (void)foundInstanceData;

          for (; childProxyIdx < numChildProxies; ++childProxyIdx) {
            if (current.proxy->children[childProxyIdx].linkId != NumLimit::MAX_UINT32) {
              continue;
            }

            GE_ASSERT(NumLimit::MAX_UINT32 == current.proxy->children[childProxyIdx].linkId);
            child->_setInstanceData(current.proxy->children[childProxyIdx].instanceData);

            if (child->m_prefabLinkUUID.empty()) {
              todo.push(StackEntry());

              StackEntry& newEntry = todo.top();
              newEntry.so = child;
              newEntry.proxy = &current.proxy->children[childProxyIdx];
            }

            foundInstanceData = true;
            break;
          }

          GE_ASSERT(foundInstanceData);
        }
        else {
          if (!child->m_prefabLinkUUID.empty()) {
            continue;
          }

          for (uint32 j = 0; j < numChildProxies; ++j) {
            if (child->getLinkId() == current.proxy->children[j].linkId) {
              todo.push(StackEntry());

              StackEntry& newEntry = todo.top();
              newEntry.so = child;
              newEntry.proxy = &current.proxy->children[j];
              break;
            }
          }
        }
      }
    }
  }
}
