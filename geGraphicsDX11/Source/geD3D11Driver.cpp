/*****************************************************************************/
/**
 * @file    geD3D11Driver.cpp
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

/*****************************************************************************/
/**
 * Includes
 */
/*****************************************************************************/
#include "geD3D11Driver.h"
#include "geD3D11VideoModeInfo.h"

#include <geException.h>

namespace geEngineSDK {
  namespace geCoreThread {
    D3D11Driver::D3D11Driver(const D3D11Driver &ob) {
      m_adapterNumber = ob.m_adapterNumber;
      m_adapterIdentifier = ob.m_adapterIdentifier;
      m_dxgiAdapter = ob.m_dxgiAdapter;

      if (m_dxgiAdapter) {
        m_dxgiAdapter->AddRef();
      }

      construct();
    }

    D3D11Driver::D3D11Driver(uint32 adapterNumber, IDXGIAdapter* pDXGIAdapter) {
      m_adapterNumber = adapterNumber;
      m_dxgiAdapter = pDXGIAdapter;

      if (m_dxgiAdapter) {
        m_dxgiAdapter->AddRef();
      }

      pDXGIAdapter->GetDesc(&m_adapterIdentifier);
      construct();
    }

    D3D11Driver::~D3D11Driver() {
      SAFE_RELEASE(m_dxgiAdapter);
    }

    void
    D3D11Driver::construct() {
      GE_ASSERT(nullptr != m_dxgiAdapter);

      uint32 outputIdx = 0;
      IDXGIOutput* output = nullptr;
      while (m_dxgiAdapter->EnumOutputs(outputIdx, &output) != DXGI_ERROR_NOT_FOUND) {
        ++outputIdx;
        SAFE_RELEASE(output);
      }

      m_numOutputs = outputIdx;
      m_videoModeInfo = ge_shared_ptr_new<D3D11VideoModeInfo>(m_dxgiAdapter);
    }

    D3D11Driver&
    D3D11Driver::operator=(const D3D11Driver& ob) {
      m_adapterNumber = ob.m_adapterNumber;
      m_adapterIdentifier = ob.m_adapterIdentifier;

      if (ob.m_dxgiAdapter) {
        ob.m_dxgiAdapter->AddRef();
      }
      
      SAFE_RELEASE(m_dxgiAdapter);
      m_dxgiAdapter = ob.m_dxgiAdapter;
      return *this;
    }

    String
    D3D11Driver::getDriverName() const {
      SIZE_T size = wcslen(m_adapterIdentifier.Description);
      char* str = (char*)ge_alloc((uint32)(size + 1));

      wcstombs(str, m_adapterIdentifier.Description, size);
      str[size] = '\0';
      String Description = str;

      ge_free(str);
      return String(Description);
    }

    String D3D11Driver::getDriverDescription() const
    {
      SIZE_T size = wcslen(m_adapterIdentifier.Description);
      ANSICHAR* str = reinterpret_cast<ANSICHAR*>(ge_alloc(size + 1));

      wcstombs(str, m_adapterIdentifier.Description, size);
      str[size] = '\0';
      String driverDescription = str;

      ge_free(str);
      StringUtil::trim(driverDescription);

      return driverDescription;
    }

    DXGI_OUTPUT_DESC
    D3D11Driver::getOutputDesc(uint32 adapterOutputIdx) const {
      DXGI_OUTPUT_DESC desc;

      IDXGIOutput* output = nullptr;
      if (m_dxgiAdapter->EnumOutputs(adapterOutputIdx, &output) == DXGI_ERROR_NOT_FOUND) {
        GE_EXCEPT(InvalidParametersException,
                  "Cannot find output with the specified index: " +
                  toString(adapterOutputIdx));
      }

      output->GetDesc(&desc);
      SAFE_RELEASE(output);
      return desc;
    }
  }
}
