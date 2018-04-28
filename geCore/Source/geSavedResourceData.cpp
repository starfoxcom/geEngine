/*****************************************************************************/
/**
 * @file    geSavedResourceData.cpp
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

/*****************************************************************************/
/**
 * Includes
 */
/*****************************************************************************/
#include "geSavedResourceData.h"
#include "geSavedResourceDataRTTI.h"

namespace geEngineSDK {
  SavedResourceData::SavedResourceData(const Vector<UUID>& dependencies,
                                       bool allowAsync,
                                       uint32 compressionMethod)
    : m_dependencies(dependencies),
      m_allowAsync(allowAsync),
      m_compressionMethod(compressionMethod)
  {}

  RTTITypeBase*
  SavedResourceData::getRTTIStatic() {
    return SavedResourceDataRTTI::instance();
  }

  RTTITypeBase*
  SavedResourceData::getRTTI() const {
    return SavedResourceData::getRTTIStatic();
  }
}
