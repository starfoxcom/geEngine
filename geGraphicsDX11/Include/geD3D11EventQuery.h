/*****************************************************************************/
/**
 * @file    geD3D11EventQuery.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2018/05/29
 * @brief   D3D11 implementation of an event query.
 *
 * D3D11 implementation of an event query.
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

#include <geEventQuery.h>

namespace geEngineSDK {
  namespace geCoreThread {
    /**
     * @copydoc EventQuery
     */
    class D3D11EventQuery final : public EventQuery
    {
     public:
      D3D11EventQuery(uint32 deviceIdx);
      ~D3D11EventQuery();

      /**
       * @copydoc EventQuery::begin
       */
      void
      begin(const SPtr<CommandBuffer>& cb = nullptr) override;

      /**
       * @copydoc EventQuery::isReady
       */
      bool
      isReady() const override;

     private:
      ID3D11Query* m_query;
      ID3D11DeviceContext* m_context;
    };
  }
}
