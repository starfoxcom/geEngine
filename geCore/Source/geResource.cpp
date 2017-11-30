/*****************************************************************************/
/**
 * @file    geResource.cpp
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2017/11/29
 * @brief   Base class for all resources.
 *
 * Base class for all resources.
 *
 * @bug     No known bugs.
 */
/*****************************************************************************/

/*****************************************************************************/
/**
 * Includes
 */
/*****************************************************************************/
#include "geResource.h"
#include "geResourceRTTI.h"
#include "geResourceMetaData.h"

namespace geEngineSDK {
  Resource::Resource(bool initializeOnRenderThread)
    : CoreObject(initializeOnRenderThread),
      m_size(0),
      m_keepSourceData(true) {
    m_metaData = ge_shared_ptr_new<ResourceMetaData>();
  }

  const WString&
  Resource::getName() const {
    return m_metaData->m_displayName;
  }

  void
  Resource::setName(const WString& name) {
    m_metaData->m_displayName = name;
  }

  bool
  Resource::areDependenciesLoaded() const {
    ge_frame_mark();

    bool areLoaded = true;
    {
      FrameVector<HResource> dependencies;
      getResourceDependencies(dependencies);

      for (auto& dependency : dependencies) {
        if (nullptr != dependency && !dependency.isLoaded()) {
          areLoaded = false;
          break;
        }
      }
    }

    ge_frame_clear();
    return areLoaded;
  }

  RTTITypeBase*
  Resource::getRTTIStatic() {
    return ResourceRTTI::instance();
  }

  RTTITypeBase*
  Resource::getRTTI() const {
    return Resource::getRTTIStatic();
  }
}
