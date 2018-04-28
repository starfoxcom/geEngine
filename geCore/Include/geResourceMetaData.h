/*****************************************************************************/
/**
 * @file    geResourceMetaData.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2017/11/29
 * @brief   Class containing meta-information describing a resource.
 *
 * Class containing meta-information describing a resource.
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
  class GE_CORE_EXPORT ResourceMetaData : public IReflectable
  {
   public:
    String m_displayName;

    /*************************************************************************/
    /**
     * Serialization
     */
    /*************************************************************************/
   public:
    friend class ResourceMetaDataRTTI;

    static RTTITypeBase*
    getRTTIStatic();

    virtual RTTITypeBase*
    getRTTI() const override;
  };
}
