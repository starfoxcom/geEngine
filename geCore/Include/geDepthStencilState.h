/*****************************************************************************/
/**
 * @file    geDepthStencilState.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2018/06/09
 * @brief   Pipeline state that allows you to modify how an object is rendered.
 *
 * Render system pipeline state that allows you to modify how an object is
 * rendered. More exactly this state allows to you to control how are depth and
 * stencil buffers modified upon rendering.
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
   * @brief Descriptor structured used for initializing DepthStencilState.
   * @see   DepthStencilState
   */
  struct GE_CORE_EXPORT DEPTH_STENCIL_STATE_DESC
  {
    bool
    operator==(const DEPTH_STENCIL_STATE_DESC& rhs) const;

    /**
     * @brief If enabled, any pixel about to be written will be tested against
     *        the depth value currently in the buffer. If the depth test passes
     *        (depending on the set value and chosen depth comparison
     *        function), that pixel is written and depth is updated (if depth
     *        write is enabled).
     */
    bool depthReadEnable = true;

    /**
     * @brief If enabled rendering pixels will update the depth buffer value.
     */
    bool depthWriteEnable = true;

    /**
     * @brief Determines what operation should the renderer use when comparing
     *        previous and current depth value. If the operation passes, pixel
     *        with the current depth value will be considered visible.
     */
    COMPARE_FUNCTION::E depthComparisonFunc = COMPARE_FUNCTION::kLESS;

    /**
     * @brief If true then stencil buffer will also be updated when a pixel is
     *        written, and pixels will be tested against the stencil buffer
     *        before rendering.
     */
    bool stencilEnable = false;

    /**
     * @brief Mask to apply to any value read from the stencil buffer, before
     *        applying the stencil comparison function.
     */
    uint8 stencilReadMask = 0xFF;

    /**
     * @brief Mask to apply to any value about to be written in the stencil
     *        buffer.
     */
    uint8 stencilWriteMask = 0xFF;

    /**
     * @brief Operation that happens when stencil comparison function fails on
     *        a front facing polygon.
     */
    STENCIL_OPERATION::E frontStencilFailOp = STENCIL_OPERATION::kKEEP;

    /**
     * @brief Operation that happens when stencil comparison function passes
     *        but depth test fails on a front facing polygon.
     */
    STENCIL_OPERATION::E frontStencilZFailOp = STENCIL_OPERATION::kKEEP;

    /**
     * @brief Operation that happens when stencil comparison function passes on
     *        a front facing polygon.
     */
    STENCIL_OPERATION::E frontStencilPassOp = STENCIL_OPERATION::kKEEP;

    /**
     * @brief Stencil comparison function used for front facing polygons.
     *        Stencil buffer will be modified according to previously set
     *        stencil operations depending whether this comparison passes
     *        or fails.
     */
    COMPARE_FUNCTION::E frontStencilComparisonFunc = COMPARE_FUNCTION::kALWAYS_PASS;

    /**
     * @brief Operation that happens when stencil comparison function fails on
     *        a back facing polygon.
     */
    STENCIL_OPERATION::E backStencilFailOp = STENCIL_OPERATION::kKEEP;

    /**
     * @brief Operation that happens when stencil comparison function passes
     *        but depth test fails on a back facing polygon.
     */
    STENCIL_OPERATION::E backStencilZFailOp = STENCIL_OPERATION::kKEEP;

    /**
     * @brief Operation that happens when stencil comparison function passes
     *        on a back facing polygon.
     */
    STENCIL_OPERATION::E backStencilPassOp = STENCIL_OPERATION::kKEEP;

    /**
     * @brief Stencil comparison function used for back facing polygons.
     *        Stencil buffer will be modified according	to previously set
     *        stencil operations depending whether this comparison passes
     *        or fails.
     */
    COMPARE_FUNCTION::E backStencilComparisonFunc = COMPARE_FUNCTION::kALWAYS_PASS;
  };

  GE_ALLOW_MEMCPY_SERIALIZATION(DEPTH_STENCIL_STATE_DESC);

  /**
   * @brief Properties of DepthStencilState. Shared between sim and core thread
   *        versions of DepthStencilState.
   */
  class GE_CORE_EXPORT DepthStencilProperties
  {
   public:
    DepthStencilProperties(const DEPTH_STENCIL_STATE_DESC& desc);

    /**
     * @copydoc DEPTH_STENCIL_STATE_DESC::depthReadEnable
     */
    bool
    getDepthReadEnable() const {
      return m_data.depthReadEnable;
    }

    /**
     * @copydoc DEPTH_STENCIL_STATE_DESC::depthWriteEnable
     */
    bool
    getDepthWriteEnable() const {
      return m_data.depthWriteEnable;
    }

    /**
     * @copydoc DEPTH_STENCIL_STATE_DESC::depthComparisonFunc
     */
    COMPARE_FUNCTION::E
    getDepthComparisonFunc() const {
      return m_data.depthComparisonFunc;
    }

    /**
     * @copydoc DEPTH_STENCIL_STATE_DESC::stencilEnable
     */
    bool
    getStencilEnable() const {
      return m_data.stencilEnable;
    }

    /**
     * @copydoc DEPTH_STENCIL_STATE_DESC::stencilReadMask
     */
    uint8
    getStencilReadMask() const {
      return m_data.stencilReadMask;
    }

    /**
     * @copydoc DEPTH_STENCIL_STATE_DESC::stencilWriteMask
     */
    uint8
    getStencilWriteMask() const {
      return m_data.stencilWriteMask;
    }

    /**
     * @copydoc DEPTH_STENCIL_STATE_DESC::frontStencilFailOp
     */
    STENCIL_OPERATION::E
    getStencilFrontFailOp() const {
      return m_data.frontStencilFailOp;
    }

    /**
     * @copydoc DEPTH_STENCIL_STATE_DESC::frontStencilZFailOp
     */
    STENCIL_OPERATION::E
    getStencilFrontZFailOp() const {
      return m_data.frontStencilZFailOp;
    }

    /**
     * @copydoc DEPTH_STENCIL_STATE_DESC::frontStencilPassOp
     */
    STENCIL_OPERATION::E
    getStencilFrontPassOp() const {
      return m_data.frontStencilPassOp;
    }

    /**
     * @copydoc DEPTH_STENCIL_STATE_DESC::frontStencilComparisonFunc
     */
    COMPARE_FUNCTION::E
    getStencilFrontCompFunc() const {
      return m_data.frontStencilComparisonFunc;
    }

    /**
     * @copydoc DEPTH_STENCIL_STATE_DESC::backStencilFailOp
     */
    STENCIL_OPERATION::E
    getStencilBackFailOp() const {
      return m_data.backStencilFailOp;
    }

    /**
     * @copydoc DEPTH_STENCIL_STATE_DESC::backStencilZFailOp
     */
    STENCIL_OPERATION::E
    getStencilBackZFailOp() const {
      return m_data.backStencilZFailOp;
    }

    /**
     * @copydoc DEPTH_STENCIL_STATE_DESC::backStencilPassOp
     */
    STENCIL_OPERATION::E
    getStencilBackPassOp() const {
      return m_data.backStencilPassOp;
    }

    /**
     * @copydoc DEPTH_STENCIL_STATE_DESC::backStencilComparisonFunc
     */
    COMPARE_FUNCTION::E
    getStencilBackCompFunc() const {
      return m_data.backStencilComparisonFunc;
    }

    /**
     * @brief Returns the hash value generated from the depth-stencil state properties.
     */
    uint64
    getHash() const {
      return m_hash;
    }

   protected:
    friend class DepthStencilState;
    friend class geCoreThread::DepthStencilState;
    friend class DepthStencilStateRTTI;

    DEPTH_STENCIL_STATE_DESC m_data;
    uint64 m_hash;
  };

  /**
   * @brief Render system pipeline state that allows you to modify how an
   *        object is rendered. More exactly this state allows to you to
   *        control how are depth and stencil buffers modified upon rendering.
   * @note  Depth stencil states are immutable. Sim thread only.
   */
  class GE_CORE_EXPORT DepthStencilState
    : public IReflectable, public CoreObject
  {
   public:
    virtual ~DepthStencilState() = default;

    /**
     * @brief Returns information about the depth stencil state.
     */
    const DepthStencilProperties&
    getProperties() const;

    /**
     * @brief Retrieves a core implementation of a sampler state usable only
     *        from the core thread.
     */
    SPtr<geCoreThread::DepthStencilState>
    getCore() const;

    /**
     * @brief Creates a new depth stencil state using the specified depth
     *        stencil state description structure.
     */
    static SPtr<DepthStencilState>
    create(const DEPTH_STENCIL_STATE_DESC& desc);

    /**
     * @brief Returns the default depth stencil state that you may use when no
     *        other is available.
     */
    static const SPtr<DepthStencilState>&
    getDefault();

    /**
     * @brief Generates a hash value from a depth-stencil state descriptor.
     */
    static uint64
    generateHash(const DEPTH_STENCIL_STATE_DESC& desc);

   protected:
    friend class RenderStateManager;

    DepthStencilState(const DEPTH_STENCIL_STATE_DESC& desc);

    /**
     * @copydoc CoreObject::createCore
     */
    SPtr<geCoreThread::CoreObject>
    createCore() const override;

    DepthStencilProperties m_properties;
    mutable uint32 m_id = 0;

    /*************************************************************************/
    /**
     * Includes
     */
    /*************************************************************************/

   public:
    friend class DepthStencilStateRTTI;

    static RTTITypeBase*
    getRTTIStatic();

    RTTITypeBase*
    getRTTI() const override;
  };

  namespace geCoreThread {
    /**
     * @brief Core thread version of geEngineSDK::DepthStencilState.
     * @note  Core thread.
     */
    class GE_CORE_EXPORT DepthStencilState : public CoreObject
    {
     public:
      virtual ~DepthStencilState() = default;

      /**
       * @brief Returns information about the depth stencil state.
       */
      const DepthStencilProperties&
      getProperties() const;

      /**
       * @brief Returns a unique state ID. Only the lowest 10 bits are used.
       */
      uint32
      getId() const {
        return m_id;
      }

      /**
       * @brief Creates a new depth stencil state using the specified depth
       *        stencil state description structure.
       */
      static SPtr<DepthStencilState>
      create(const DEPTH_STENCIL_STATE_DESC& desc);

      /**
       * @brief Returns the default depth stencil state that you may use when
       *        no other is available.
       */
      static const SPtr<DepthStencilState>&
      getDefault();

     protected:
      friend class RenderStateManager;

      DepthStencilState(const DEPTH_STENCIL_STATE_DESC& desc, uint32 id);

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

      DepthStencilProperties m_properties;
      uint32 m_id;
    };
  }
}

namespace std {
  /**
   * @brief Hash value generator for DEPTH_STENCIL_STATE_DESC.
   */
  template<>
  struct hash<geEngineSDK::DEPTH_STENCIL_STATE_DESC>
  {
    size_t
    operator()(const geEngineSDK::DEPTH_STENCIL_STATE_DESC& value) const {
      return static_cast<size_t>(geEngineSDK::DepthStencilState::generateHash(value));
    }
  };
}
