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

/*****************************************************************************/
/**
 * Includes
 */
/*****************************************************************************/
#include "geIResourceListener.h"
#include "geResourceListenerManager.h"

namespace geEngineSDK {
  IResourceListener::IResourceListener() {
    ResourceListenerManager::instance().registerListener(this);
  }

  IResourceListener::~IResourceListener() {
    ResourceListenerManager::instance().unregisterListener(this);
  }

  void
  IResourceListener::markListenerResourcesDirty() {
    ResourceListenerManager::instance().markListenerDirty(this);
  }
}
