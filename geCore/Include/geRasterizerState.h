/*****************************************************************************/
/**
 * @file    geRasterizerState.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2018/06/09
 * @brief   State that allows to modify how are polygons converted to pixels.
 *
 * Render system pipeline state that allows you to modify how an object is
 * rasterized (how are polygons converted to pixels).
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

namespace geEngineSDK {
  /**
   * @brief Structure that describes pipeline rasterizer state.
   *        Used for initializing a RasterizerState.
   */
  struct GE_CORE_EXPORT RASTERIZER_STATE_DESC
  {
    bool
    operator==(const RASTERIZER_STATE_DESC& rhs) const;

    /**
     * @brief Polygon mode allows you to draw polygons as solid objects or as
     *        wireframe by just drawing their edges.
     */
    POLYGON_MODE::E polygonMode = POLYGON_MODE::kSOLID;

    /**
     * @brief Sets vertex winding order. Faces that contain vertices with this
     *        order will be culled and not rasterized. Used primarily for
     *        saving cycles by not rendering back facing faces.
     */
    CULLING_MODE::E cullMode = CULLING_MODE::kCOUNTERCLOCKWISE;

    /**
     * @brief Represents a constant depth bias that will offset the depth
     *        values of new pixels by the specified amount.
     * @note  This is useful if you want to avoid z fighting for objects at the
     *        same or similar depth.
     */
    float depthBias = 0;

    /**
     * @brief Maximum depth bias value.
     */
    float depthBiasClamp = 0.0f;

    /**
     * @brief Represents a dynamic depth bias that increases as the slope of
     *        the rendered polygons surface increases. Resulting value offsets
     *        depth values of new pixels. This offset will be added on top of
     *        the constant depth bias.
     * @note  This is useful if you want to avoid z fighting for objects at the
     *        same or similar depth.
     */
    float slopeScaledDepthBias = 0.0f;

    /**
     * @brief If true, clipping of polygons past the far Z plane is enabled.
     *        This ensures proper Z ordering for polygons outside of valid
     *        depth range (otherwise they all have the same depth). It can be
     *        useful to disable if you are performing stencil operations that
     *        count on objects having a front and a back (like stencil shadow)
     *        and don't want to clip the back.
     */
    bool depthClipEnable = true;

    /**
     * @brief Scissor rectangle allows you to cull all pixels outside of the
     *        scissor rectangle.
     * @see   geCoreThread::RenderAPI::setScissorRect
     */
    bool scissorEnable = false;

    /**
     * @brief Determines how are samples in multi-sample render targets
     *        handled. If disabled all samples in the render target will be
     *        written the same value, and if enabled each sample will be
     *        generated separately.
     * @note  In order to get an anti aliased image you need to both enable
     *        this option and use a MSAA render target.
     */
    bool multisampleEnable = true;

    /**
     * @brief Determines should the lines be anti aliased. This is separate
     *        from multi-sample anti aliasing setting as lines can be anti
     *        aliased without multi-sampling.
     * @note  This setting is usually ignored if MSAA is used, as that provides
     *        sufficient anti aliasing.
     */
    bool antialiasedLineEnable = false;
  };

  /**
   * @brief Properties of RasterizerState.
   *        Shared between sim and core thread versions of RasterizerState.
   */
  class GE_CORE_EXPORT RasterizerProperties
  {
   public:
    RasterizerProperties(const RASTERIZER_STATE_DESC& desc);

    /**
     * @copydoc RASTERIZER_STATE_DESC::polygonMode
     */
    POLYGON_MODE::E
    getPolygonMode() const {
      return m_data.polygonMode;
    }

    /**
     * @copydoc RASTERIZER_STATE_DESC::cullMode
     */
    CULLING_MODE::E
    getCullMode() const {
      return m_data.cullMode;
    }

    /**
     * @copydoc RASTERIZER_STATE_DESC::depthBias
     */
    float
    getDepthBias() const {
      return m_data.depthBias;
    }

    /**
     * @copydoc RASTERIZER_STATE_DESC::depthBiasClamp
     */
    float
    getDepthBiasClamp() const {
      return m_data.depthBiasClamp;
    }

    /**
     * @copydoc RASTERIZER_STATE_DESC::slopeScaledDepthBias
     */
    float
    getSlopeScaledDepthBias() const {
      return m_data.slopeScaledDepthBias;
    }

    /**
     * @copydoc RASTERIZER_STATE_DESC::depthClipEnable
     */
    bool
    getDepthClipEnable() const {
      return m_data.depthClipEnable;
    }

    /**
     * @copydoc RASTERIZER_STATE_DESC::scissorEnable
     */
    bool
    getScissorEnable() const {
      return m_data.scissorEnable;
    }

    /**
     * @copydoc RASTERIZER_STATE_DESC::multisampleEnable
     */
    bool
    getMultisampleEnable() const {
      return m_data.multisampleEnable;
    }

    /**
     * @copydoc RASTERIZER_STATE_DESC::antialiasedLineEnable
     */
    bool
    getAntialiasedLineEnable() const {
      return m_data.antialiasedLineEnable;
    }

    /**
     * @brief Returns the hash value generated from the rasterizer state properties.
     */
    uint64
    getHash() const {
      return m_hash;
    }

   protected:
    friend class RasterizerState;
    friend class geCoreThread::RasterizerState;
    friend class RasterizerStateRTTI;

    RASTERIZER_STATE_DESC m_data;
    uint64 m_hash;
  };

  /**
   * @brief Render system pipeline state that allows you to modify how an
   *        object is rasterized (how are polygons converted to pixels).
   * @note  Rasterizer states are immutable. Sim thread only.
   */
  class GE_CORE_EXPORT RasterizerState : public IReflectable, public CoreObject
  {
   public:
    virtual ~RasterizerState() = default;

    /**
     * @brief Returns information about the rasterizer state.
     */
    const RasterizerProperties& getProperties() const;

    /**
     * @brief Retrieves a core implementation of the rasterizer state usable only from the core thread.
     */
    SPtr<geCoreThread::RasterizerState> getCore() const;

    /**
     * @brief Creates a new rasterizer state using the specified rasterizer state descriptor structure.
     */
    static SPtr<RasterizerState> create(const RASTERIZER_STATE_DESC& desc);

    /**
     * @brief Returns the default rasterizer state.
     */
    static const SPtr<RasterizerState>& getDefault();

    /**
     * @brief Generates a hash value from a rasterizer state descriptor.
     */
    static uint64 generateHash(const RASTERIZER_STATE_DESC& desc);

  protected:
    friend class RenderStateManager;

    RasterizerState(const RASTERIZER_STATE_DESC& desc);

    /**
     * @copydoc CoreObject::createCore
     */
    SPtr<geCoreThread::CoreObject> createCore() const override;

    RasterizerProperties m_properties;
    mutable uint32 m_id = 0;

    /*************************************************************************/
    /**
     * RTTI
     */
    /*************************************************************************/

   public:
    friend class RasterizerStateRTTI;

    static RTTITypeBase*
    getRTTIStatic();

    RTTITypeBase*
    getRTTI() const override;
  };

  namespace geCoreThread {
    /**
     * @brief Core thread version of geEngineSDK::RasterizerState.
     * @note  Core thread.
     */
    class GE_CORE_EXPORT RasterizerState : public CoreObject
    {
     public:
      virtual ~RasterizerState() = default;

      /**
       * @brief Returns information about the rasterizer state.
       */
      const RasterizerProperties&
      getProperties() const;

      /**
       * @brief Returns a unique state ID. Only the lowest 10 bits are used.
       */
      uint32
      getId() const {
        return m_id;
      }

      /**
       * @brief Creates a new rasterizer state using the specified rasterizer state descriptor structure.
       */
      static SPtr<RasterizerState>
      create(const RASTERIZER_STATE_DESC& desc);

      /**
       * @brief Returns the default rasterizer state.
       */
      static const SPtr<RasterizerState>&
      getDefault();

     protected:
      friend class RenderStateManager;

      RasterizerState(const RASTERIZER_STATE_DESC& desc, uint32 id);

      /**
       * @copydoc CoreObject::initialize
       */
      void
      initialize() override;

      /**
       * @brief Creates any API-specific state objects.
       */
      virtual void
      createInternal() {}

      RasterizerProperties m_properties;
      uint32 m_id;
    };
  }

  GE_ALLOW_MEMCPY_SERIALIZATION(RASTERIZER_STATE_DESC);
}

namespace std
{
  /**
   * @brief Hash value generator for RASTERIZER_STATE_DESC.
   */
  template<>
  struct hash<geEngineSDK::RASTERIZER_STATE_DESC>
  {
    size_t
    operator()(const geEngineSDK::RASTERIZER_STATE_DESC& value) const {
      return static_cast<size_t>(geEngineSDK::RasterizerState::generateHash(value));
    }
  };
}
