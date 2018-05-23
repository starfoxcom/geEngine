/*****************************************************************************/
/**
 * @file    geRenderTexture.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2018/05/20
 * @brief   Render target specialization that allows to render on textures.
 *
 * Render target specialization that allows you to render into one or multiple
 * textures. Such textures can then be used in other operations as GPU program
 * input.
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
#include "geRenderTarget.h"

namespace geEngineSDK {
  /**
   * @brief Structure that describes a render texture color and depth / stencil
   *        surfaces.
   */
  struct GE_CORE_EXPORT RENDER_TEXTURE_DESC
  {
    RENDER_SURFACE_DESC colorSurfaces[GE_MAX_MULTIPLE_RENDER_TARGETS];
    RENDER_SURFACE_DESC depthStencilSurface;
  };

  namespace geCoreThread { struct RENDER_TEXTURE_DESC; }

  /**
   * @brief Contains various properties that describe a render texture.
   */
  class GE_CORE_EXPORT RenderTextureProperties : public RenderTargetProperties
  {
   public:
    RenderTextureProperties(const RENDER_TEXTURE_DESC& desc,
                            bool requiresFlipping);
    RenderTextureProperties(const geCoreThread::RENDER_TEXTURE_DESC& desc,
                            bool requiresFlipping);
    virtual ~RenderTextureProperties() = default;

   private:
    void
    construct(const TextureProperties* textureProps,
              uint32 numSlices,
              uint32 mipLevel,
              bool requiresFlipping,
              bool hwGamma);

    friend class geCoreThread::RenderTexture;
    friend class RenderTexture;
  };

  /**
   * @brief Render target specialization that allows you to render into one or
   *        multiple textures. Such textures can then be used in other
   *        operations as GPU program input.
   * @note  Sim thread only. Retrieve core implementation from getCore() for
   *        core thread only functionality.
   */
  class GE_CORE_EXPORT GE_SCRIPT_EXPORT(m:Rendering) RenderTexture
    : public RenderTarget
  {
   public:
    virtual ~RenderTexture() = default;

    /**
     * @copydoc TextureManager::createRenderTexture(const TEXTURE_DESC&,
     *                                              bool,
     *                                              PixelFormat)
     */
    static SPtr<RenderTexture>
    create(const TEXTURE_DESC& colorDesc,
           bool createDepth = true,
           PixelFormat depthStencilFormat = PixelFormat::kD32);

    /**
     * @copydoc TextureManager::createRenderTexture(const RENDER_TEXTURE_DESC&)
     */
    static SPtr<RenderTexture>
    create(const RENDER_TEXTURE_DESC& desc);

    /**
     * @brief Returns a color surface texture you may bind as an input to an
     *        GPU program.
     * @note  Be aware that you cannot bind a render texture for reading and
     *        writing at the same time.
     */
    const HTexture&
    getColorTexture(uint32 idx) const {
      return m_bindableColorTex[idx];
    }

    /**
     * @brief Returns a depth/stencil surface texture you may bind as an input
     *        to an GPU program.
     * @note  Be aware that you cannot bind a render texture for reading and
     *        writing at the same time.
     */
    const HTexture&
    getDepthStencilTexture() const {
      return m_bindableDepthStencilTex;
    }

    /**
     * @brief Retrieves a core implementation of a render texture usable only
     *        from the core thread.
     * @note  Core thread only.
     */
    SPtr<geCoreThread::RenderTexture>
    getCore() const;

    /**
     * @brief Returns properties that describe the render texture.
     */
    const RenderTextureProperties&
    getProperties() const;

   protected:
    friend class TextureManager;

    RenderTexture(const RENDER_TEXTURE_DESC& desc);

    /**
     * @copydoc CoreObject::createCore
     */
    SPtr<geCoreThread::CoreObject>
    createCore() const override;

    /**
     * @copydoc CoreObject::syncToCore
     */
    CoreSyncData
    syncToCore(FrameAlloc* allocator) override;

   protected:
    HTexture m_bindableColorTex[GE_MAX_MULTIPLE_RENDER_TARGETS];
    HTexture m_bindableDepthStencilTex;

    RENDER_TEXTURE_DESC m_desc;
  };

  namespace geCoreThread {
    /**
     * @see geEngineSDK::RENDER_TEXTURE_DESC
     * @note  References core textures instead of texture handles.
     */
    struct GE_CORE_EXPORT RENDER_TEXTURE_DESC
    {
      RENDER_SURFACE_DESC colorSurfaces[GE_MAX_MULTIPLE_RENDER_TARGETS];
      RENDER_SURFACE_DESC depthStencilSurface;
    };

    /**
     * @brief Provides access to internal render texture implementation usable
     *        only from the core thread.
     * @note  Core thread only.
     */
    class GE_CORE_EXPORT RenderTexture : public RenderTarget
    {
     public:
      RenderTexture(const RENDER_TEXTURE_DESC& desc, uint32 deviceIdx);
      virtual ~RenderTexture() = default;

      /**
       * @copydoc CoreObject::initialize
       */
      void
      initialize() override;

      /**
       * @copydoc
       * TextureManager::createRenderTexture(const RENDER_TEXTURE_DESC&,
                                             uint32)
       */
      static SPtr<RenderTexture>
      create(const RENDER_TEXTURE_DESC& desc, uint32 deviceIdx = 0);

      /**
       * @brief Returns a color surface texture you may bind as an input to an
       *        GPU program.
       * @note  Be aware that you cannot bind a render texture for reading and
       *        writing at the same time.
       */
      SPtr<Texture>
      getColorTexture(uint32 idx) const {
        return m_desc.colorSurfaces[idx].texture;
      }

      /**
       * @brief Returns a depth/stencil surface texture you may bind as an
       *        input to an GPU program.
       * @note  Be aware that you cannot bind a render texture for reading and
       *        writing at the same time.
       */
      SPtr<Texture>
      getDepthStencilTexture() const {
        return m_desc.depthStencilSurface.texture;
      }

      /**
       * @brief Returns properties that describe the render texture.
       */
      const RenderTextureProperties&
      getProperties() const;

     protected:
      /**
       * @copydoc CoreObject::syncToCore
       */
      void
      syncToCore(const CoreSyncData& data) override;

     private:
      /**
       * @brief Throws an exception of the color and depth / stencil buffers
       *        aren't compatible.
       */
      void
      throwIfBuffersDontMatch() const;

     protected:
      friend class geEngineSDK::RenderTexture;

      SPtr<TextureView> m_colorSurfaces[GE_MAX_MULTIPLE_RENDER_TARGETS];
      SPtr<TextureView> m_depthStencilSurface;

      RENDER_TEXTURE_DESC m_desc;
    };
  }
}
