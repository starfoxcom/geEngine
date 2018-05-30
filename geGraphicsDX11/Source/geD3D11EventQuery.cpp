/*****************************************************************************/
/**
 * @file    geD3D11EventQuery.cpp
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2018/05/29
 * @brief   D3D11 implementation of an event query.
 *
 * D3D11 implementation of an event query.
 *
 * @bug     No known bugs.
 */
/*****************************************************************************/

/*****************************************************************************/
/**
 * Includes
 */
/*****************************************************************************/
#include "geD3D11EventQuery.h"
#include "geD3D11RenderAPI.h"
#include "geD3D11Device.h"
#include "geD3D11CommandBuffer.h"

#include <geRenderStats.h>
#include <geException.h>

namespace geEngineSDK {
  using std::static_pointer_cast;

  namespace geCoreThread {
    D3D11EventQuery::D3D11EventQuery(uint32 deviceIdx)
      : m_query(nullptr) {
      GE_ASSERT(0 == deviceIdx &&
                "Multiple GPUs not supported natively on DirectX 11.");

      auto rs = static_cast<D3D11RenderAPI*>(RenderAPI::instancePtr());
      D3D11Device& device = rs->getPrimaryDevice();

      D3D11_QUERY_DESC queryDesc;
      queryDesc.Query = D3D11_QUERY_EVENT;
      queryDesc.MiscFlags = 0;

      HRESULT hr = device.getD3D11Device()->CreateQuery(&queryDesc, &m_query);
      if (S_OK != hr) {
        GE_EXCEPT(RenderingAPIException, "Failed to create an Event query.");
      }

      m_context = device.getImmediateContext();

      GE_INC_RENDER_STAT_CAT(ResCreated, RENDER_STAT_RESOURCE_TYPE::kQuery);
    }

    D3D11EventQuery::~D3D11EventQuery() {
      if (nullptr != m_query) {
        m_query->Release();
      }

      GE_INC_RENDER_STAT_CAT(ResDestroyed, RENDER_STAT_RESOURCE_TYPE::kQuery);
    }

    void
    D3D11EventQuery::begin(const SPtr<CommandBuffer>& cb) {
      auto execute = [&]() {
        m_context->End(m_query);
        setActive(true);
      };

      if (nullptr == cb) {
        execute();
      }
      else {
        SPtr<D3D11CommandBuffer> d3d11cb = static_pointer_cast<D3D11CommandBuffer>(cb);
        d3d11cb->queueCommand(execute);
      }
    }

    bool
    D3D11EventQuery::isReady() const {
      BOOL queryData;
      return m_context->GetData(m_query, &queryData, sizeof(BOOL), 0) == S_OK;
    }
  }
}
