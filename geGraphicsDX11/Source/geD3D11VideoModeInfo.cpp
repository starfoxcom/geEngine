/*****************************************************************************/
/**
 * @file    geD3D11VideoModeInfo.cpp
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
#include "geD3D11VideoModeInfo.h"

#include <geException.h>
#include <geUnicode.h>

namespace geEngineSDK {
  namespace geCoreThread {
    D3D11VideoModeInfo::D3D11VideoModeInfo(IDXGIAdapter* dxgiAdapter) {
      uint32 outputIdx = 0;
      IDXGIOutput* output = nullptr;
      while (dxgiAdapter->EnumOutputs(outputIdx, &output) != DXGI_ERROR_NOT_FOUND) {
        m_outputs.push_back(ge_new<D3D11VideoOutputInfo>(output, outputIdx));
        ++outputIdx;
      }
    }

    D3D11VideoOutputInfo::D3D11VideoOutputInfo(IDXGIOutput* output, uint32 outputIdx)
      : m_dxgiOutput(output) {
      DXGI_OUTPUT_DESC outputDesc;
      output->GetDesc(&outputDesc);
      m_name = UTF8::fromWide(WString(outputDesc.DeviceName));

      uint32 numModes = 0;

      HRESULT hr = output->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM,
                                              0,
                                              &numModes,
                                              nullptr);
      if (FAILED(hr)) {
        SAFE_RELEASE(output);
        GE_EXCEPT(InternalErrorException,
                  "Error while enumerating adapter output video modes.");
      }

      DXGI_MODE_DESC* modeDesc = ge_newN<DXGI_MODE_DESC>(numModes);

      hr = output->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, 0, &numModes, modeDesc);
      if (FAILED(hr)) {
        ge_deleteN(modeDesc, numModes);
        SAFE_RELEASE(output);
        GE_EXCEPT(InternalErrorException,
                  "Error while enumerating adapter output video modes.");
      }

      for (uint32 i = 0; i < numModes; ++i) {
        DXGI_MODE_DESC displayMode = modeDesc[i];
        bool foundVideoMode = false;
        for (auto videoMode : m_videoModes) {
          D3D11VideoMode* d3d11videoMode = static_cast<D3D11VideoMode*>(videoMode);

          if (d3d11videoMode->m_width == displayMode.Width &&
              d3d11videoMode->m_height == displayMode.Height &&
              d3d11videoMode->m_refreshRateNumerator == displayMode.RefreshRate.Numerator &&
              d3d11videoMode->m_refreshRateDenominator == displayMode.RefreshRate.Denominator)
          {
            foundVideoMode = true;
            break;
          }
        }

        if (!foundVideoMode) {
          float refreshRate = displayMode.RefreshRate.Numerator /
                              static_cast<float>(displayMode.RefreshRate.Denominator);
          D3D11VideoMode*
            videoMode = ge_new<D3D11VideoMode>(displayMode.Width,
                                               displayMode.Height,
                                               refreshRate,
                                               outputIdx,
                                               displayMode.RefreshRate.Numerator,
                                               displayMode.RefreshRate.Denominator,
                                               displayMode);

          m_videoModes.push_back(videoMode);
        }
      }

      ge_deleteN(modeDesc, numModes);

      //Get desktop display mode
      HMONITOR hMonitor = outputDesc.Monitor;
      MONITORINFOEX monitorInfo;
      monitorInfo.cbSize = sizeof(MONITORINFOEX);
      GetMonitorInfo(hMonitor, &monitorInfo);

      DEVMODE devMode;
      devMode.dmSize = sizeof(DEVMODE);
      devMode.dmDriverExtra = 0;
      EnumDisplaySettings(monitorInfo.szDevice, ENUM_CURRENT_SETTINGS, &devMode);

      DXGI_MODE_DESC currentMode;
      currentMode.Width = devMode.dmPelsWidth;
      currentMode.Height = devMode.dmPelsHeight;
      bool useDefaultRefreshRate = 1 == devMode.dmDisplayFrequency ||
                                   0 == devMode.dmDisplayFrequency;
      currentMode.RefreshRate.Numerator = useDefaultRefreshRate ?
                                            0 : devMode.dmDisplayFrequency;
      currentMode.RefreshRate.Denominator = useDefaultRefreshRate ? 0 : 1;
      currentMode.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
      currentMode.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
      currentMode.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

      DXGI_MODE_DESC nearestMode;
      memset(&nearestMode, 0, sizeof(nearestMode));

      output->FindClosestMatchingMode(&currentMode, &nearestMode, nullptr);

      float refreshRate = nearestMode.RefreshRate.Numerator /
                          static_cast<float>(nearestMode.RefreshRate.Denominator);
      m_desktopVideoMode = ge_new<D3D11VideoMode>(nearestMode.Width,
                                                  nearestMode.Height,
                                                  refreshRate,
                                                  outputIdx,
                                                  nearestMode.RefreshRate.Numerator,
                                                  nearestMode.RefreshRate.Denominator,
                                                  nearestMode);
    }

    D3D11VideoOutputInfo::~D3D11VideoOutputInfo() {
      SAFE_RELEASE(m_dxgiOutput);
    }

    D3D11VideoMode::D3D11VideoMode(uint32 width,
                                   uint32 height,
                                   float refreshRate,
                                   uint32 outputIdx,
                                   uint32 refreshRateNumerator,
                                   uint32 refreshRateDenominator,
                                   DXGI_MODE_DESC mode)
      : VideoMode(width, height, refreshRate, outputIdx),
        m_refreshRateNumerator(refreshRateNumerator),
        m_refreshRateDenominator(refreshRateDenominator),
        m_d3d11Mode(mode)
    {}
  }
}
