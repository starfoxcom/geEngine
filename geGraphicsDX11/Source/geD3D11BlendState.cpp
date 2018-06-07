/*****************************************************************************/
/**
 * @file    geD3D11BlendState.cpp
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2018/06/02
 * @brief   DirectX 11 implementation of a blend state.
 *
 * DirectX 11 implementation of a blend state.
 * Corresponds directly with a DX11 blend state object.
 *
 * @bug     No known bugs.
 */
/*****************************************************************************/

/*****************************************************************************/
/**
 * Includes
 */
/*****************************************************************************/
#include "geD3D11BlendState.h"

#include "geD3D11Mappings.h"
#include "geD3D11RenderAPI.h"
#include "geD3D11Device.h"
#include "geRenderStats.h"

namespace geEngineSDK {
  namespace geCoreThread {
    D3D11BlendState::D3D11BlendState(const BLEND_STATE_DESC& desc, uint32 id)
      : BlendState(desc, id),
        m_blendState(nullptr)
    {}

    D3D11BlendState::~D3D11BlendState() {
      SAFE_RELEASE(m_blendState);
      GE_INC_RENDER_STAT_CAT(ResDestroyed,
                             D3D11_RENDER_STAT_RESOURCE_TYPE::kBlendState);
    }

    void
    D3D11BlendState::createInternal() {
      D3D11_BLEND_DESC blendStateDesc;
      memset(&blendStateDesc, 0, sizeof(D3D11_BLEND_DESC));

      blendStateDesc.AlphaToCoverageEnable = m_properties.getAlphaToCoverageEnabled();
      blendStateDesc.IndependentBlendEnable = m_properties.getIndependantBlendEnable();

      for (uint32 i = 0; i < GE_MAX_MULTIPLE_RENDER_TARGETS; ++i) {
        D3D11_RENDER_TARGET_BLEND_DESC& bSD = blendStateDesc.RenderTarget[i];

        bSD.BlendEnable     = m_properties.getBlendEnabled(i);
        bSD.BlendOp         = D3D11Mappings::get(m_properties.getBlendOperation(i));
        bSD.BlendOpAlpha    = D3D11Mappings::get(m_properties.getAlphaBlendOperation(i));
        bSD.DestBlend       = D3D11Mappings::get(m_properties.getDstBlend(i));
        bSD.DestBlendAlpha  = D3D11Mappings::get(m_properties.getAlphaDstBlend(i));
        bSD.SrcBlend        = D3D11Mappings::get(m_properties.getSrcBlend(i));
        bSD.SrcBlendAlpha   = D3D11Mappings::get(m_properties.getAlphaSrcBlend(i));

        //Mask out all but last 4 bits
        bSD.RenderTargetWriteMask = 0xf & (m_properties.getRenderTargetWriteMask(i));        
      }

      auto rs = static_cast<D3D11RenderAPI*>(RenderAPI::instancePtr());
      D3D11Device& device = rs->getPrimaryDevice();
      HRESULT hr = device.getD3D11Device()->CreateBlendState(&blendStateDesc, &m_blendState);

      if (FAILED(hr) || device.hasError()) {
        String errorDescription = device.getErrorDescription();
        GE_EXCEPT(RenderingAPIException,
                  "Cannot create blend state.\nError Description:" +
                  errorDescription);
      }

      GE_INC_RENDER_STAT_CAT(ResCreated,
                             D3D11_RENDER_STAT_RESOURCE_TYPE::kBlendState);

      BlendState::createInternal();
    }
  }
}
