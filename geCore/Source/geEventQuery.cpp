/*****************************************************************************/
/**
 * @file    geEventQuery.cpp
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2018/05/29
 * @brief   GPU query that gets triggered when GPU starts processing the query.
 *
 * Represents a GPU query that gets triggered when GPU starts processing the
 * query.
 *
 * @bug     No known bugs.
 */
/*****************************************************************************/

/*****************************************************************************/
/**
 * Includes
 */
/*****************************************************************************/
#include "geEventQuery.h"
#include "geQueryManager.h"

namespace geEngineSDK {
  namespace geCoreThread {
    SPtr<EventQuery>
    EventQuery::create(uint32 deviceIdx) {
      return QueryManager::instance().createEventQuery(deviceIdx);
    }
  }
}
