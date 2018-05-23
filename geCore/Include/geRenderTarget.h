/*****************************************************************************/
/**
 * @file    geRenderTarget.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2018/05/12
 * @brief   Buffer or texture that the render system renders the scene to.
 *
 * Render target is a frame buffer or a texture that the render system renders
 * the scene to.
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
#include "gePixelUtil.h"
#include "geViewport.h"
#include "geCoreObject.h"

#include <geEvent.h>

namespace geEngineSDK {
  /**
   * @brief Structure that contains information about what part of the texture
   *        represents the render surface.
   */
  struct GE_CORE_EXPORT RENDER_SURFACE_DESC
  {
    RENDER_SURFACE_DESC() = default;

    HTexture texture;

    /**
     * @brief First face of the texture to bind (array index in texture arrays,
     *        or Z slice in 3D textures).
     */
    uint32 face = 0;

    /**
     * @brief Number of faces to bind (entries in a texture array, or Z slices
     *        in 3D textures). When zero the entire resource will be bound.
     */
    uint32 numFaces = 0;

    /**
     * @brief If the texture has multiple mips, which one to bind (only one can
     *        be bound for rendering).
     */
    uint32 mipLevel = 0;
  };

  namespace geCoreThread {
    /**
     * @see   geEngineSDK::RENDER_SURFACE_DESC
     * @note  References core textures instead of texture handles.
     */
    struct GE_CORE_EXPORT RENDER_SURFACE_DESC
    {
      RENDER_SURFACE_DESC() = default;

      SPtr<Texture> texture;

      /**
       * @brief First face of the texture to bind (array index in texture
       *        arrays, or Z slice in 3D textures).
       */
      uint32 face = 0;

      /**
       * @brief Number of faces to bind (entries in a texture array, or Z
       *        slices in 3D textures). When zero the entire resource will be
       *        bound.
       */
      uint32 numFaces = 0;

      /**
       * @brief If the texture has multiple mips, which one to bind (only one
       *        can be bound for rendering).
       */
      uint32 mipLevel = 0;
    };
  }

  class GE_CORE_EXPORT RenderTargetProperties
  {
   public:
    RenderTargetProperties() = default;
    virtual ~RenderTargetProperties() = default;

    /**
     * @brief Width of the render target, in pixels.
     */
    uint32 m_width = 0;

    /**
     * @brief Height of the render target, in pixels.
     */
    uint32 m_height = 0;

    /**
     * @brief Number of three dimensional slices of the render target. This
     *        will be number of layers for array textures or number of faces
     *        cube textures.
     */
    uint32 m_numSlices = 0;

    /**
     * @brief Controls in what order is the render target rendered to compared
     *        to other render targets. Targets with higher priority will be
     *        rendered before ones with lower priority.
     */
    int32 m_priority = 0;

    /**
     * @brief True if the render target will wait for vertical sync before
     *        swapping buffers. This will eliminate tearing but may increase
     *        input latency.
     */
    bool m_vsync = false;

    /**
     * @brief Controls how often should the frame be presented in respect to
     *        display device refresh rate. Normal value is 1 where it will
     *        match the refresh rate. Higher values will decrease the frame
     *        rate (for example present interval of 2 on 60Hz refresh rate will
     *        display at most 30 frames per second).
     */
    uint32 m_vsyncInterval = 1;

    /**
     * @brief True if pixels written to the render target will be gamma
     *        corrected.
     */
    bool m_hwGamma = false;

    /**
     * @brief Does the texture need to be vertically flipped because of
     *        different screen space coordinate systems. (Determines is origin
     *        top left or bottom left. Engine default is top left.)
     */
    bool m_requiresTextureFlipping = false;

    /**
     * @brief True if the target is a window, false if an off screen target.
     */
    bool m_isWindow = false;

    /**
     * @brief Controls how many samples are used for multi sampling.
     *        (0 or 1 if multi sampling is not used).
     */
    uint32 m_multisampleCount = 0;
  };

  /**
   * @brief Render target is a frame buffer or a texture that the render system
   *        renders the scene to.
   * @note Sim thread unless noted otherwise. Retrieve core implementation from
   *        getCore() for core thread only functionality.
   */
  class GE_CORE_EXPORT GE_SCRIPT_EXPORT(m:Rendering) RenderTarget
    : public CoreObject
  {
   public:
    RenderTarget();
    virtual ~RenderTarget() = default;

    /**
     * @brief Queries the render target for a custom attribute. This may be
     *        anything and is implementation specific.
     */
    virtual void
    getCustomAttribute(const String& name, void* pData) const;

    /**
     * @copydoc geCoreThread::RenderTarget::setPriority
     * @note This is an @ref asyncMethod "asynchronous method".
     */
    void
    setPriority(int32 priority);

    /**
     * @brief Returns properties that describe the render target.
     * @note  Sim thread only.
     */
    const RenderTargetProperties&
    getProperties() const;

    /**
     * @brief Retrieves a core implementation of a render target usable only
     *        from the core thread.
     */
    SPtr<geCoreThread::RenderTarget>
    getCore() const;

    /**
     * @brief Event that gets triggered whenever the render target is resized.
     * @note  Sim thread only.
     */
    mutable Event<void()> onResized;

   protected:
    friend class geCoreThread::RenderTarget;

    /**
     * @brief Returns properties that describe the render target.
     */
    virtual const
    RenderTargetProperties& getPropertiesInternal() const = 0;
  };

  namespace geCoreThread {
    /**
    * @brief Frame buffer type when double-buffering is used.
    */
    namespace FRAME_BUFFER {
      enum E {
        kFRONT,
        kBACK,
        kAUTO
      };
    }

    /**
     * @brief Provides access to internal render target implementation usable
     *        only from the core thread.
     * @note  Core thread only.
     */
    class GE_CORE_EXPORT RenderTarget : public CoreObject
    {
     public:
      RenderTarget() = default;
      virtual ~RenderTarget() = default;

      /**
       * @brief Sets a priority that determines in which orders the render
       *        targets are processed.
       * @param[in] priority  The priority. Higher value means the target will
       *            be rendered sooner.
       */
      void
      setPriority(int32 priority);

      /**
       * @brief Swaps the frame buffers to display the next frame.
       * @param[in] syncMask  Optional synchronization mask that determines for
       *            which queues should the system wait before performing the
       *            swap buffer operation. By default the system waits for all
       *            queues. However if certain queues are performing
       *            non-rendering operations, or operations not related to this
       *            render target, you can exclude them from the sync mask for
       *            potentially better performance. You can use CommandSyncMask
       *            to generate a valid sync mask.
       */
      virtual void
      swapBuffers(uint32 syncMask = 0xFFFFFFFF) {
        GE_UNREFERENCED_PARAMETER(syncMask);
      }

      /**
       * @brief Queries the render target for a custom attribute. This may be
       *        anything and is implementation specific.
       */
      virtual void
      getCustomAttribute(const String& name, void* pData) const;

      /**
       * @brief Returns properties that describe the render target.
       */
      const RenderTargetProperties&
      getProperties() const;

     protected:
      friend class geEngineSDK::RenderTarget;

      /**
       * @brief Returns properties that describe the render target.
       */
      virtual const RenderTargetProperties&
      getPropertiesInternal() const = 0;
    };
  }
}
