/*****************************************************************************/
/**
 * @file    geGPUParamDesc.cpp
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2018/06/10
 * @brief   Describes a single GPU program data parameter.
 *
 * Describes a single GPU program data (i.e. int, float, Vector2) parameter.
 *
 * @bug     No known bugs.
 */
/*****************************************************************************/

/*****************************************************************************/
/**
 * Includes
 */
/*****************************************************************************/
#include "geGPUParamDesc.h"
#include "geGPUProgramRTTI.h"

namespace geEngineSDK {
  constexpr uint32 RTTIPlainType<GPUParamBlockDesc>::VERSION;
  constexpr uint32 RTTIPlainType<GPUParamObjectDesc>::VERSION;
  constexpr uint32 RTTIPlainType<GPUParamDataDesc>::VERSION;

  RTTITypeBase*
  GPUParamDesc::getRTTIStatic() {
    return GPUParamDescRTTI::instance();
  }

  RTTITypeBase*
  GPUParamDesc::getRTTI() const {
    return GPUParamDesc::getRTTIStatic();
  }
}
