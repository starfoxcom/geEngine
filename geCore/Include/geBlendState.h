/*****************************************************************************/
/**
 * @file    geBlendState.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2018/06/02
 * @brief   Pipeline state that allows you to modify how an object is rendered.
 *
 * Render system pipeline state that allows you to modify how an object is
 * rendered. More exactly this state allows to you to control how is a rendered
 * object blended with any previously rendered objects.
 *
 * @note    Blend states are immutable. Sim thread only.
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
#include "geResource.h"

#include <geNumericLimits.h>

namespace geEngineSDK {
  /**
   * @brief Structure that describes blend states for a single render target.
   *        Used internally by BLEND_STATE_DESC for initializing a BlendState.
   * @see BLEND_STATE_DESC
   * @see BlendState
   */
  struct GE_CORE_EXPORT RENDER_TARGET_BLEND_STATE_DESC
  {
    bool
    operator==(const RENDER_TARGET_BLEND_STATE_DESC& rhs) const;

    /**
     * @brief Queries is blending enabled for the specified render target.
     *        Blending allows you to combine the color from current and
     *        previous pixel based on some value.
     */
    bool blendEnable = false;

    /**
     * @brief Determines what should the source blend factor be.
     *        This value determines what will the color being generated
     *        currently be multiplied by.
     */
    BLEND_FACTOR::E srcBlend = BLEND_FACTOR::kONE;

    /**
     * @brief Determines what should the destination blend factor be.
     *        This value determines what will the color already in render
     *        target be multiplied by.
     */
    BLEND_FACTOR::E dstBlend = BLEND_FACTOR::kZERO;

    /**
     * @brief Determines how are source and destination colors combined
     *        (after they are multiplied by their respective blend factors).
     */
    BLEND_OPERATION::E blendOp = BLEND_OPERATION::kADD;

    /**
     * @brief Determines what should the alpha source blend factor be.
     *        This value determines what will the alpha value being generated
     *        currently be multiplied by.
     */
    BLEND_FACTOR::E srcBlendAlpha = BLEND_FACTOR::kONE;

    /**
     * @brief Determines what should the alpha destination blend factor be.
     *        This value determines what will the alpha value already in render
     *        target be multiplied by.
     */
    BLEND_FACTOR::E dstBlendAlpha = BLEND_FACTOR::kZERO;

    /**
     * @brief Determines how are source and destination alpha values combined
     *        (after they are multiplied by their respective blend factors).
     */
    BLEND_OPERATION::E blendOpAlpha = BLEND_OPERATION::kADD;

    /**
     * @brief Render target write mask allows to choose which pixel components
     *        should the pixel shader output.
     *        Only the first four bits are used. First bit representing red,
     *        second green, third blue and fourth alpha value.
     *        Set bits means pixel shader will output those channels.
     */
    uint8 renderTargetWriteMask = 0xFF;
  };

  /**
   * @brief Structure that describes render pipeline blend states.
   *        Used for initializing BlendState.
   */
  struct GE_CORE_EXPORT BLEND_STATE_DESC
  {
    bool
    operator==(const BLEND_STATE_DESC& rhs) const;

    /**
     * @brief Alpha to coverage allows you to perform blending without needing
     *        to worry about order of rendering like regular blending does. It
     *        requires multi-sampling to be active in order to work, and you
     *        need to supply an alpha texture that determines object
     *        transparency. Blending is then performed by only using
     *        sub-samples covered by the alpha texture for the current pixel
     *        and combining them with sub-samples previously stored.
     *        Be aware this is a limited technique only useful for certain
     *        situations. Unless you are having performance problems use
     *        regular blending.
     */
    bool alphaToCoverageEnable = false;

    /**
     * @brief When not set, only the first render target blend descriptor will
     *        be used for all render targets. If set each render target will
     *        use its own blend descriptor.
     */
    bool independantBlendEnable = false;

    RENDER_TARGET_BLEND_STATE_DESC renderTargetDesc[GE_MAX_MULTIPLE_RENDER_TARGETS];
  };

  /**
   * @brief Properties of a BlendState.
   *        Shared between sim and core thread versions of BlendState.
   */
  class GE_CORE_EXPORT BlendProperties
  {
   public:
    BlendProperties(const BLEND_STATE_DESC& desc);

    /**
     * @copydoc BLEND_STATE_DESC::alphaToCoverageEnable
     */
    bool
    getAlphaToCoverageEnabled() const {
      return m_data.alphaToCoverageEnable;
    }

    /**
     * @copydoc BLEND_STATE_DESC::independantBlendEnable
     */
    bool
    getIndependantBlendEnable() const {
      return m_data.independantBlendEnable;
    }

    /**
     * @copydoc RENDER_TARGET_BLEND_STATE_DESC::blendEnable
     */
    bool
    getBlendEnabled(uint32 renderTargetIdx) const;

    /**
     * @copydoc RENDER_TARGET_BLEND_STATE_DESC::srcBlend
     */
    BLEND_FACTOR::E
    getSrcBlend(uint32 renderTargetIdx) const;

    /**
     * @copydoc RENDER_TARGET_BLEND_STATE_DESC::dstBlend
     */
    BLEND_FACTOR::E
    getDstBlend(uint32 renderTargetIdx) const;

    /**
     * @copydoc RENDER_TARGET_BLEND_STATE_DESC::blendOp
     */
    BLEND_OPERATION::E
    getBlendOperation(uint32 renderTargetIdx) const;

    /**
     * @copydoc RENDER_TARGET_BLEND_STATE_DESC::srcBlendAlpha
     */
    BLEND_FACTOR::E
    getAlphaSrcBlend(uint32 renderTargetIdx) const;

    /**
     * @copydoc RENDER_TARGET_BLEND_STATE_DESC::dstBlendAlpha
     */
    BLEND_FACTOR::E
    getAlphaDstBlend(uint32 renderTargetIdx) const;

    /**
     * @copydoc RENDER_TARGET_BLEND_STATE_DESC::blendOpAlpha
     */
    BLEND_OPERATION::E
    getAlphaBlendOperation(uint32 renderTargetIdx) const;

    /**
     * @copydoc RENDER_TARGET_BLEND_STATE_DESC::renderTargetWriteMask
     */
    uint8
    getRenderTargetWriteMask(uint32 renderTargetIdx) const;

    /**
     * Returns the hash value generated from the blend state properties.
     */
    uint64
    getHash() const {
      return m_hash;
    }

   protected:
    friend class BlendState;
    friend class geCoreThread::BlendState;
    friend class BlendStateRTTI;

    BLEND_STATE_DESC m_data;
    uint64 m_hash;
  };

  /**
   * @brief Render system pipeline state that allows you to modify how an
   *        object is rendered. More exactly this state allows to you to
   *        control how is a rendered object blended with any previously
   *        rendered objects.
   * @note  Blend states are immutable. Sim thread only.
   */
  class GE_CORE_EXPORT BlendState : public IReflectable, public CoreObject
  {
   public:
    virtual ~BlendState() = default;

    /**
     * @brief Returns information about a blend state.
     */
    const BlendProperties&
    getProperties() const;

    /**
     * @brief Retrieves a core implementation of the sampler state usable only
     *        from the core thread.
     */
    SPtr<geCoreThread::BlendState>
    getCore() const;

    /**
     * @brief Creates a new blend state using the specified blend state
     *        description structure.
     */
    static SPtr<BlendState>
    create(const BLEND_STATE_DESC& desc);

    /**
     * @brief Returns the default blend state that you may use when no other is
     *        available.
     */
    static const SPtr<BlendState>&
    getDefault();

    /**
     * @brief Generates a hash value from a blend state descriptor.
     */
    static uint64
    generateHash(const BLEND_STATE_DESC& desc);

   protected:
    friend class RenderStateManager;

    BlendState(const BLEND_STATE_DESC& desc);

    /**
     * @copydoc CoreObject::createCore
     */
    SPtr<geCoreThread::CoreObject>
    createCore() const override;

    BlendProperties m_properties;
    mutable uint32 m_id;

    /*************************************************************************/
    /**
     * RTTI
     */
    /*************************************************************************/

   public:
    friend class BlendStateRTTI;

    static RTTITypeBase*
    getRTTIStatic();

    RTTITypeBase*
    getRTTI() const override;
  };

  namespace geCoreThread {
    /**
     * @brief Core thread version of geEngineSDK::BlendState.
     * @note  Core thread.
     */
    class GE_CORE_EXPORT BlendState : public CoreObject
    {
     public:
      virtual ~BlendState() = default;

      /**
       * @brief Returns information about the blend state.
       */
      const BlendProperties&
      getProperties() const;

      /**
       * @brief Returns a unique state ID. Only the lowest 10 bits are used.
       */
      uint32
      getId() const {
        return m_id;
      }

      /**
       * @brief Creates a new blend state using the specified blend state
       *        description structure.
       */
      static SPtr<BlendState>
      create(const BLEND_STATE_DESC& desc);

      /**
       * @brief Returns the default blend state that you may use when no other
       *        is available.
       */
      static const SPtr<BlendState>&
      getDefault();

     protected:
      friend class RenderStateManager;

      BlendState(const BLEND_STATE_DESC& desc, uint32 id);

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

      BlendProperties m_properties;
      uint32 m_id;
    };
  }

  template<>
  struct RTTIPlainType<BLEND_STATE_DESC>
  {
    enum { kID = TYPEID_CORE::kID_BLEND_STATE_DESC };
    enum { kHasDynamicSize = 1 };

    static void
    toMemory(const BLEND_STATE_DESC& data, char* memory) {
      uint32 size = getDynamicSize(data);

      memcpy(memory, &size, sizeof(uint32));
      memory += sizeof(uint32);
      size -= sizeof(uint32);
      memcpy(memory, &data, size);
    }

    static uint32
    fromMemory(BLEND_STATE_DESC& data, char* memory) {
      uint32 size;
      memcpy(&size, memory, sizeof(uint32));
      memory += sizeof(uint32);

      uint32 dataSize = size - sizeof(uint32);
      memcpy((void*)&data, memory, dataSize);

      return size;
    }

    static uint32
    getDynamicSize(const BLEND_STATE_DESC& data) {
      uint64 dataSize = sizeof(data) + sizeof(uint32);
      uint64 maxSize = static_cast<uint64>(geEngineSDK::NumLimit::MAX_UINT32);
#if GE_DEBUG_MODE
      if (dataSize > maxSize) {
        GE_EXCEPT(InternalErrorException,
                  "Data overflow! Size doesn't fit into 32 bits.");
      }
#endif

      return static_cast<uint32>(dataSize);
    }
  };
}

namespace std {
  /**
   * @brief Hash value generator for BLEND_STATE_DESC.
   */
  template<>
  struct hash<geEngineSDK::BLEND_STATE_DESC>
  {
    size_t
    operator()(const geEngineSDK::BLEND_STATE_DESC& value) const {
      return static_cast<size_t>(geEngineSDK::BlendState::generateHash(value));
    }
  };
}
