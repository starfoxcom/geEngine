/*****************************************************************************/
/**
 * @file    geD3D11TimerQuery.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2018/06/02
 * @brief   GPU query that measures execution time of GPU operations.
 *
 * Represents a GPU query that measures execution time of GPU operations.
 * The query will measure any GPU operations that take place between its
 * begin() and end() calls.
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

#include <geTimerQuery.h>

namespace geEngineSDK {
  namespace geCoreThread {
    /**
     * @copydoc TimerQuery
     */
    class D3D11TimerQuery : public TimerQuery
    {
     public:
      D3D11TimerQuery(uint32 deviceIdx);
      ~D3D11TimerQuery();

      /**
       * @copydoc TimerQuery::begin
       */
      void
      begin(const SPtr<CommandBuffer>& cb = nullptr) override;

      /**
       * @copydoc TimerQuery::end
       */
      void
      end(const SPtr<CommandBuffer>& cb = nullptr) override;

      /**
       * @copydoc TimerQuery::isReady
       */
      bool
      isReady() const override;

      /**
       * @copydoc TimerQuery::getTimeMs
       */
      float
      getTimeMs() override;

     private:
      /**
       * Resolve timing information after the query has finished.
       */
      void
      finalize();

      bool m_finalized;
      bool m_queryEndCalled;
      float m_timeDelta;

      ID3D11Query* m_beginQuery;
      ID3D11Query* m_endQuery;
      ID3D11Query* m_disjointQuery;
      ID3D11DeviceContext* m_context;
    };
  }
}
