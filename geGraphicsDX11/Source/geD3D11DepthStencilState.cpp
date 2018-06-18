/*****************************************************************************/
/**
 * @file    geD3D11DepthStencilState.cpp
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2018/06/17
 * @brief   DirectX 11 implementation of a depth stencil state.
 *
 * DirectX 11 implementation of a depth stencil state.
 * Corresponds directly with a DX11 depth stencil state object.
 *
 * @bug     No known bugs.
 */
/*****************************************************************************/

/*****************************************************************************/
/**
 * Includes
 */
/*****************************************************************************/
#include "geD3D11DepthStencilState.h"
#include "geD3D11Device.h"
#include "geD3D11RenderAPI.h"
#include "geD3D11Mappings.h"

#include <geRenderStats.h>

namespace geEngineSDK {
  namespace geCoreThread {
    D3D11DepthStencilState::D3D11DepthStencilState(const DEPTH_STENCIL_STATE_DESC& desc,
                                                   uint32 id)
      : DepthStencilState(desc, id),
        m_depthStencilState(nullptr)
    {}

    D3D11DepthStencilState::~D3D11DepthStencilState() {
      SAFE_RELEASE(m_depthStencilState);
      GE_INC_RENDER_STAT_CAT(ResDestroyed,
                             D3D11_RENDER_STAT_RESOURCE_TYPE::kDepthStencilState);
    }

    void
    D3D11DepthStencilState::createInternal() {
      D3D11_DEPTH_STENCIL_DESC desc;
      memset(&desc, 0, sizeof(D3D11_DEPTH_STENCIL_DESC));

      bool depthEnable = m_properties.getDepthWriteEnable() ||
                         m_properties.getDepthReadEnable();
      COMPARE_FUNCTION::E compareFunc;
      if (m_properties.getDepthReadEnable()) {
        compareFunc = m_properties.getDepthComparisonFunc();
      }
      else {
        compareFunc = COMPARE_FUNCTION::kALWAYS_PASS;
      }

      desc.BackFace.StencilPassOp =
        D3D11Mappings::get(m_properties.getStencilBackPassOp());
      desc.BackFace.StencilFailOp =
        D3D11Mappings::get(m_properties.getStencilBackFailOp());
      desc.BackFace.StencilDepthFailOp =
        D3D11Mappings::get(m_properties.getStencilBackZFailOp());
      desc.BackFace.StencilFunc =
        D3D11Mappings::get(m_properties.getStencilBackCompFunc());
      desc.FrontFace.StencilPassOp =
        D3D11Mappings::get(m_properties.getStencilFrontPassOp());
      desc.FrontFace.StencilFailOp =
        D3D11Mappings::get(m_properties.getStencilFrontFailOp());
      desc.FrontFace.StencilDepthFailOp =
        D3D11Mappings::get(m_properties.getStencilFrontZFailOp());
      desc.FrontFace.StencilFunc =
        D3D11Mappings::get(m_properties.getStencilFrontCompFunc());
      desc.DepthEnable = depthEnable;
      desc.DepthWriteMask =
        m_properties.getDepthWriteEnable() ? D3D11_DEPTH_WRITE_MASK_ALL :
                                             D3D11_DEPTH_WRITE_MASK_ZERO;
      desc.DepthFunc = D3D11Mappings::get(compareFunc);
      desc.StencilEnable = m_properties.getStencilEnable();
      desc.StencilReadMask = m_properties.getStencilReadMask();
      desc.StencilWriteMask = m_properties.getStencilWriteMask();

      auto rs = static_cast<D3D11RenderAPI*>(RenderAPI::instancePtr());
      D3D11Device& device = rs->getPrimaryDevice();
      HRESULT hr = device.getD3D11Device()->CreateDepthStencilState(&desc,
                                                                    &m_depthStencilState);

      if (FAILED(hr) || device.hasError()) {
        String errorDescription = device.getErrorDescription();
        GE_EXCEPT(RenderingAPIException,
                  "Cannot create depth stencil state.\n"
                  "Error Description:" + errorDescription);
      }

      GE_INC_RENDER_STAT_CAT(ResCreated,
                             D3D11_RENDER_STAT_RESOURCE_TYPE::kDepthStencilState);

      DepthStencilState::createInternal();
    }
  }
}
