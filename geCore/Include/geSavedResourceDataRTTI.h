/*****************************************************************************/
/**
 * @file    geSavedResourceDataRTTI.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2017/12/04
 * @brief   RTTI Objects for geSavedResourceData.
 *
 * RTTI Objects for geSavedResourceData.
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
#include "geSavedResourceData.h"
#include <geRTTIType.h>

namespace geEngineSDK {
  class GE_CORE_EXPORT SavedResourceDataRTTI
    : public RTTIType <SavedResourceData, IReflectable, SavedResourceDataRTTI>
  {
   private:
    GE_BEGIN_RTTI_MEMBERS
      GE_RTTI_MEMBER_PLAIN_ARRAY(m_dependencies,  0)
      GE_RTTI_MEMBER_PLAIN(m_allowAsync,          1)
      GE_RTTI_MEMBER_PLAIN(m_compressionMethod,   2)
    GE_END_RTTI_MEMBERS

   public:
    SavedResourceDataRTTI() : m_initMembers(this) {}

    const String&
    getRTTIName() override {
      static String name = "ResourceDependencies";
      return name;
    }

    uint32
    getRTTIId() override {
      return TYPEID_CORE::kID_ResourceDependencies;
    }

    SPtr<IReflectable>
    newRTTIObject() override {
      return ge_shared_ptr_new<SavedResourceData>();
    }
  };
}
