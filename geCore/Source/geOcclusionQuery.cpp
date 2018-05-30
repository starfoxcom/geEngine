/*****************************************************************************/
/**
 * @file    geOcclusionQuery.cpp
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2018/05/29
 * @brief   Query that counts number of samples rendered by the GPU while the
 *          query is active.
 *
 * Represents a query that counts number of samples rendered by the GPU while
 * the query is active.
 *
 * @note    Core thread only.
 *
 * @bug     No known bugs.
 */
/*****************************************************************************/

/*****************************************************************************/
/**
 * Includes
 */
/*****************************************************************************/
#include "geOcclusionQuery.h"
#include "geQueryManager.h"

namespace geEngineSDK {
  namespace geCoreThread {
    OcclusionQuery::OcclusionQuery(bool binary)
      : m_active(false),
        m_binary(binary)
    {}

    SPtr<OcclusionQuery>
    OcclusionQuery::create(bool binary, uint32 deviceIdx) {
      return QueryManager::instance().createOcclusionQuery(binary, deviceIdx);
    }
  }
}
