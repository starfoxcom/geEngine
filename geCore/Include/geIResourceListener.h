/*****************************************************************************/
/**
 * @file    geIResourceListener.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2017/11/30
 * @brief   Interface that allows the implementing class to be notified when
 *          the resources it is referencing change.
 *
 * Interface that allows the implementing class to be notified when the
 * resources it is referencing change.
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

namespace geEngineSDK {
  class GE_CORE_EXPORT IResourceListener
  {
   public:
    IResourceListener();
    virtual ~IResourceListener();

   protected:
    friend class ResourceListenerManager;

    /**
     * @brief Retrieves all the resources that the class depends on.
     * @note  Derived implementations must add the resources to the provided @p
     *        resources array.
     */
    virtual void
    getListenerResources(Vector<HResource>& resources) = 0;

    /**
     * @brief Marks the resource dependencies list as dirty and schedules it
     *        for rebuild.
     */
    virtual void
    markListenerResourcesDirty();

    /**
     * @brief Called when a resource has been fully loaded.
     */
    virtual void
    notifyResourceLoaded(const HResource& /*resource*/) {}

    /**
     * @brief Called when the internal resource handle is pointing to changes.
     */
    virtual void
    notifyResourceChanged(const HResource& /*resource*/) {}
  };
}
