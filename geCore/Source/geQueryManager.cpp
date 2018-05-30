/*****************************************************************************/
/**
 * @file    geQueryManager.cpp
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2018/05/29
 * @brief   Handles creation and destruction of GPU queries.
 *
 * Handles creation and destruction of GPU queries.
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
#include "geQueryManager.h"
#include "geEventQuery.h"
#include "geTimerQuery.h"
#include "geOcclusionQuery.h"
#include "geProfilerCPU.h"

namespace geEngineSDK {
  using std::find;

  namespace geCoreThread {
    QueryManager::~QueryManager() {
      //Trigger all remaining queries, whether they completed or not
      for (auto& query : m_eventQueries) {
        if (query->isActive()) {
          query->onTriggered();
        }
      }

      for (auto& query : m_timerQueries) {
        if (query->isActive()) {
          query->onTriggered(query->getTimeMs());
        }
      }

      for (auto& query : m_occlusionQueries) {
        if (query->isActive()) {
          query->onComplete(query->getNumSamples());
        }
      }

      processDeletedQueue();
    }

    void
    QueryManager::_update() {
      for (auto& query : m_eventQueries) {
        if (query->isActive() && query->isReady()) {
          query->onTriggered();
          query->setActive(false);
        }
      }

      for (auto& query : m_timerQueries) {
        if (query->isActive() && query->isReady()) {
          query->onTriggered(query->getTimeMs());
          query->setActive(false);
        }
      }

      for (auto& query : m_occlusionQueries) {
        if (query->isActive() && query->isReady()) {
          query->onComplete(query->getNumSamples());
          query->setActive(false);
        }
      }

      processDeletedQueue();
    }

    void
    QueryManager::deleteEventQuery(EventQuery* query) {
      instance().m_deletedEventQueries.push_back(query);
    }

    void
    QueryManager::deleteTimerQuery(TimerQuery* query) {
      instance().m_deletedTimerQueries.push_back(query);
    }

    void
    QueryManager::deleteOcclusionQuery(OcclusionQuery* query) {
      instance().m_deletedOcclusionQueries.push_back(query);
    }

    void
    QueryManager::processDeletedQueue() {
      for (auto& query : m_deletedEventQueries) {
        auto iterFind = find(m_eventQueries.begin(),
                             m_eventQueries.end(),
                             query);

        if (iterFind != m_eventQueries.end()) {
          m_eventQueries.erase(iterFind);
        }

        ge_delete(query);
      }

      m_deletedEventQueries.clear();

      for (auto& query : m_deletedTimerQueries) {
        auto iterFind = find(m_timerQueries.begin(),
                             m_timerQueries.end(),
                             query);

        if (iterFind != m_timerQueries.end()) {
          m_timerQueries.erase(iterFind);
        }

        ge_delete(query);
      }

      m_deletedTimerQueries.clear();

      for (auto& query : m_deletedOcclusionQueries) {
        auto iterFind = find(m_occlusionQueries.begin(),
                             m_occlusionQueries.end(),
                             query);

        if (iterFind != m_occlusionQueries.end()) {
          m_occlusionQueries.erase(iterFind);
        }

        ge_delete(query);
      }

      m_deletedOcclusionQueries.clear();
    }
  }
}
