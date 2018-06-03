/*****************************************************************************/
/**
 * @file    geD3D11TimerQuery.cpp
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

/*****************************************************************************/
/**
 * Includes
 */
/*****************************************************************************/
#include "geD3D11TimerQuery.h"
#include "geD3D11RenderAPI.h"
#include "geD3D11Device.h"
#include "geD3D11CommandBuffer.h"

#include <geRenderStats.h>
#include <geDebug.h>

namespace geEngineSDK {
  using std::static_pointer_cast;

  namespace geCoreThread {
    D3D11TimerQuery::D3D11TimerQuery(uint32 deviceIdx)
      : m_finalized(false),
        m_context(nullptr),
        m_beginQuery(nullptr),
        m_endQuery(nullptr),
        m_disjointQuery(nullptr),
        m_timeDelta(0.0f),
        m_queryEndCalled(false) {
      GE_ASSERT(0 == deviceIdx &&
                "Multiple GPUs not supported natively on DirectX 11.");

      auto rs = static_cast<D3D11RenderAPI*>(RenderAPI::instancePtr());
      D3D11Device& device = rs->getPrimaryDevice();

      D3D11_QUERY_DESC queryDesc;
      queryDesc.Query = D3D11_QUERY_TIMESTAMP_DISJOINT;
      queryDesc.MiscFlags = 0;

      HRESULT hr = device.getD3D11Device()->CreateQuery(&queryDesc, &m_disjointQuery);
      if (S_OK != hr) {
        GE_EXCEPT(RenderingAPIException, "Failed to create a timer query.");
      }

      queryDesc.Query = D3D11_QUERY_TIMESTAMP;

      hr = device.getD3D11Device()->CreateQuery(&queryDesc, &m_beginQuery);
      if (S_OK != hr) {
        GE_EXCEPT(RenderingAPIException, "Failed to create a timer query.");
      }

      hr = device.getD3D11Device()->CreateQuery(&queryDesc, &m_endQuery);
      if (S_OK != hr) {
        GE_EXCEPT(RenderingAPIException, "Failed to create a timer query.");
      }

      m_context = device.getImmediateContext();
      GE_INC_RENDER_STAT_CAT(ResCreated, RENDER_STAT_RESOURCE_TYPE::kQuery);
    }

    D3D11TimerQuery::~D3D11TimerQuery() {
      if (nullptr != m_beginQuery) {
        m_beginQuery->Release();
      }

      if (nullptr != m_endQuery) {
        m_endQuery->Release();
      }

      if (nullptr != m_disjointQuery) {
        m_disjointQuery->Release();
      }

      GE_INC_RENDER_STAT_CAT(ResDestroyed, RENDER_STAT_RESOURCE_TYPE::kQuery);
    }

    void
    D3D11TimerQuery::begin(const SPtr<CommandBuffer>& cb) {
      auto execute = [&]() {
        m_context->Begin(m_disjointQuery);
        m_context->End(m_beginQuery);
        m_queryEndCalled = false;
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

    void
    D3D11TimerQuery::end(const SPtr<CommandBuffer>& cb) {
      auto execute = [&]() {
        m_context->End(m_endQuery);
        m_context->End(m_disjointQuery);
        m_queryEndCalled = true;
        m_finalized = false;
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
    D3D11TimerQuery::isReady() const {
      if (!m_queryEndCalled) {
        return false;
      }

      D3D11_QUERY_DATA_TIMESTAMP_DISJOINT disjointData;
      return m_context->GetData(m_disjointQuery,
                                &disjointData,
                                sizeof(disjointData),
                                0) == S_OK;
    }

    float
    D3D11TimerQuery::getTimeMs() {
      if (!m_finalized && isReady()) {
        finalize();
      }

      return m_timeDelta;
    }

    void
    D3D11TimerQuery::finalize() {
      uint64 timeStart, timeEnd;

      m_context->GetData(m_beginQuery, &timeStart, sizeof(timeStart), 0);
      m_context->GetData(m_endQuery, &timeEnd, sizeof(timeEnd), 0);

      D3D11_QUERY_DATA_TIMESTAMP_DISJOINT disjointData;
      m_context->GetData(m_disjointQuery, &disjointData, sizeof(disjointData), 0);

      if (FALSE == disjointData.Disjoint) {
        float frequency = static_cast<float>(disjointData.Frequency);

        uint64 delta = timeEnd - timeStart;
        m_timeDelta = (delta / static_cast<float>(frequency)) * 1000.0f;
      }
      else {
        LOGWRN_VERBOSE("Unrealiable GPU timer query detected.");
      }
    }
  }
}
