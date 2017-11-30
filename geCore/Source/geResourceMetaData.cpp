/*****************************************************************************/
/**
 * @file    geResourceMetaData.cpp
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2017/11/29
 * @brief   Class containing meta-information describing a resource.
 *
 * Class containing meta-information describing a resource.
 *
 * @bug     No known bugs.
 */
/*****************************************************************************/

/*****************************************************************************/
/**
 * Includes
 */
/*****************************************************************************/
#include "geResourceMetaData.h"
#include "geResourceMetaDataRTTI.h"

namespace geEngineSDK {
  RTTITypeBase*
  ResourceMetaData::getRTTIStatic() {
    return ResourceMetaDataRTTI::instance();
  }

  RTTITypeBase*
  ResourceMetaData::getRTTI() const {
    return ResourceMetaData::getRTTIStatic();
  }
}
