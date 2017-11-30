/*****************************************************************************/
/**
 * @file    geResource.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2017/11/29
 * @brief   Base class for all resources.
 *
 * Base class for all resources.
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
#include "geCoreObject.h"
#include <geIReflectable.h>

namespace geEngineSDK {
  class GE_CORE_EXPORT Resource : public IReflectable, public CoreObject
  {
   public:
    Resource(bool requiresGpuInitialization = true);
    virtual ~Resource() {};

    /**
     * @brief Returns the name of the resource.
     */
    const WString&
    getName() const;

    /**
     * @brief Sets the name of the resource.
     */
    void
    setName(const WString& name);

    /**
     * @brief Retrieves meta-data containing various information describing a
     *        resource.
     */
    SPtr<ResourceMetaData>
    getMetaData() const {
      return m_metaData;
    }

    /**
     * @brief Returns whether or not this resource is allowed to be
     *        asynchronously loaded.
     */
    virtual bool
    allowAsyncLoading() const {
      return true;
    }

   protected:
    friend class Resources;
    friend class ResourceHandleBase;

    /**
     * @brief Retrieves a list of all resources that this resource depends on.
     */
    virtual void
    getResourceDependencies(FrameVector<HResource>& /*dependencies*/) const {}

    /**
     * @brief Checks if all the resources this object is dependent on are fully
     *        loaded.
     */
    bool
    areDependenciesLoaded() const;

    /**
     * @brief Returns true if the resource can be compressed using a generic
     *        compression when saved on a storage device. Certain resources
     *        already have their contents compressed (like audio files) and
     *        will not benefit from further compression. Resources supporting
     *        streaming should never be compressed, instead such resources can
     *        handle compression/decompression locally through their streams.
     */
    virtual bool
    isCompressible() const {
      return true;
    }

    uint32 m_size;
    SPtr<ResourceMetaData> m_metaData;

    /**
     * @brief Signal to the resource implementation if original data should be
     *        kept in memory. This is sometimes needed if the resource destroys
     *        original data during normal usage, but it might still be required
     *        for special purposes (like saving in the editor).
     */
    bool m_keepSourceData;

    /*************************************************************************/
    /**
     * Serialization
     */
    /*************************************************************************/
   public:
    friend class ResourceRTTI;
    
    static RTTITypeBase*
    getRTTIStatic();

    RTTITypeBase*
    getRTTI() const override;
  };
}
