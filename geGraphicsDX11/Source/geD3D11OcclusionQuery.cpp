/*****************************************************************************/
/**
 * @file    geD3D11OcclusionQuery.cpp
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

/*****************************************************************************/
/**
 * Includes
 */
/*****************************************************************************/
#include "geD3D11OcclusionQuery.h"
#include "geD3D11RenderAPI.h"
#include "geD3D11Device.h"
#include "geD3D11CommandBuffer.h"

#include <geRenderStats.h>
#include <geMath.h>

namespace geEngineSDK {
  using std::static_pointer_cast;

  namespace geCoreThread {
    D3D11OcclusionQuery::D3D11OcclusionQuery(bool binary, uint32 deviceIdx)
      : OcclusionQuery(binary),
        m_context(nullptr),
        m_query(nullptr),
        m_numSamples(0),
        m_finalized(false),
        m_queryEndCalled(false) {
      GE_ASSERT(0 == deviceIdx &&
                "Multiple GPUs not supported natively on DirectX 11.");

      auto rs = static_cast<D3D11RenderAPI*>(RenderAPI::instancePtr());
      D3D11Device& device = rs->getPrimaryDevice();

      D3D11_QUERY_DESC queryDesc;
      queryDesc.Query = m_binary ? D3D11_QUERY_OCCLUSION_PREDICATE :
                                   D3D11_QUERY_OCCLUSION;
      queryDesc.MiscFlags = 0;

      HRESULT hr = device.getD3D11Device()->CreateQuery(&queryDesc, &m_query);
      if (S_OK != hr) {
        GE_EXCEPT(RenderingAPIException,
                  "Failed to create an occlusion query.");
      }

      m_context = device.getImmediateContext();
      GE_INC_RENDER_STAT_CAT(ResCreated, RENDER_STAT_RESOURCE_TYPE::kQuery);
    }

    D3D11OcclusionQuery::~D3D11OcclusionQuery() {
      if (nullptr != m_query) {
        m_query->Release();
      }
      GE_INC_RENDER_STAT_CAT(ResDestroyed, RENDER_STAT_RESOURCE_TYPE::kQuery);
    }

    void
    D3D11OcclusionQuery::begin(const SPtr<CommandBuffer>& cb) {
      auto execute = [&]() {
        m_context->Begin(m_query);
        m_numSamples = 0;
        m_queryEndCalled = false;
        setActive(true);
      };

      if (nullptr == cb) {
        execute();
      }
      else {
        SPtr<D3D11CommandBuffer> d3d11CB = std::static_pointer_cast<D3D11CommandBuffer>(cb);
        d3d11CB->queueCommand(execute);
      }
    }

    void
    D3D11OcclusionQuery::end(const SPtr<CommandBuffer>& cb) {
      auto execute = [&]() {
        m_context->End(m_query);
        m_queryEndCalled = true;
        m_finalized = false;
      };

      if (nullptr == cb) {
        execute();
      }
      else {
        SPtr<D3D11CommandBuffer> d3d11CB = static_pointer_cast<D3D11CommandBuffer>(cb);
        d3d11CB->queueCommand(execute);
      }
    }

    bool
    D3D11OcclusionQuery::isReady() const {
      if (!m_queryEndCalled) {
        return false;
      }

      if (m_binary) {
        BOOL anySamples = FALSE;
        return m_context->GetData(m_query, &anySamples, sizeof(anySamples), 0) == S_OK;
      }
      else {
        uint64 numSamples = 0;
        return m_context->GetData(m_query, &numSamples, sizeof(numSamples), 0) == S_OK;
      }
    }

    uint32
    D3D11OcclusionQuery::getNumSamples() {
      if (!m_finalized && isReady()) {
        finalize();
      }
      return m_numSamples;
    }

    void
    D3D11OcclusionQuery::finalize() {
      m_finalized = true;

      if (m_binary) {
        BOOL anySamples = FALSE;
        m_context->GetData(m_query, &anySamples, sizeof(anySamples), 0);
        m_numSamples = anySamples == TRUE ? 1 : 0;
      }
      else {
        uint64 numSamples = 0;
        m_context->GetData(m_query, &numSamples, sizeof(numSamples), 0);
        m_numSamples = static_cast<uint32>(numSamples);
      }
    }
  }
}
