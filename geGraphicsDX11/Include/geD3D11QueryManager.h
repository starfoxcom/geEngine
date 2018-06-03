/*****************************************************************************/
/**
 * @file    geD3D11QueryManager.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2018/06/02
 * @brief   Handles creation of DirectX 11 queries.
 *
 * Handles creation of DirectX 11 queries.
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
#include "gePrerequisitesD3D11.h"

#include <geQueryManager.h>

namespace geEngineSDK {
  namespace geCoreThread {
    class D3D11QueryManager : public QueryManager
    {
     public:
      /**
       * @copydoc QueryManager::createEventQuery
       */
      SPtr<EventQuery>
      createEventQuery(uint32 deviceIdx = 0) const override;

      /**
       * @copydoc QueryManager::createTimerQuery
       */
      SPtr<TimerQuery>
      createTimerQuery(uint32 deviceIdx = 0) const override;

      /**
       * @copydoc QueryManager::createOcclusionQuery
       */
      SPtr<OcclusionQuery>
      createOcclusionQuery(bool binary, uint32 deviceIdx = 0) const override;
    };
  }
}
