/*****************************************************************************/
/**
 * @file    geD3D11TextureManager.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2018/06/13
 * @brief   Handles creation of DirectX 11 textures.
 *
 * Handles creation of DirectX 11 textures.
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

#include <geTextureManager.h>

namespace geEngineSDK {
  class D3D11TextureManager : public TextureManager
  {
   public:
    /**
     * @copydoc TextureManager::getNativeFormat
     */
    PixelFormat
    getNativeFormat(TextureType ttype,
                    PixelFormat format,
                    int32 usage,
                    bool hwGamma) override;

   protected:
    /**
     * @copydoc TextureManager::createRenderTextureImpl
     */
    SPtr<RenderTexture>
    createRenderTextureImpl(const RENDER_TEXTURE_DESC& desc) override;
  };

  namespace geCoreThread {
    /**
     * @brief Handles creation of DirectX 11 textures.
     */
    class D3D11TextureManager : public TextureManager
    {
     protected:
      /**
       * @copydoc TextureManager::createTextureInternal
       */
      SPtr<Texture>
      createTextureInternal(const TEXTURE_DESC& desc,
                            const SPtr<PixelData>& initialData = nullptr,
                            GPU_DEVICE_FLAGS::E deviceMask =
                              GPU_DEVICE_FLAGS::kDEFAULT) override;

      /**
       * @copydoc TextureManager::createRenderTextureInternal
       */
      SPtr<RenderTexture>
      createRenderTextureInternal(const RENDER_TEXTURE_DESC& desc,
                                  uint32 deviceIdx = 0) override;
    };
  }
}
