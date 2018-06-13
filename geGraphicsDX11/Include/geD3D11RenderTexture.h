/*****************************************************************************/
/**
 * @file    geD3D11RenderTexture.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2018/06/12
 * @brief   DirectX 11 implementation of a render texture.
 *
 * DirectX 11 implementation of a render texture.
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

#include <geTexture.h>
#include <geRenderTexture.h>

namespace geEngineSDK {
  /**
   * @brief DirectX 11 implementation of a render texture.
   * @note  Sim thread only.
   */
  class D3D11RenderTexture : public RenderTexture
  {
   public:
    virtual ~D3D11RenderTexture() = default;

   protected:
    friend class D3D11TextureManager;

    D3D11RenderTexture(const RENDER_TEXTURE_DESC& desc);

    /**
     * @copydoc RenderTexture::getProperties
     */
    const RenderTargetProperties&
    getPropertiesInternal() const override {
      return mProperties;
    }

    RenderTextureProperties mProperties;
  };

  namespace geCoreThread {
    /**
     * @brief DirectX 11 implementation of a render texture.
     * @note  Core thread only.
     */
    class D3D11RenderTexture : public RenderTexture
    {
     public:
      D3D11RenderTexture(const RENDER_TEXTURE_DESC& desc, uint32 deviceIdx);
      virtual ~D3D11RenderTexture() = default;

      /**
       * @copydoc RenderTexture::getCustomAttribute
       */
      void
      getCustomAttribute(const String& name, void* data) const override;

    protected:
      /**
       * @copydoc RenderTexture::getProperties
       */
      const RenderTargetProperties&
      getPropertiesInternal() const override {
        return mProperties;
      }

      RenderTextureProperties mProperties;
    };
  }
}
