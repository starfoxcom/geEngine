/*****************************************************************************/
/**
 * @file    geViewport.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2018/05/12
 * @brief   Determines to which RenderTarget should rendering be performed.
 *
 * Viewport determines to which RenderTarget should rendering be performed.
 * It allows you to render to a sub-region of the target by specifying the area
 * rectangle, and allows you to set up color / depth / stencil clear values for
 * that specific region.
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
#include "geCoreObject.h"

#include <geIReflectable.h>
#include <geColor.h>
#include <geBox2DI.h>
#include <geBox2D.h>
#include <geEvent.h>

namespace geEngineSDK {
  /**
   * @brief Flags that determine which portion of the viewport to clear.
   */
  enum GE_SCRIPT_EXPORT(n:ClearFlags) class ClearFlagBits {
    kEmpty,
    kColor = 1 << 0,
    kDepth = 1 << 1,
    kStencil = 1 << 2
  };

  using ClearFlags = Flags<ClearFlagBits>;
  GE_FLAGS_OPERATORS(ClearFlagBits);

  class GE_CORE_EXPORT ViewportBase
  {
   public:
    virtual ~ViewportBase() = default;

    /**
     * @brief Determines the area that the viewport covers.
     *        Coordinates are in normalized [0, 1] range.
     */
    GE_SCRIPT_EXPORT(n:Area, pr:setter)
    void
    setArea(const Box2D& area);

    /**
     * @copydoc setArea()
     */
    GE_SCRIPT_EXPORT(n:Area, pr:getter)
    Box2D
    getArea() const {
      return m_normArea;
    }

    /**
     * @brief Returns the area of the render target covered by the viewport,
     *        in pixels.
     */
    GE_SCRIPT_EXPORT(n:PixelArea, pr:getter)
    Box2DI
    getPixelArea() const;

    /**
     * @brief Determines which portions of the render target should be cleared
     *        before rendering to this viewport is performed.
     */
    GE_SCRIPT_EXPORT(n:ClearFlags, pr:setter)
    void
    setClearFlags(ClearFlags flags);

    /**
     * @copydoc setClearFlags()
     */
    GE_SCRIPT_EXPORT(n:ClearFlags, pr:getter)
    ClearFlags
    getClearFlags() const {
      return m_clearFlags;
    }

    /**
     * @brief Sets values to clear color, depth and stencil buffers to.
     */
    void
    setClearValues(const LinearColor& clearColor,
                   float clearDepth = 0.0f,
                   uint16 clearStencil = 0);

    /**
     * @brief Determines the color to clear the viewport to before rendering,
     *        if color clear is enabled.
     */
    GE_SCRIPT_EXPORT(n:ClearColor, pr:setter)
    void
    setClearColorValue(const LinearColor& color);

    /**
     * @copydoc setClearColorValue()
     */
    GE_SCRIPT_EXPORT(n:ClearColor, pr:getter)
    const LinearColor&
    getClearColorValue() const {
      return m_clearColorValue;
    }

    /**
     * @brief Determines the value to clear the depth buffer to before
     *        rendering, if depth clear is enabled.
     */
    GE_SCRIPT_EXPORT(n:ClearDepth, pr:setter)
    void
    setClearDepthValue(float depth);

    /**
     * @copydoc setClearDepthValue()
     */
    GE_SCRIPT_EXPORT(n:ClearDepth, pr:getter)
    float
    getClearDepthValue() const {
      return m_clearDepthValue;
    }

    /**
     * @brief Determines the value to clear the stencil buffer to before
     *        rendering, if stencil clear is enabled.
     */
    GE_SCRIPT_EXPORT(n:ClearStencil, pr:setter)
    void
    setClearStencilValue(uint16 value);

    /**
     * @copydoc setClearStencilValue()
     */
    GE_SCRIPT_EXPORT(n:ClearStencil, pr:getter)
    uint16
    getClearStencilValue() const {
      return m_clearStencilValue;
    }

   protected:
    ViewportBase(float x = 0.0f,
                 float y = 0.0f,
                 float width = 1.0f,
                 float height = 1.0f);

    /**
     * @brief Marks the core data as dirty. This causes the data from the sim
     *        thread object be synced with the core thread version of the
     *        object.
     */
    virtual void
    _markCoreDirty() {}

    /**
     * @brief Gets the render target width.
     */
    virtual uint32
    getTargetWidth() const = 0;

    /**
     * @brief Gets the render target width.
     */
    virtual uint32
    getTargetHeight() const = 0;

    Box2D m_normArea;
    ClearFlags m_clearFlags;
    LinearColor m_clearColorValue;
    float m_clearDepthValue;
    uint16 m_clearStencilValue;

    static const LinearColor DEFAULT_CLEAR_COLOR;
  };

  class GE_CORE_EXPORT GE_SCRIPT_EXPORT(m:Rendering) Viewport
    : public IReflectable, public CoreObject, public ViewportBase
  {
   public:
    /**
     * @brief Determines the render target the viewport is associated with.
     */
    GE_SCRIPT_EXPORT(n:Target, pr:setter)
    void
    setTarget(const SPtr<RenderTarget>& target);

    /**
     * @copydoc setTarget()
     */
    GE_SCRIPT_EXPORT(n:Target, pr:getter)
    SPtr<RenderTarget>
    getTarget() const {
      return m_target;
    }

    /**
     * @brief Retrieves a core implementation of a viewport usable only from
     *        the core thread.
     */
    SPtr<geCoreThread::Viewport>
    getCore() const;

    /**
     * @brief Creates a new viewport.
     * @note  Viewport coordinates are normalized in [0, 1] range.
     */
    GE_SCRIPT_EXPORT(ec:Viewport)
    static SPtr<Viewport>
    create(const SPtr<RenderTarget>& target,
           float x = 0.0f,
           float y = 0.0f,
           float width = 1.0f,
           float height = 1.0f);

   protected:
    Viewport(const SPtr<RenderTarget>& target,
             float x = 0.0f,
             float y = 0.0f,
             float width = 1.0f,
             float height = 1.0f);

    /**
     * @copydoc ViewportBase::_markCoreDirty
     */
    void
    _markCoreDirty() override;

    /**
     * @copydoc ViewportBase::getTargetWidth
     */
    uint32
    getTargetWidth() const override;

    /**
     * @copydoc ViewportBase::getTargetHeight
     */
    uint32
    getTargetHeight() const override;

    /**
     * @copydoc CoreObject::syncToCore
     */
    CoreSyncData
    syncToCore(FrameAlloc* allocator) override;

    /**
     * @copydoc CoreObject::getCoreDependencies
     */
    void
    getCoreDependencies(Vector<CoreObject*>& dependencies) override;

    /**
     * @copydoc CoreObject::createCore
     */
    SPtr<geCoreThread::CoreObject>
    createCore() const override;

    SPtr<RenderTarget> m_target;

    /*************************************************************************/
    /**
     * RTTI
     */
    /*************************************************************************/
    Viewport();

    /**
     * @brief Creates an empty viewport for serialization purposes.
     */
    static SPtr<Viewport>
    createEmpty();

   public:
    friend class ViewportRTTI;

    static RTTITypeBase*
    getRTTIStatic();

    RTTITypeBase*
    getRTTI() const override;
  };

  namespace geCoreThread {
    class GE_CORE_EXPORT Viewport : public CoreObject, public ViewportBase
    {
     public:
      /**
       * @brief Returns the render target the viewport is associated with.
       */
      SPtr<RenderTarget>
      getTarget() const {
        return m_target;
      }

      /**
       * @brief Sets the render target the viewport will be associated with.
       */
      void
      setTarget(const SPtr<RenderTarget>& target) {
        m_target = target;
      }

      /**
       * @copydoc bs::Viewport::create()
       */
      static SPtr<Viewport>
      create(const SPtr<RenderTarget>& target,
             float x = 0.0f,
             float y = 0.0f,
             float width = 1.0f,
             float height = 1.0f);

     protected:
      friend class geEngineSDK::Viewport;

      Viewport(const SPtr<RenderTarget>& target,
               float x = 0.0f,
               float y = 0.0f,
               float width = 1.0f,
               float height = 1.0f);

      /**
       * @copydoc ViewportBase::getTargetWidth
       */
      uint32
      getTargetWidth() const override;

      /**
       * @copydoc ViewportBase::getTargetHeight
       */
      uint32
      getTargetHeight() const override;

      /**
       * @copydoc CoreObject::syncToCore
       */
      void
      syncToCore(const CoreSyncData& data) override;

      SPtr<RenderTarget> m_target;
    };
  }
}
