/*****************************************************************************/
/**
 * @file    geGPUResourceDataRTTI.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2017/11/29
 * @brief   RTTI Objects for geGPUResourceData.
 *
 * RTTI Objects for geGPUResourceData.
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
#include <geGPUResourceData.h>
#include <geRTTIType.h>

namespace geEngineSDK {
  class GE_CORE_EXPORT GPUResourceDataRTTI
    : public RTTIType<GPUResourceData, IReflectable, GPUResourceDataRTTI>
  {
   public:
    GPUResourceDataRTTI() {}

    const String&
    getRTTIName() override {
      static String name = "GPUResourceData";
      return name;
    }

    uint32
    getRTTIId() override {
      return TYPEID_CORE::kID_GPUResourceData;
    }

    SPtr<IReflectable>
    newRTTIObject() override {
      GE_EXCEPT(InternalErrorException,
                "Cannot instantiate an abstract class.");
      return nullptr;
    }
  };
}
