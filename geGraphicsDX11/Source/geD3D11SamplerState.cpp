/*****************************************************************************/
/**
 * @file    geD3D11SamplerState.cpp
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2018/06/17
 * @brief   DirectX 11 implementation of a sampler state.
 *
 * DirectX 11 implementation of a sampler state.
 * Wraps a DX11 sampler state object.
 *
 * @bug     No known bugs.
 */
/*****************************************************************************/

/*****************************************************************************/
/**
 * Includes
 */
/*****************************************************************************/
#include "geD3D11SamplerState.h"
#include "geD3D11RenderAPI.h"
#include "geD3D11Device.h"
#include "geD3D11Mappings.h"

#include <geRenderStats.h>

namespace geEngineSDK {
  namespace geCoreThread {
    D3D11SamplerState::D3D11SamplerState(const SAMPLER_STATE_DESC& desc,
                                         GPU_DEVICE_FLAGS::E deviceMask)
      : SamplerState(desc, deviceMask),
        m_samplerState(nullptr)
    {}

    D3D11SamplerState::~D3D11SamplerState() {
      SAFE_RELEASE(m_samplerState);
      GE_INC_RENDER_STAT_CAT(ResDestroyed, D3D11_RENDER_STAT_RESOURCE_TYPE::kSamplerState);
    }

    void
    D3D11SamplerState::createInternal() {
      D3D11_SAMPLER_DESC samplerState;
      memset(&samplerState, 0, sizeof(D3D11_SAMPLER_DESC));

      samplerState.AddressU = D3D11Mappings::get(m_properties.getTextureAddressingMode().u);
      samplerState.AddressV = D3D11Mappings::get(m_properties.getTextureAddressingMode().v);
      samplerState.AddressW = D3D11Mappings::get(m_properties.getTextureAddressingMode().w);
      samplerState.BorderColor[0] = m_properties.getBorderColor()[0];
      samplerState.BorderColor[1] = m_properties.getBorderColor()[1];
      samplerState.BorderColor[2] = m_properties.getBorderColor()[2];
      samplerState.BorderColor[3] = m_properties.getBorderColor()[3];
      samplerState.ComparisonFunc = D3D11Mappings::get(m_properties.getComparisonFunction());
      samplerState.MaxAnisotropy = m_properties.getTextureAnisotropy();
      samplerState.MaxLOD = m_properties.getMaximumMip();
      samplerState.MinLOD = m_properties.getMinimumMip();
      samplerState.MipLODBias = m_properties.getTextureMipmapBias();

      FILTER_OPTIONS::E minFilter = m_properties.getTextureFiltering(FILTER_TYPE::kMIN);
      FILTER_OPTIONS::E magFilter = m_properties.getTextureFiltering(FILTER_TYPE::kMAG);
      FILTER_OPTIONS::E mipFilter = m_properties.getTextureFiltering(FILTER_TYPE::kMIP);

      if (FILTER_OPTIONS::kANISOTROPIC == minFilter &&
          FILTER_OPTIONS::kANISOTROPIC == magFilter &&
          FILTER_OPTIONS::kANISOTROPIC == mipFilter) {
        samplerState.Filter = D3D11_FILTER_ANISOTROPIC;
      }
      else {
        if (FILTER_OPTIONS::kPOINT == minFilter ||
            FILTER_OPTIONS::kNONE == minFilter) {
          if (FILTER_OPTIONS::kPOINT == magFilter ||
              FILTER_OPTIONS::kNONE == magFilter) {
            if (FILTER_OPTIONS::kPOINT == mipFilter ||
                FILTER_OPTIONS::kNONE == mipFilter) {
              samplerState.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
            }
            else if (FILTER_OPTIONS::kLINEAR == mipFilter) {
              samplerState.Filter = D3D11_FILTER_MIN_MAG_POINT_MIP_LINEAR;
            }
          }
          else if (FILTER_OPTIONS::kLINEAR == magFilter) {
            if (FILTER_OPTIONS::kPOINT == mipFilter ||
                FILTER_OPTIONS::kNONE == mipFilter) {
              samplerState.Filter = D3D11_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT;
            }
            else if (FILTER_OPTIONS::kLINEAR == mipFilter) {
              samplerState.Filter = D3D11_FILTER_MIN_POINT_MAG_MIP_LINEAR;
            }
          }
        }
        else if (FILTER_OPTIONS::kLINEAR == minFilter) {
          if (FILTER_OPTIONS::kPOINT == magFilter ||
              FILTER_OPTIONS::kNONE == magFilter) {
            if (FILTER_OPTIONS::kPOINT == mipFilter ||
                FILTER_OPTIONS::kNONE == mipFilter) {
              samplerState.Filter = D3D11_FILTER_MIN_LINEAR_MAG_MIP_POINT;
            }
            else if (mipFilter == FILTER_OPTIONS::kLINEAR) {
              samplerState.Filter = D3D11_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR;
            }
          }
          else if (FILTER_OPTIONS::kLINEAR == magFilter) {
            if (FILTER_OPTIONS::kPOINT == mipFilter ||
                FILTER_OPTIONS::kNONE == mipFilter) {
              samplerState.Filter = D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;
            }
            else if (FILTER_OPTIONS::kLINEAR == mipFilter) {
              samplerState.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
            }
          }
        }
      }

      bool isComparison = m_properties.getComparisonFunction() !=
                            COMPARE_FUNCTION::kALWAYS_PASS;
      if (isComparison) {
        //Adds COMPARISON flag to the filter
        //See: http://msdn.microsoft.com/en-us/library/windows/desktop/ff476132(v=vs.85).aspx
        samplerState.Filter = static_cast<D3D11_FILTER>(0x80 | samplerState.Filter);
      }

      auto rs = static_cast<D3D11RenderAPI*>(RenderAPI::instancePtr());
      D3D11Device& device = rs->getPrimaryDevice();
      HRESULT hr = device.getD3D11Device()->CreateSamplerState(&samplerState,
                                                               &m_samplerState);
      if (FAILED(hr) || device.hasError()) {
        String errorDescription = device.getErrorDescription();
        GE_EXCEPT(RenderingAPIException,
                  "Cannot create sampler state.\n"
                  "Error Description:" + errorDescription);
      }

      GE_INC_RENDER_STAT_CAT(ResCreated,
                             D3D11_RENDER_STAT_RESOURCE_TYPE::kSamplerState);

      SamplerState::createInternal();
    }
  }
}
