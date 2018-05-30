/*****************************************************************************/
/**
 * @file    geTimerQuery.cpp
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2018/05/29
 * @brief   GPU query that measures execution time of GPU operations.
 *
 * Represents a GPU query that measures execution time of GPU operations.
 * The query will measure any GPU operations that take place between its
 * begin() and end() calls.
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
#include "geTimerQuery.h"
#include "geQueryManager.h"

namespace geEngineSDK {
  namespace geCoreThread {
    SPtr<TimerQuery>
    TimerQuery::create(uint32 deviceIdx) {
      return QueryManager::instance().createTimerQuery(deviceIdx);
    }
  }
}
