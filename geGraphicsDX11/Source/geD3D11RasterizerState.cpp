/*****************************************************************************/
/**
 * @file    geD3D11RasterizerState.cpp
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2018/06/17
 * @brief   DirectX 11 implementation of a rasterizer state.
 *
 * DirectX 11 implementation of a rasterizer state.
 * Wraps a DX11 rasterizer state object.
 *
 * @bug     No known bugs.
 */
/*****************************************************************************/

/*****************************************************************************/
/**
 * Includes
 */
/*****************************************************************************/
#include "geD3D11RasterizerState.h"
#include "geD3D11RenderAPI.h"
#include "geD3D11Device.h"
#include "geD3D11Mappings.h"

#include <geRenderStats.h>
#include <geMath.h>

namespace geEngineSDK {
  namespace geCoreThread {
    D3D11RasterizerState::D3D11RasterizerState(const RASTERIZER_STATE_DESC& desc,
                                               uint32 id)
      : RasterizerState(desc, id),
        m_rasterizerState(nullptr)
    {}

    D3D11RasterizerState::~D3D11RasterizerState() {
      SAFE_RELEASE(m_rasterizerState);
      GE_INC_RENDER_STAT_CAT(ResDestroyed,
                             D3D11_RENDER_STAT_RESOURCE_TYPE::kRasterizerState);
    }

    void
    D3D11RasterizerState::createInternal() {
      //NOTE: Assumes 24-bit depth buffer
      int32 scaledDepthBias = Math::floor(-m_properties.getDepthBias() * float((1 << 24)));

      D3D11_RASTERIZER_DESC rasterizerStateDesc;
      memset(&rasterizerStateDesc, 0, sizeof(D3D11_RASTERIZER_DESC));

      rasterizerStateDesc.AntialiasedLineEnable = m_properties.getAntialiasedLineEnable();
      rasterizerStateDesc.CullMode = D3D11Mappings::get(m_properties.getCullMode());
      rasterizerStateDesc.DepthBias = scaledDepthBias;
      rasterizerStateDesc.DepthBiasClamp = m_properties.getDepthBiasClamp();
      rasterizerStateDesc.DepthClipEnable = m_properties.getDepthClipEnable();
      rasterizerStateDesc.FillMode = D3D11Mappings::get(m_properties.getPolygonMode());
      rasterizerStateDesc.MultisampleEnable = m_properties.getMultisampleEnable();
      rasterizerStateDesc.ScissorEnable = m_properties.getScissorEnable();
      rasterizerStateDesc.SlopeScaledDepthBias = m_properties.getSlopeScaledDepthBias();
      rasterizerStateDesc.FrontCounterClockwise = false;

      auto rs = static_cast<D3D11RenderAPI*>(RenderAPI::instancePtr());
      D3D11Device& device = rs->getPrimaryDevice();
      HRESULT hr = device.getD3D11Device()->CreateRasterizerState(&rasterizerStateDesc,
                                                                  &m_rasterizerState);
      if (FAILED(hr) || device.hasError()) {
        String errorDescription = device.getErrorDescription();
        GE_EXCEPT(RenderingAPIException,
                  "Cannot create rasterizer state.\n"
                  "Error Description:" + errorDescription);
      }

      GE_INC_RENDER_STAT_CAT(ResCreated,
                             D3D11_RENDER_STAT_RESOURCE_TYPE::kRasterizerState);
      RasterizerState::createInternal();
    }
  }
}