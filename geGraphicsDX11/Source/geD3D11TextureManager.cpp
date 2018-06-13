/*****************************************************************************/
/**
 * @file    geD3D11TextureManager.cpp
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2018/06/13
 * @brief   Handles creation of DirectX 11 textures.
 *
 * Handles creation of DirectX 11 textures.
 *
 * @bug     No known bugs.
 */
/*****************************************************************************/

/*****************************************************************************/
/**
 * Includes
 */
/*****************************************************************************/
#include "geD3D11TextureManager.h"
#include "geD3D11Texture.h"
#include "geD3D11RenderTexture.h"
#include "geD3D11Mappings.h"
#include "geD3D11RenderAPI.h"

namespace geEngineSDK {
  SPtr<RenderTexture>
  D3D11TextureManager::createRenderTextureImpl(const RENDER_TEXTURE_DESC& desc) {
    auto tex = GE_PVT_NEW(D3D11RenderTexture, desc);
    return ge_core_ptr<D3D11RenderTexture>(tex);
  }

  PixelFormat
  D3D11TextureManager::getNativeFormat(TextureType ttype,
                                       PixelFormat format,
                                       int32 usage,
                                       bool hwGamma) {
    using shMappings = geCoreThread::D3D11Mappings;

    DXGI_FORMAT d3dPF = shMappings::getPF(
                                      shMappings::getClosestSupportedPF(format, ttype, usage),
                                      hwGamma);

    return shMappings::getPF(d3dPF);
  }

  namespace geCoreThread {
    SPtr<Texture>
    D3D11TextureManager::createTextureInternal(const TEXTURE_DESC& desc,
                                               const SPtr<PixelData>& initialData,
                                               GPU_DEVICE_FLAGS::E deviceMask) {
      auto tex = GE_PVT_NEW(D3D11Texture, desc, initialData, deviceMask);
      auto texPtr = ge_shared_ptr<D3D11Texture>(tex);
      texPtr->_setThisPtr(texPtr);

      return texPtr;
    }

    SPtr<RenderTexture>
    D3D11TextureManager::createRenderTextureInternal(const RENDER_TEXTURE_DESC& desc,
                                                     uint32 deviceIdx) {
      auto texPtr = ge_shared_ptr_new<D3D11RenderTexture>(desc, deviceIdx);
      texPtr->_setThisPtr(texPtr);

      return texPtr;
    }
  }
}
