/*****************************************************************************/
/**
 * @file    geD3D11RenderTexture.cpp
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2018/06/12
 * @brief   DirectX 11 implementation of a render texture.
 *
 * DirectX 11 implementation of a render texture.
 *
 * @bug     No known bugs.
 */
/*****************************************************************************/

/*****************************************************************************/
/**
 * Includes
 */
/*****************************************************************************/
#include "geD3D11RenderTexture.h"
#include "geD3D11TextureView.h"

namespace geEngineSDK {
  D3D11RenderTexture::D3D11RenderTexture(const RENDER_TEXTURE_DESC& desc)
    : RenderTexture(desc),
      mProperties(desc, false)
  {}

  namespace geCoreThread {
    D3D11RenderTexture::D3D11RenderTexture(const RENDER_TEXTURE_DESC& desc,
                                           uint32 deviceIdx)
      : RenderTexture(desc, deviceIdx),
        mProperties(desc, false) {
      GE_ASSERT(0 == deviceIdx &&
                "Multiple GPUs not supported natively on DirectX 11.");
    }

    void
    D3D11RenderTexture::getCustomAttribute(const String& name,
                                           void* data) const {
      if ("RTV" == name) {
        auto rtvs = reinterpret_cast<ID3D11RenderTargetView**>(data);
        for (uint32 i = 0; i < GE_MAX_MULTIPLE_RENDER_TARGETS; ++i) {
          if (nullptr == m_colorSurfaces[i]) {
            continue;
          }

          auto textureView = static_cast<D3D11TextureView*>(m_colorSurfaces[i].get());
          rtvs[i] = textureView->getRTV();
        }
      }
      else if ("DSV" == name) {
        if (nullptr == m_depthStencilSurface) {
          return;
        }

        auto dsv = reinterpret_cast<ID3D11DepthStencilView**>(data);
        auto depthStencilView = static_cast<D3D11TextureView*>(m_depthStencilSurface.get());

        *dsv = depthStencilView->getDSV(false, false);
      }
      else if ("RODSV" == name) {
        if (nullptr == m_depthStencilSurface) {
          return;
        }

        auto dsv = reinterpret_cast<ID3D11DepthStencilView**>(data);
        auto depthStencilView = static_cast<D3D11TextureView*>(m_depthStencilSurface.get());

        *dsv = depthStencilView->getDSV(true, true);
      }
      else if ("RODWSV" == name) {
        if (nullptr == m_depthStencilSurface) {
          return;
        }

        auto dsv = reinterpret_cast<ID3D11DepthStencilView**>(data);
        auto depthStencilView = static_cast<D3D11TextureView*>(m_depthStencilSurface.get());

        *dsv = depthStencilView->getDSV(true, false);
      }
      else if ("WDROSV" == name) {
        if (nullptr == m_depthStencilSurface) {
          return;
        }

        auto dsv = reinterpret_cast<ID3D11DepthStencilView**>(data);
        auto depthStencilView = static_cast<D3D11TextureView*>(m_depthStencilSurface.get());

        *dsv = depthStencilView->getDSV(false, true);
      }
    }
  }
}
