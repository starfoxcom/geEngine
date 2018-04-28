/*****************************************************************************/
/**
 * @file    geResourceMetaDataRTTI.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2017/11/29
 * @brief   RTTI Objects for geResourceMetaData.
 *
 * RTTI Objects for geResourceMetaData.
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
#include "geResourceMetaData.h"
#include <geRTTIType.h>

namespace geEngineSDK {
  class GE_CORE_EXPORT ResourceMetaDataRTTI
    : public RTTIType<ResourceMetaData, IReflectable, ResourceMetaDataRTTI>
  {
   private:
    String&
    getDisplayName(ResourceMetaData* obj) {
      return obj->m_displayName;
    }

    void
    setDisplayName(ResourceMetaData* obj, String& name) {
      obj->m_displayName = name;
    }

   public:
    ResourceMetaDataRTTI() {
      addPlainField("m_displayName",
                    0,
                    &ResourceMetaDataRTTI::getDisplayName,
                    &ResourceMetaDataRTTI::setDisplayName);
    }

    const String&
    getRTTIName() override {
      static String name = "ResourceMetaData";
      return name;
    }

    uint32
    getRTTIId() override {
      return TYPEID_CORE::kID_ResourceMetaData;
    }

    SPtr<IReflectable>
    newRTTIObject() override {
      return ge_shared_ptr_new<ResourceMetaData>();
    }
  };
}
