/*****************************************************************************/
/**
 * @file    geSavedResourceData.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2017/12/04
 * @brief   Contains information about a resource saved to the disk.
 *
 * Contains information about a resource saved to the disk.
 * Purpose of this class is primarily to be a wrapper around a list of objects
 * to make serialization easier.
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
#include <geIReflectable.h>

namespace geEngineSDK {
  class GE_CORE_EXPORT SavedResourceData : public IReflectable
  {
   public:
    SavedResourceData() = default;
    SavedResourceData(const Vector<UUID>& dependencies,
                      bool allowAsync,
                      uint32 compressionMethod);

    /**
     * @brief Returns a list of all resource dependencies.
     */
    const Vector<UUID>&
    getDependencies() const {
      return m_dependencies;
    }

    /**
     * @brief Returns true if this resource is allow to be asynchronously loaded.
     */
    bool
    allowAsyncLoading() const {
      return m_allowAsync;
    }

    /**
     * @brief Returns the method used for compressing the resource. 0 if none.
     */
    uint32
    getCompressionMethod() const {
      return m_compressionMethod;
    }

   private:
    Vector<UUID> m_dependencies;
    bool m_allowAsync = true;
    uint32 m_compressionMethod = 0;

    /*************************************************************************/
    /**
     * Serialization
     */
    /*************************************************************************/
   public:
    friend class SavedResourceDataRTTI;

    static RTTITypeBase*
    getRTTIStatic();

    RTTITypeBase*
    getRTTI() const override;
  };
}
