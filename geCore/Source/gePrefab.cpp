/*****************************************************************************/
/**
 * @file    gePrefab.cpp
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2018/05/05
 * @brief   Prefab is a saveable hierarchy of scene objects.
 *
 * Prefab is a saveable hierarchy of scene objects. In general it can serve as
 * any grouping of scene objects (for example a level) or be used as a form of
 * a template instantiated and reused throughout the scene.
 *
 * @bug     No known bugs.
 */
/*****************************************************************************/

/*****************************************************************************/
/**
 * Includes
 */
/*****************************************************************************/
#include "gePrefab.h"
#include "gePrefabRTTI.h"
#include "geResources.h"
#include "geSceneObject.h"
#include "gePrefabUtility.h"
#include "geCoreApplication.h"

#include <geNumericLimits.h>

namespace geEngineSDK {
  Prefab::Prefab() : Resource(false), m_hash(0), m_isScene(true) {}

  Prefab::~Prefab() {
    if (nullptr != m_root) {
      m_root->destroy(true);
    }
  }

  HPrefab
  Prefab::create(const HSceneObject& sceneObject, bool isScene) {
    SPtr<Prefab> newPrefab = createEmpty();
    newPrefab->m_isScene = isScene;

    PrefabUtility::clearPrefabIds(sceneObject, true, false);
    newPrefab->initialize(sceneObject);

    HPrefab handle =
      static_resource_cast<Prefab>(g_resources()._createResourceHandle(newPrefab));

    newPrefab->m_uuid = handle.getUUID();
    sceneObject->m_prefabLinkUUID = newPrefab->m_uuid;
    newPrefab->_getRoot()->m_prefabLinkUUID = newPrefab->m_uuid;

    return handle;
  }

  SPtr<Prefab>
  Prefab::createEmpty() {
    SPtr<Prefab> newPrefab = ge_core_ptr<Prefab>(new (ge_alloc<Prefab>()) Prefab());
    newPrefab->_setThisPtr(newPrefab);
    return newPrefab;
  }

  void
  Prefab::initialize(const HSceneObject& sceneObject) {
    sceneObject->m_prefabDiff = nullptr;
    PrefabUtility::generatePrefabIds(sceneObject);

    //If there are any child prefab instances, make sure to update their diffs
    //so they are saved with this prefab
    Stack<HSceneObject> todo;
    todo.push(sceneObject);

    while (!todo.empty()) {
      HSceneObject current = todo.top();
      todo.pop();

      uint32 childCount = current->getNumChildren();
      for (uint32 i = 0; i < childCount; ++i) {
        HSceneObject child = current->getChild(i);

        if (!child->m_prefabLinkUUID.empty()) {
          PrefabUtility::recordPrefabDiff(child);
        }
        else {
          todo.push(child);
        }
      }
    }

    //Clone the hierarchy for internal storage
    if (nullptr != m_root) {
      m_root->destroy(true);
    }

    m_root = sceneObject->clone(false);
    m_root->m_parent = nullptr;
    m_root->m_linkId = NumLimit::MAX_UINT32;

    //Remove objects with "don't save" flag
    todo.push(m_root);

    while (!todo.empty()) {
      HSceneObject current = todo.top();
      todo.pop();

      if (current->hasFlag(SCENE_OBJECT_FLAGS::kDontSave)) {
        current->destroy();
      }
      else {
        uint32 numChildren = current->getNumChildren();
        for (uint32 i = 0; i < numChildren; ++i) {
          todo.push(current->getChild(i));
        }
      }
    }
  }

  void
  Prefab::update(const HSceneObject& sceneObject) {
    initialize(sceneObject);
    sceneObject->m_prefabLinkUUID = m_uuid;
    m_root->m_prefabLinkUUID = m_uuid;
    ++m_hash;
  }

  void
  Prefab::_updateChildInstances() {
    Stack<HSceneObject> todo;
    todo.push(m_root);

    while (!todo.empty()) {
      HSceneObject current = todo.top();
      todo.pop();

      uint32 childCount = current->getNumChildren();
      for (uint32 i = 0; i < childCount; ++i) {
        HSceneObject child = current->getChild(i);

        if (!child->m_prefabLinkUUID.empty()) {
          PrefabUtility::updateFromPrefab(child);
        }
        else {
          todo.push(child);
        }
      }
    }
  }

  HSceneObject
  Prefab::instantiate() {
    if (nullptr == m_root) {
      return HSceneObject();
    }

#if GE_IS_ENGINEEDITOR
    if (g_coreApplication().isEditor()) {
      //Update any child prefab instances in case their prefabs changed
      _updateChildInstances();
    }
#endif
    HSceneObject clone = _clone();
    clone->_instantiate();

    return clone;
  }

  HSceneObject
  Prefab::_clone() {
    if (nullptr == m_root) {
      return HSceneObject();
    }

    m_root->m_prefabHash = m_hash;
    m_root->m_linkId = NumLimit::MAX_UINT32;

    return m_root->clone(false);
  }

  RTTITypeBase*
  Prefab::getRTTIStatic() {
    return PrefabRTTI::instance();
  }

  RTTITypeBase*
  Prefab::getRTTI() const {
    return Prefab::getRTTIStatic();
  }
}
