/*****************************************************************************/
/**
 * @file    geD3D11VideoModeInfo.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2018/05/11
 * @brief   Provides information about a driver.
 *
 * Provides information about a driver (for example hardware GPU driver or
 * software emulated).
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

#include <geVideoModeInfo.h>

namespace geEngineSDK {
  namespace geCoreThread {
    class D3D11VideoMode final : public VideoMode
    {
     public:
      D3D11VideoMode(uint32 width,
                     uint32 height,
                     float refreshRate,
                     uint32 outputIdx,
                     uint32 refreshRateNumerator,
                     uint32 refreshRateDenominator,
                     DXGI_MODE_DESC mode);

      /**
       * @brief Returns an internal DXGI representation of this video mode.
       */
      const DXGI_MODE_DESC&
      getDXGIModeDesc() const {
        return m_d3d11Mode;
      }

      /**
       * @brief Gets internal DX11 refresh rate numerator.
       */
      uint32
      getRefreshRateNumerator() const {
        return m_refreshRateNumerator;
      }

      /**
       * @brief Gets internal DX11 refresh rate denominator.
       */
      uint32
      getRefreshRateDenominator() const {
        return m_refreshRateDenominator;
      }

     private:
      friend class D3D11VideoOutputInfo;

      uint32 m_refreshRateNumerator;
      uint32 m_refreshRateDenominator;
      DXGI_MODE_DESC m_d3d11Mode;
    };

    /**
     * @copydoc VideoOutputInfo
     */
    class D3D11VideoOutputInfo final : public VideoOutputInfo
    {
     public:
      D3D11VideoOutputInfo(IDXGIOutput* output, uint32 outputIdx);
      ~D3D11VideoOutputInfo();

      /**
       * @brief Returns the internal DXGI object representing an output device.
       */
      IDXGIOutput*
      getDXGIOutput() const {
        return m_dxgiOutput;
      }

     private:
      IDXGIOutput* m_dxgiOutput;
    };

    /**
     * @copydoc VideoModeInfo
     */
    class D3D11VideoModeInfo final : public VideoModeInfo
    {
     public:
      D3D11VideoModeInfo(IDXGIAdapter* dxgiAdapter);
    };
  }
}