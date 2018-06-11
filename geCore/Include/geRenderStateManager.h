/*****************************************************************************/
/**
 * @file    geRenderStateManager.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2018/06/09
 * @brief   Handles creation of various render states.
 *
 * Handles creation of various render states.
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
#include "geBlendState.h"
#include "geRasterizerState.h"
#include "geDepthStencilState.h"
#include "geSamplerState.h"
#include "geGPUPipelineState.h"
#include "geGPUPipelineParamInfo.h"

#include <geModule.h>

namespace geEngineSDK {
  using std::weak_ptr;

  class GE_CORE_EXPORT RenderStateManager : public Module <RenderStateManager>
  {
   public:
    /**
     * @brief Creates and initializes a new SamplerState.
     * @param[in] desc  Object describing the sampler state to create.
     */
    SPtr<SamplerState>
    createSamplerState(const SAMPLER_STATE_DESC& desc) const;

    /**
     * @brief Creates and initializes a new DepthStencilState.
     */
    SPtr<DepthStencilState>
    createDepthStencilState(const DEPTH_STENCIL_STATE_DESC& desc) const;

    /**
     * @brief Creates and initializes a new RasterizerState.
     */
    SPtr<RasterizerState>
    createRasterizerState(const RASTERIZER_STATE_DESC& desc) const;

    /**
     * @brief Creates and initializes a new BlendState.
     */
    SPtr<BlendState>
    createBlendState(const BLEND_STATE_DESC& desc) const;

    /**
     * @brief Creates and initializes a new GraphicsPipelineState.
     * @param[in] desc  Object describing the pipeline to create.
     */
    SPtr<GraphicsPipelineState>
    createGraphicsPipelineState(const PIPELINE_STATE_DESC& desc) const;

    /**
     * @brief Creates and initializes a new ComputePipelineState.
     * @param[in] program Compute GPU program to be executed by the pipeline.
     */
    SPtr<ComputePipelineState>
    createComputePipelineState(const SPtr<GPUProgram>& program) const;

    /**
     * @brief Creates an uninitialized sampler state.
     *        Requires manual initialization after creation.
     */
    SPtr<SamplerState>
    _createSamplerStatePtr(const SAMPLER_STATE_DESC& desc) const;

    /**
     * @brief Creates an uninitialized depth-stencil state.
     *        Requires manual initialization after creation.
     */
    SPtr<DepthStencilState>
    _createDepthStencilStatePtr(const DEPTH_STENCIL_STATE_DESC& desc) const;

    /**
     * @brief Creates an uninitialized rasterizer state.
     *        Requires manual initialization after creation.
     */
    SPtr<RasterizerState>
    _createRasterizerStatePtr(const RASTERIZER_STATE_DESC& desc) const;

    /**
     * @brief Creates an uninitialized blend state.
     *        Requires manual initialization after creation.
     */
    SPtr<BlendState>
    _createBlendStatePtr(const BLEND_STATE_DESC& desc) const;

    /**
     * @brief Creates an uninitialized GraphicsPipelineState.
     *        Requires manual initialization after creation.
     */
    virtual SPtr<GraphicsPipelineState>
    _createGraphicsPipelineState(const PIPELINE_STATE_DESC& desc) const;

    /**
     * @brief Creates an uninitialized ComputePipelineState.
     *        Requires manual initialization after creation.
     */
    virtual SPtr<ComputePipelineState>
    _createComputePipelineState(const SPtr<GPUProgram>& program) const;

    /**
     * @brief Gets a sampler state initialized with default options.
     */
    const SPtr<SamplerState>&
    getDefaultSamplerState() const;

    /**
     * @brief Gets a blend state initialized with default options.
     */
    const SPtr<BlendState>&
    getDefaultBlendState() const;

    /**
     * @brief Gets a rasterizer state initialized with default options.
     */
    const SPtr<RasterizerState>&
    getDefaultRasterizerState() const;

    /**
     * @brief Gets a depth stencil state initialized with default options.
     */
    const SPtr<DepthStencilState>&
    getDefaultDepthStencilState() const;

   private:
    friend class SamplerState;
    friend class BlendState;
    friend class RasterizerState;
    friend class DepthStencilState;

    mutable SPtr<SamplerState> mDefaultSamplerState;
    mutable SPtr<BlendState> mDefaultBlendState;
    mutable SPtr<RasterizerState> mDefaultRasterizerState;
    mutable SPtr<DepthStencilState> mDefaultDepthStencilState;
  };

  namespace geCoreThread {
    /**
     * @brief Handles creation of various render states.
     */
    class GE_CORE_EXPORT RenderStateManager : public Module<RenderStateManager>
    {
     private:
      /**
       * @brief Contains data about a cached blend state.
       */
      struct CachedBlendState
      {
        CachedBlendState() : id(0) {}
        CachedBlendState(uint32 id) : id(id) {}

        weak_ptr<BlendState> state;
        uint32 id;
      };

      /**
       * @brief Contains data about a cached rasterizer state.
       */
      struct CachedRasterizerState
      {
        CachedRasterizerState() : id(0) {}
        CachedRasterizerState(uint32 id) : id(id) {}

        weak_ptr<RasterizerState> state;
        uint32 id;
      };

      /**
       * @brief Contains data about a cached depth stencil state.
       */
      struct CachedDepthStencilState
      {
        CachedDepthStencilState() : id(0) {}
        CachedDepthStencilState(uint32 id) : id(id) {}

        weak_ptr<DepthStencilState> state;
        uint32 id;
      };

     public:
      RenderStateManager() = default;

      /**
       * @copydoc geEngineSDK::RenderStateManager::createSamplerState
       * @param[in] deviceMask  Mask that determines on which GPU devices
       *                        should the object be created on.
       */
      SPtr<SamplerState>
      createSamplerState(const SAMPLER_STATE_DESC& desc,
                         GPU_DEVICE_FLAGS::E deviceMask =
                           GPU_DEVICE_FLAGS::kDEFAULT) const;

      /**
       * @copydoc geEngineSDK::RenderStateManager::createDepthStencilState
       */
      SPtr<DepthStencilState>
      createDepthStencilState(const DEPTH_STENCIL_STATE_DESC& desc) const;

      /**
       * @copydoc geEngineSDK::RenderStateManager::createRasterizerState
       */
      SPtr<RasterizerState>
      createRasterizerState(const RASTERIZER_STATE_DESC& desc) const;

      /**
       * @copydoc geEngineSDK::RenderStateManager::createBlendState
       */
      SPtr<BlendState>
      createBlendState(const BLEND_STATE_DESC& desc) const;

      /**
       * @copydoc geEngineSDK::RenderStateManager::createGraphicsPipelineState
       * @param[in] deviceMask  Mask that determines on which GPU devices
       *                        should the object be created on.
       */
      SPtr<GraphicsPipelineState>
      createGraphicsPipelineState(const PIPELINE_STATE_DESC& desc,
                                  GPU_DEVICE_FLAGS::E deviceMask =
                                    GPU_DEVICE_FLAGS::kDEFAULT) const;

      /**
       * @copydoc geEngineSDK::RenderStateManager::createComputePipelineState
       * @param[in] deviceMask  Mask that determines on which GPU devices
       *                        should the object be created on.
       */
      SPtr<ComputePipelineState>
      createComputePipelineState(const SPtr<GPUProgram>& program,
                                 GPU_DEVICE_FLAGS::E deviceMask =
                                   GPU_DEVICE_FLAGS::kDEFAULT) const;

      /**
       * @copydoc GPUPipelineParamInfo::create
       */
      SPtr<GPUPipelineParamInfo>
      createPipelineParamInfo(const GPU_PIPELINE_PARAMS_DESC& desc,
                              GPU_DEVICE_FLAGS::E deviceMask =
                                GPU_DEVICE_FLAGS::kDEFAULT) const;

      /**
       * @brief Creates an uninitialized sampler state.
       *        Requires manual initialization after creation.
       */
      SPtr<SamplerState>
      _createSamplerState(const SAMPLER_STATE_DESC& desc,
                          GPU_DEVICE_FLAGS::E deviceMask =
                            GPU_DEVICE_FLAGS::kDEFAULT) const;

      /**
       * @brief Creates an uninitialized depth-stencil state.
       *        Requires manual initialization after creation.
       */
      SPtr<DepthStencilState>
      _createDepthStencilState(const DEPTH_STENCIL_STATE_DESC& desc) const;

      /**
       * @brief Creates an uninitialized rasterizer state.
       *        Requires manual initialization after creation.
       */
      SPtr<RasterizerState>
      _createRasterizerState(const RASTERIZER_STATE_DESC& desc) const;

      /**
       * @brief Creates an uninitialized blend state.
       *        Requires manual initialization after creation.
       */
      SPtr<BlendState>
      _createBlendState(const BLEND_STATE_DESC& desc) const;

      /**
       * @brief Creates an uninitialized GraphicsPipelineState.
       *        Requires manual initialization after creation.
       */
      virtual SPtr<GraphicsPipelineState>
      _createGraphicsPipelineState(const PIPELINE_STATE_DESC& desc,
                                   GPU_DEVICE_FLAGS::E deviceMask =
                                     GPU_DEVICE_FLAGS::kDEFAULT) const;

      /**
       * @brief Creates an uninitialized ComputePipelineState.
       *        Requires manual initialization after creation.
       */
      virtual SPtr<ComputePipelineState>
      _createComputePipelineState(const SPtr<GPUProgram>& program,
                                  GPU_DEVICE_FLAGS::E deviceMask =
                                    GPU_DEVICE_FLAGS::kDEFAULT) const;

      /**
       * @brief Creates an uninitialized GPUPipelineParamInfo.
       *        Requires manual initialization after creation.
       */
      virtual SPtr<GPUPipelineParamInfo>
      _createPipelineParamInfo(const GPU_PIPELINE_PARAMS_DESC& desc,
                               GPU_DEVICE_FLAGS::E deviceMask =
                                 GPU_DEVICE_FLAGS::kDEFAULT) const;

      /**
       * @brief Gets a sampler state initialized with default options.
       */
      const SPtr<SamplerState>&
      getDefaultSamplerState() const;

      /**
       * @brief Gets a blend state initialized with default options.
       */
      const SPtr<BlendState>&
      getDefaultBlendState() const;

      /**
       * @brief Gets a rasterizer state initialized with default options.
       */
      const SPtr<RasterizerState>&
      getDefaultRasterizerState() const;

      /**
       * @brief Gets a depth stencil state initialized with default options.
       */
      const SPtr<DepthStencilState>&
      getDefaultDepthStencilState() const;

     protected:
      friend class geEngineSDK::SamplerState;
      friend class geEngineSDK::BlendState;
      friend class geEngineSDK::RasterizerState;
      friend class geEngineSDK::DepthStencilState;
      friend class SamplerState;
      friend class BlendState;
      friend class RasterizerState;
      friend class DepthStencilState;

      /**
       * @copydoc Module::onShutDown
       */
      void
      onShutDown() override;

      /**
       * @copydoc createSamplerState
       */
      virtual SPtr<SamplerState>
      createSamplerStateInternal(const SAMPLER_STATE_DESC& desc,
                                 GPU_DEVICE_FLAGS::E deviceMask) const;

      /**
       * @copydoc createBlendState
       */
      virtual SPtr<BlendState>
      createBlendStateInternal(const BLEND_STATE_DESC& desc, uint32 id) const;

      /**
       * @copydoc createRasterizerState
       */
      virtual SPtr<RasterizerState>
      createRasterizerStateInternal(const RASTERIZER_STATE_DESC& desc,
                                    uint32 id) const;

      /**
       * @copydoc createDepthStencilState
       */
      virtual SPtr<DepthStencilState>
      createDepthStencilStateInternal(const DEPTH_STENCIL_STATE_DESC& desc,
                                      uint32 id) const;

     private:
      /**
       * @brief Triggered when a new sampler state is created.
       */
      void
      notifySamplerStateCreated(const SAMPLER_STATE_DESC& desc,
                                const SPtr<SamplerState>& state) const;

      /**
       * @brief Triggered when a new sampler state is created.
       */
      void
      notifyBlendStateCreated(const BLEND_STATE_DESC& desc,
                              const CachedBlendState& state) const;

      /**
       * @brief Triggered when a new sampler state is created.
       */
      void
      notifyRasterizerStateCreated(const RASTERIZER_STATE_DESC& desc,
                                   const CachedRasterizerState& state) const;

      /**
       * @brief Triggered when a new sampler state is created.
       */
      void
      notifyDepthStencilStateCreated(const DEPTH_STENCIL_STATE_DESC& desc,
                                     const CachedDepthStencilState& state) const;

      /**
       * @brief Triggered when the last reference to a specific sampler state
       *        is destroyed, which means we must clear our cached version as
       *        well.
       */
      void
      notifySamplerStateDestroyed(const SAMPLER_STATE_DESC& desc) const;

      /**
       * @brief Attempts to find a cached sampler state corresponding to the
       *        provided descriptor.
       *        Returns null if one doesn't exist.
       */
      SPtr<SamplerState>
      findCachedState(const SAMPLER_STATE_DESC& desc) const;

      /**
       * @brief Attempts to find a cached blend state corresponding to the
       *        provided descriptor.
       *        Returns null if one doesn't exist.
       */
      SPtr<BlendState>
      findCachedState(const BLEND_STATE_DESC& desc, uint32& id) const;

      /**
       * @brief Attempts to find a cached rasterizer state corresponding to the
       *        provided descriptor.
       *        Returns null if one doesn't exist.
       */
      SPtr<RasterizerState>
      findCachedState(const RASTERIZER_STATE_DESC& desc, uint32& id) const;

      /**
       * @brief Attempts to find a cached depth-stencil state corresponding to
       *        the provided descriptor.
       *        Returns null if one doesn't exist.
       */
      SPtr<DepthStencilState>
      findCachedState(const DEPTH_STENCIL_STATE_DESC& desc, uint32& id) const;

      mutable SPtr<SamplerState> mDefaultSamplerState;
      mutable SPtr<BlendState> mDefaultBlendState;
      mutable SPtr<RasterizerState> mDefaultRasterizerState;
      mutable SPtr<DepthStencilState> mDefaultDepthStencilState;

      mutable UnorderedMap<SAMPLER_STATE_DESC, std::weak_ptr<SamplerState>> mCachedSamplerStates;
      mutable UnorderedMap<BLEND_STATE_DESC, CachedBlendState> mCachedBlendStates;
      mutable UnorderedMap<RASTERIZER_STATE_DESC, CachedRasterizerState> mCachedRasterizerStates;
      mutable UnorderedMap<DEPTH_STENCIL_STATE_DESC, CachedDepthStencilState> mCachedDepthStencilStates;

      mutable uint32 mNextBlendStateId = 0;
      mutable uint32 mNextRasterizerStateId = 0;
      mutable uint32 mNextDepthStencilStateId = 0;

      mutable Mutex mMutex;
    };
  }
}
