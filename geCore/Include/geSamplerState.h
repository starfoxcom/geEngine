/*****************************************************************************/
/**
 * @file    geSamplerState.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2018/06/09
 * @brief   Class representing the state of a texture sampler.
 *
 * Class representing the state of a texture sampler.
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

#include <geNumericLimits.h>
#include <geColor.h>
#include <geIReflectable.h>

namespace geEngineSDK {
  /**
   * @brief Structure used for initializing a SamplerState.
   * @see   SamplerState
   */
  struct GE_CORE_EXPORT SAMPLER_STATE_DESC
  {
    SAMPLER_STATE_DESC()
      : minFilter(FILTER_OPTIONS::kLINEAR),
        magFilter(FILTER_OPTIONS::kLINEAR),
        mipFilter(FILTER_OPTIONS::kLINEAR),
        maxAniso(0),
        mipmapBias(0),
        mipMin(-NumLimit::MAX_FLOAT),
        mipMax(NumLimit::MAX_FLOAT),
        borderColor(LinearColor::White),
        comparisonFunc(COMPARE_FUNCTION::kALWAYS_PASS)
    {}

    bool
    operator==(const SAMPLER_STATE_DESC& rhs) const;

    /**
     * @brief Determines how are texture coordinates outside of [0, 1]
     *        range handled.
     */
    UVWAddressingMode addressMode;

    /**
     * @brief Filtering used when texture is displayed as smaller than its
     *        original size.
     */
    FILTER_OPTIONS::E minFilter;

    /**
     * @brief Filtering used when texture is displayed as larger than its
     *        original size.
     */
    FILTER_OPTIONS::E magFilter;

    /**
     * @brief Filtering used to blend between the different mip levels.
     */
    FILTER_OPTIONS::E mipFilter;

    /**
     * @brief Maximum number of samples if anisotropic filtering is enabled.
     *        Max is 16.
     */
    uint32 maxAniso;

    /**
     * @brief Mipmap bias allows you to adjust the mipmap selection
     *        calculation. Negative values force a larger mipmap to be used,
     *        and positive values smaller. Units are in values of mip levels,
     *        so -1 means use a mipmap one level higher than default.
     */
    float mipmapBias;

    /**
     * @brief Minimum mip-map level that is allowed to be displayed.
     */
    float mipMin;

    /**
     * @brief Maximum mip-map level that is allowed to be displayed.
     *        Set to NumLimit::MAX_FLOAT for no limit.
     */
    float mipMax;

    /**
     * @brief Border color to use when using border addressing mode as
     *        specified by @p addressMode.
     */
    LinearColor borderColor;

    /**
     * @brief Function that compares sampled data with existing sampled data.
     */
    COMPARE_FUNCTION::E comparisonFunc;
  };

  /**
   * @brief Properties of SamplerState. Shared between sim and core thread
   *        versions of SamplerState.
   */
  class GE_CORE_EXPORT SamplerProperties
  {
   public:
    SamplerProperties(const SAMPLER_STATE_DESC& desc);

    /**
     * @brief Returns texture addressing mode for each possible texture
     *        coordinate. Addressing modes determine how are texture
     *        coordinates outside of [0, 1] range handled.
     */
    const UVWAddressingMode&
    getTextureAddressingMode() const {
      return m_data.addressMode;
    }

    /**
     * @brief Gets the filtering used when sampling from a texture.
     */
    FILTER_OPTIONS::E
    getTextureFiltering(FILTER_TYPE::E ftpye) const;

    /**
     * @brief Gets the anisotropy level. Higher anisotropy means better
     *        filtering for textures displayed on an angled slope relative to
     *        the viewer.
     */
    uint32
    getTextureAnisotropy() const {
      return m_data.maxAniso;
    }

    /**
     * @brief Gets a function that compares sampled data with existing sampled
     *        data.
     */
    COMPARE_FUNCTION::E
    getComparisonFunction() const {
      return m_data.comparisonFunc;
    }

    /**
     * @brief Mipmap bias allows you to adjust the mipmap selection
     *        calculation. Negative values force a larger mipmap to be used,
     *        and positive values smaller. Units are in values of mip levels,
     *        so -1 means use a mipmap one level higher than default.
     */
    float
    getTextureMipmapBias() const {
      return m_data.mipmapBias;
    }

    /**
     * @brief Returns the minimum mip map level.
     */
    float
    getMinimumMip() const {
      return m_data.mipMin;
    }

    /**
     * @brief Returns the maximum mip map level.
     */
    float
    getMaximumMip() const {
      return m_data.mipMax;
    }

    /**
     * @brief Gets the border color that will be used when border texture
     *        addressing is used and texture address is outside of the valid
     *        range.
     */
    const LinearColor&
    getBorderColor() const;

    /**
     * @brief Returns the hash value generated from the sampler state
     *        properties.
     */
    uint64
    getHash() const {
      return m_hash;
    }

    /**
     * @brief Returns the descriptor originally used for creating the sampler
     *        state.
     */
    SAMPLER_STATE_DESC
    getDesc() const {
      return m_data;
    }

   protected:
    friend class SamplerState;
    friend class geCoreThread::SamplerState;
    friend class SamplerStateRTTI;

    SAMPLER_STATE_DESC m_data;
    uint64 m_hash;
  };

  /**
   * @brief Class representing the state of a texture sampler.
   * @note  Sampler units are used for retrieving and filtering data from
   *        textures set in a GPU program. Sampler states are immutable.
   * @note  Sim thread.
   */
  class GE_CORE_EXPORT SamplerState : public IReflectable, public CoreObject
  {
   public:
    virtual ~SamplerState() = default;

    /**
     * @brief Returns information about the sampler state.
     */
    const SamplerProperties&
    getProperties() const;

    /**
     * @brief Retrieves a core implementation of the sampler state usable only
     *        from the core thread.
     */
    SPtr<geCoreThread::SamplerState>
    getCore() const;

    /**
     * @brief Creates a new sampler state using the provided descriptor
     *        structure.
     */
    static SPtr<SamplerState>
    create(const SAMPLER_STATE_DESC& desc);

    /**
     * @brief Returns the default sampler state.
     */
    static const SPtr<SamplerState>&
    getDefault();

    /**
     * @brief Generates a hash value from a sampler state descriptor.
     */
    static uint64
    generateHash(const SAMPLER_STATE_DESC& desc);

   protected:
    SamplerState(const SAMPLER_STATE_DESC& desc);

    /**
     * @copydoc CoreObject::createCore
     */
    SPtr<geCoreThread::CoreObject>
    createCore() const override;

    SamplerProperties m_properties;

    friend class RenderStateManager;

    /*************************************************************************/
    /**
     * RTTI
     */
    /*************************************************************************/

   public:
    friend class SamplerStateRTTI;

    static RTTITypeBase*
    getRTTIStatic();

    RTTITypeBase*
    getRTTI() const override;
  };

  namespace geCoreThread {
    /**
     * @brief Core thread version of geEngineSDK::SamplerState.
     * @note  Core thread.
     */
    class GE_CORE_EXPORT SamplerState : public CoreObject
    {
     public:
      virtual ~SamplerState();

      /**
       * @brief Returns information about the sampler state.
       */
      const SamplerProperties&
      getProperties() const;

      /**
       * @copydoc RenderStateManager::createSamplerState
       */
      static SPtr<SamplerState>
      create(const SAMPLER_STATE_DESC& desc,
             GPU_DEVICE_FLAGS::E deviceMask = GPU_DEVICE_FLAGS::kDEFAULT);

      /**
       * @brief Returns the default sampler state.
       */
      static const SPtr<SamplerState>&
      getDefault();

     protected:
      friend class RenderStateManager;

      SamplerState(const SAMPLER_STATE_DESC& desc,
                   GPU_DEVICE_FLAGS::E deviceMask);

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

      SamplerProperties m_properties;
    };
  }
}

namespace std {
  /**
   * @brief Hash value generator for SAMPLER_STATE_DESC.
   */
  template<>
  struct hash<geEngineSDK::SAMPLER_STATE_DESC>
  {
    size_t
    operator()(const geEngineSDK::SAMPLER_STATE_DESC& value) const {
      return static_cast<size_t>(geEngineSDK::SamplerState::generateHash(value));
    }
  };
}