/*****************************************************************************/
/**
 * @file    geTextureManager.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2018/05/21
 * @brief   Interface for creation of textures.
 *
 * Defines an interface for creation of textures. Render systems provide their
 * own implementations.
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
#include "gePrerequisitesCore.h"
#include "geTexture.h"
#include "geRenderTexture.h"

#include <geModule.h>

namespace geEngineSDK {
  class GE_CORE_EXPORT TextureManager : public Module<TextureManager>
  {
   public:
    virtual ~TextureManager() = default;

    /**
     * @copydoc Texture::create(const TEXTURE_DESC&)
     */
    SPtr<Texture>
    createTexture(const TEXTURE_DESC& desc);

    /**
     * @brief Creates a new 2D or 3D texture initialized using the provided
     *        pixel data. Texture will not have any mipmaps.
     * @param[in] desc      Description of the texture to create.
     *                      Must match the pixel data.
     * @param[in] pixelData Data to initialize the texture width.
     */
    SPtr<Texture>
    createTexture(const TEXTURE_DESC& desc, const SPtr<PixelData>& pixelData);

    /**
     * @brief Creates a completely empty and uninitialized Texture.
     * @note  Internal method. Should only be used for very specific purposes,
     *        like deserialization, as it requires additional manual
     *        initialization that is not required normally.
     */
    SPtr<Texture>
    _createEmpty();

    /**
     * @brief Creates a new RenderTexture and automatically generates a single
     *        color surface and (optionally) a depth/stencil surface.
     * @param[in] colorDesc       Description of the color surface to create.
     * @param[in] createDepth     Determines will a depth/stencil buffer of the
     *                            same size as the color buffer be created for
     *                            the render texture.
     * @param[in] depthStencilFormat  Format of the depth/stencil buffer if
     *                                enabled.
     */
    virtual SPtr<RenderTexture>
    createRenderTexture(const TEXTURE_DESC& colorDesc,
                        bool createDepth = true,
                        PixelFormat depthStencilFormat = PixelFormat::kD32);

    /**
     * @brief Creates a RenderTexture using the description struct.
     * @param[in] desc  Description of the render texture to create.
     */
    virtual SPtr<RenderTexture>
    createRenderTexture(const RENDER_TEXTURE_DESC& desc);

    /**
     * @brief Gets the format which will be natively used for a requested
     *        format given the constraints of the current device.
     * @note  Thread safe.
     */
    virtual PixelFormat
    getNativeFormat(TextureType ttype,
                    PixelFormat format,
                    int32 usage,
                    bool hwGamma) = 0;

   protected:
    /**
     * @brief Creates an empty and uninitialized render texture of a specific
     *        type. This is to be implemented by render systems with their own
     *        implementations.
     */
    virtual SPtr<RenderTexture>
    createRenderTextureImpl(const RENDER_TEXTURE_DESC& desc) = 0;

    mutable HTexture m_dummyTexture;
  };

  namespace geCoreThread {
    /**
     * @brief Defines interface for creation of textures. Render systems
     *        provide their own implementations.
     * @note  Core thread only.
     */
    class GE_CORE_EXPORT TextureManager : public Module<TextureManager>
    {
     public:
      virtual ~TextureManager() = default;

      /**
       * @copydoc Module::onStartUp
       */
      void
      onStartUp() override;

      /**
       * @copydoc Module::onShutDown
       */
      void
      onShutDown() override;

      /**
       * @copydoc geEngineSDK::TextureManager::createTexture(const TEXTURE_DESC&)
       * @param[in] deviceMask  Mask that determines on which GPU devices
       *                        should the object be created on.
       */
      SPtr<Texture>
      createTexture(const TEXTURE_DESC& desc,
                    GPU_DEVICE_FLAGS::E deviceMask = GPU_DEVICE_FLAGS::kDEFAULT);

      /**
       * @copydoc geEngineSDK::TextureManager::createRenderTexture(const RENDER_TEXTURE_DESC&)
       * @param[in] deviceIdx Index of the GPU device to create the object on.
       */
      SPtr<RenderTexture>
      createRenderTexture(const RENDER_TEXTURE_DESC& desc, uint32 deviceIdx = 0);

     protected:
      friend class geEngineSDK::Texture;
      friend class Texture;
      friend class geEngineSDK::RenderTexture;

      /**
       * @brief Creates an empty and uninitialized texture of a specific type.
       *        This is to be implemented	by render systems with their own
       *        implementations.
       */
      virtual SPtr<Texture>
      createTextureInternal(const TEXTURE_DESC& desc,
                            const SPtr<PixelData>& initialData = nullptr,
                            GPU_DEVICE_FLAGS::E deviceMask = GPU_DEVICE_FLAGS::kDEFAULT) = 0;

      /**
       * @copydoc createRenderTexture
       */
      virtual SPtr<RenderTexture>
      createRenderTextureInternal(const RENDER_TEXTURE_DESC& desc, uint32 deviceIdx = 0) = 0;
    };
  }
}
