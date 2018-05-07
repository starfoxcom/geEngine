/*****************************************************************************/
/**
 * @file    gePrefab.h
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
#pragma once

/*****************************************************************************/
/**
 * Includes
 */
/*****************************************************************************/
#include "gePrerequisitesCore.h"
#include "geGameObject.h"
#include "geResource.h"

namespace geEngineSDK {
  class GE_CORE_EXPORT Prefab : public Resource
  {
   public:
    Prefab();
    ~Prefab();

    /**
     * @brief Creates a new prefab from the provided scene object. If the scene
     *        object has an existing prefab link it will be broken. After the
     *        prefab is created the scene object will be automatically linked
     *        to it.
     * @param[in] sceneObject Scene object to create the prefab from.
     * @param[in] isScene     Determines if the prefab represents a scene or
     *                        just a generic group of objects.
     * @see isScene().
     */
    static HPrefab
    create(const HSceneObject& sceneObject, bool isScene = true);

    /**
     * @brief Instantiates a prefab by creating an instance of the prefab's
     *        scene object hierarchy. The returned hierarchy will be parented
     *        to world root by default.
     * @return  Instantiated clone of the prefab's scene object hierarchy.
     */
    HSceneObject
    instantiate();

    /**
     * @brief Replaces the contents of this prefab with new contents from the
     *        provided object. Object will be automatically linked to this
     *        prefab, and its previous prefab link (if any) will be broken.
     */
    void
    update(const HSceneObject& sceneObject);

    /**
     * @brief Returns a hash value that can be used for determining if a prefab
     *        changed by comparing it to a previously saved hash.
     */
    uint32
    getHash() const {
      return m_hash;
    }

    /**
     * @brief Determines if the prefab represents a scene or just a generic
     *        group of objects. The only difference between the two is the way
     *        root object is handled: scenes are assumed to be saved with the
     *        scene root object (which is hidden), while object group root is
     *        a normal scene object (not hidden). This is relevant when when
     *        prefabs are loaded, so the systems knows to append the root
     *        object to non-scene prefabs.
     */
    bool
    isScene() const {
      return m_isScene;
    }

   public:
    /**
     * @brief Updates any prefab child instances by loading their prefabs and
     *        making sure they are up to date.
     */
    void
    _updateChildInstances();

    /**
     * @brief Returns a reference to the internal prefab hierarchy. Returned
     *        hierarchy is not instantiated and cannot be interacted with in a
     *        manner you would with normal scene objects.
     */
    HSceneObject
    _getRoot() const {
      return m_root;
    }

    /**
     * @brief Creates the clone of the prefab's current hierarchy but doesn't
     *        instantiate it.
     * @return  Clone of the prefab's scene object hierarchy.
     */
    HSceneObject
    _clone();


   private:
    using CoreObject::initialize;

    /**
     * @brief Initializes the internal prefab hierarchy. Must be called during
     *        creation.
     */
    void
    initialize(const HSceneObject& sceneObject);

    /**
     * @brief Creates an empty and uninitialized prefab.
     */
    static SPtr<Prefab>
    createEmpty();

    HSceneObject m_root;
    uint32 m_hash;
    UUID m_uuid;
    bool m_isScene;

    /*************************************************************************/
    /**
     * RTTI
     */
    /*************************************************************************/
   public:
    friend class PrefabRTTI;

    static RTTITypeBase*
    getRTTIStatic();

    RTTITypeBase*
    getRTTI() const override;
  };
}
