/*****************************************************************************/
/**
 * @file    geD3D11Driver.h
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

namespace geEngineSDK {
  namespace geCoreThread {
    class D3D11Driver final
    {
     public:
      /**
       * @brief Constructs a new object from the adapter number provided by
       *        DX11 runtime, and DXGI adapter object.
       */
      D3D11Driver(uint32 adapterNumber, IDXGIAdapter* dxgiAdapter);
      D3D11Driver(const D3D11Driver &ob);
      ~D3D11Driver();

      D3D11Driver&
      operator=(const D3D11Driver& r);

      /**
       * @brief Returns the name of the driver.
       */
      String
      getDriverName() const;

      /**
       * @brief Returns the description of the driver.
       */
      String
      getDriverDescription() const;

      /**
       * @brief Returns adapter index of the adapter the driver is managing.
       */
      uint32
      getAdapterNumber() const {
        return m_adapterNumber;
      }

      /**
       * @brief Returns number of outputs connected to the adapter the driver is managing.
       */
      uint32
      getNumAdapterOutputs() const {
        return m_numOutputs;
      }

      /**
       * @brief Returns a description of the adapter the driver is managing.
       */
      const DXGI_ADAPTER_DESC&
      getAdapterIdentifier() const {
        return m_adapterIdentifier;
      }

      /**
       * @brief Returns internal DXGI adapter object for the driver.
       */
      IDXGIAdapter*
      getDeviceAdapter() const {
        return m_dxgiAdapter;
      }

      /**
       * @brief Returns description of an output device at the specified index.
       */
      DXGI_OUTPUT_DESC
      getOutputDesc(uint32 adapterOutputIdx) const;

      /**
       * @brief Returns a list of all available video modes for all output devices.
       */
      SPtr<VideoModeInfo>
      getVideoModeInfo() const {
        return m_videoModeInfo;
      }

     private:
      /**
       * @brief Initializes the internal data.
       */
      void
      construct();

     private:
      uint32 m_adapterNumber;
      uint32 m_numOutputs = 0;
      DXGI_ADAPTER_DESC m_adapterIdentifier;
      IDXGIAdapter* m_dxgiAdapter;
      SPtr<VideoModeInfo> m_videoModeInfo;
    };
  }
}
