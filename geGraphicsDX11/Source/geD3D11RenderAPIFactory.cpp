/*****************************************************************************/
/**
 * @file    geD3D11RenderAPIFactory.cpp
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2018/05/11
 * @brief   Handles creation of the DX11 render system.
 *
 * Contains a list of all available drivers.
 *
 * @bug     No known bugs.
 */
/*****************************************************************************/

/*****************************************************************************/
/**
 * Includes
 */
/*****************************************************************************/
#include "geD3D11RenderAPIFactory.h"

//#include <geRenderAPI.h>

namespace geEngineSDK {
  namespace geCoreThread {
    constexpr const char* D3D11RenderAPIFactory::SYSTEM_NAME;

    void
    D3D11RenderAPIFactory::create() {
      RenderAPI::startUp<D3D11RenderAPI>();
    }

    D3D11RenderAPIFactory::InitOnStart D3D11RenderAPIFactory::initOnStart;
  }
}
