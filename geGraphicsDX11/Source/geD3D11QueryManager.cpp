/*****************************************************************************/
/**
 * @file    geD3D11QueryManager.cpp
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2018/06/02
 * @brief   Handles creation of DirectX 11 queries.
 *
 * Handles creation of DirectX 11 queries.
 *
 * @bug     No known bugs.
 */
/*****************************************************************************/

/*****************************************************************************/
/**
 * Includes
 */
/*****************************************************************************/
#include "geD3D11QueryManager.h"
#include "geD3D11EventQuery.h"
#include "geD3D11TimerQuery.h"
#include "geD3D11OcclusionQuery.h"

namespace geEngineSDK {
  namespace geCoreThread {
    SPtr<EventQuery>
    D3D11QueryManager::createEventQuery(uint32 deviceIdx) const {
      SPtr<EventQuery> query = SPtr<D3D11EventQuery>(ge_new<D3D11EventQuery>(deviceIdx),
                                                     &QueryManager::deleteEventQuery,
                                                     StdAlloc<D3D11EventQuery>());
      m_eventQueries.push_back(query.get());

      return query;
    }

    SPtr<TimerQuery>
    D3D11QueryManager::createTimerQuery(uint32 deviceIdx) const {
      SPtr<TimerQuery> query = SPtr<D3D11TimerQuery>(ge_new<D3D11TimerQuery>(deviceIdx),
                                                     &QueryManager::deleteTimerQuery,
                                                     StdAlloc<D3D11TimerQuery>());
      m_timerQueries.push_back(query.get());

      return query;
    }

    SPtr<OcclusionQuery>
    D3D11QueryManager::createOcclusionQuery(bool binary, uint32 deviceIdx) const {
      SPtr<OcclusionQuery> query = SPtr<D3D11OcclusionQuery>(
                                     ge_new<D3D11OcclusionQuery>(binary, deviceIdx),
                                     &QueryManager::deleteOcclusionQuery,
                                     StdAlloc<D3D11OcclusionQuery>());
      m_occlusionQueries.push_back(query.get());

      return query;
    }
  }
}
