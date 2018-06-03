/*****************************************************************************/
/**
 * @file    geD3D11OcclusionQuery.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2018/06/02
 * @brief   Query that counts number of samples rendered by the GPU while the
 *          query is active.
 *
 * Represents a query that counts number of samples rendered by the GPU while
 * the query is active.
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

#include <geOcclusionQuery.h>

namespace geEngineSDK {
  namespace geCoreThread {
    /**
     * @copydoc OcclusionQuery
     */
    class D3D11OcclusionQuery : public OcclusionQuery
    {
     public:
      D3D11OcclusionQuery(bool binary, uint32 deviceIdx);
      ~D3D11OcclusionQuery();

      /**
       * @copydoc OcclusionQuery::begin
       */
      void
      begin(const SPtr<CommandBuffer>& cb = nullptr) override;

      /**
       * @copydoc OcclusionQuery::end
       */
      void
      end(const SPtr<CommandBuffer>& cb = nullptr) override;

      /**
       * @copydoc OcclusionQuery::isReady
       */
      bool
      isReady() const override;

      /**
       * @copydoc OcclusionQuery::getNumSamples
       */
      uint32
      getNumSamples() override;

     private:
      friend class QueryManager;

      /**
       * Resolves query results after it is ready.
       */
      void
      finalize();

      ID3D11Query* m_query;
      ID3D11DeviceContext* m_context;
      bool m_finalized;
      bool m_queryEndCalled;
      uint32 m_numSamples;
    };
  }
}
