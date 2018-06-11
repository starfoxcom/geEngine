/*****************************************************************************/
/**
 * @file    geRenderStateManager.cpp
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2018/06/09
 * @brief   Handles creation of various render states.
 *
 * Handles creation of various render states.
 *
 * @bug     No known bugs.
 */
/*****************************************************************************/

/*****************************************************************************/
/**
 * Includes
 */
/*****************************************************************************/
#include "geRenderStateManager.h"
#include "geSamplerState.h"
#include "geDepthStencilState.h"
#include "geRasterizerState.h"
#include "geBlendState.h"

namespace geEngineSDK {
  SPtr<SamplerState>
  RenderStateManager::createSamplerState(const SAMPLER_STATE_DESC& desc) const {
    SPtr<SamplerState> state = _createSamplerStatePtr(desc);
    state->initialize();

    return state;
  }

  SPtr<DepthStencilState>
  RenderStateManager::createDepthStencilState(const DEPTH_STENCIL_STATE_DESC& desc) const {
    SPtr<DepthStencilState> state = _createDepthStencilStatePtr(desc);
    state->initialize();

    return state;
  }

  SPtr<RasterizerState>
  RenderStateManager::createRasterizerState(const RASTERIZER_STATE_DESC& desc) const {
    SPtr<RasterizerState> state = _createRasterizerStatePtr(desc);
    state->initialize();

    return state;
  }

  SPtr<BlendState>
  RenderStateManager::createBlendState(const BLEND_STATE_DESC& desc) const {
    SPtr<BlendState> state = _createBlendStatePtr(desc);
    state->initialize();

    return state;
  }

  SPtr<GraphicsPipelineState>
  RenderStateManager::createGraphicsPipelineState(const PIPELINE_STATE_DESC& desc) const {
    SPtr<GraphicsPipelineState> state = _createGraphicsPipelineState(desc);
    state->initialize();

    return state;
  }

  SPtr<ComputePipelineState>
  RenderStateManager::createComputePipelineState(const SPtr<GPUProgram>& program) const {
    SPtr<ComputePipelineState> state = _createComputePipelineState(program);
    state->initialize();

    return state;
  }

  SPtr<SamplerState>
  RenderStateManager::_createSamplerStatePtr(const SAMPLER_STATE_DESC& desc) const {
    auto samplerState = ge_core_ptr<SamplerState>(GE_PVT_NEW(SamplerState, desc));
    samplerState->_setThisPtr(samplerState);

    return samplerState;
  }

  SPtr<DepthStencilState>
  RenderStateManager::_createDepthStencilStatePtr(const DEPTH_STENCIL_STATE_DESC& dsc) const {
    auto dpthStnclStt = ge_core_ptr<DepthStencilState>(GE_PVT_NEW(DepthStencilState, dsc));
    dpthStnclStt->_setThisPtr(dpthStnclStt);

    return dpthStnclStt;
  }

  SPtr<RasterizerState>
  RenderStateManager::_createRasterizerStatePtr(const RASTERIZER_STATE_DESC& desc) const {
    auto rasterizerState = ge_core_ptr<RasterizerState>(GE_PVT_NEW(RasterizerState, desc));
    rasterizerState->_setThisPtr(rasterizerState);

    return rasterizerState;
  }

  SPtr<BlendState>
  RenderStateManager::_createBlendStatePtr(const BLEND_STATE_DESC& desc) const {
    auto blendState = ge_core_ptr<BlendState>(GE_PVT_NEW(BlendState, desc));
    blendState->_setThisPtr(blendState);

    return blendState;
  }

  SPtr<GraphicsPipelineState>
  RenderStateManager::_createGraphicsPipelineState(const PIPELINE_STATE_DESC& desc) const {
    auto pipelineState =
      ge_core_ptr<GraphicsPipelineState>(GE_PVT_NEW(GraphicsPipelineState, desc));
    pipelineState->_setThisPtr(pipelineState);

    return pipelineState;
  }

  SPtr<ComputePipelineState>
  RenderStateManager::_createComputePipelineState(const SPtr<GPUProgram>& program) const {
    auto pipelineState =
      ge_core_ptr<ComputePipelineState>(GE_PVT_NEW(ComputePipelineState, program));
    pipelineState->_setThisPtr(pipelineState);

    return pipelineState;
  }

  const SPtr<SamplerState>&
  RenderStateManager::getDefaultSamplerState() const {
    if (nullptr == mDefaultSamplerState) {
      mDefaultSamplerState = createSamplerState(SAMPLER_STATE_DESC());
    }

    return mDefaultSamplerState;
  }

  const SPtr<BlendState>&
  RenderStateManager::getDefaultBlendState() const {
    if (nullptr == mDefaultBlendState) {
      mDefaultBlendState = createBlendState(BLEND_STATE_DESC());
    }

    return mDefaultBlendState;
  }

  const SPtr<RasterizerState>&
  RenderStateManager::getDefaultRasterizerState() const {
    if (nullptr == mDefaultRasterizerState) {
      mDefaultRasterizerState = createRasterizerState(RASTERIZER_STATE_DESC());
    }

    return mDefaultRasterizerState;
  }

  const SPtr<DepthStencilState>&
  RenderStateManager::getDefaultDepthStencilState() const {
    if (nullptr == mDefaultDepthStencilState) {
      mDefaultDepthStencilState = createDepthStencilState(DEPTH_STENCIL_STATE_DESC());
    }

    return mDefaultDepthStencilState;
  }

  namespace geCoreThread {
    SPtr<SamplerState>
    RenderStateManager::createSamplerState(const SAMPLER_STATE_DESC& desc,
                                           GPU_DEVICE_FLAGS::E deviceMask) const {
      SPtr<SamplerState> state = findCachedState(desc);
      if (nullptr == state) {
        state = createSamplerStateInternal(desc, deviceMask);
        state->initialize();

        notifySamplerStateCreated(desc, state);
      }

      return state;
    }

    SPtr<DepthStencilState>
    RenderStateManager::createDepthStencilState(const DEPTH_STENCIL_STATE_DESC& desc) const {
      uint32 id = 0;
      SPtr<DepthStencilState> state = findCachedState(desc, id);
      if (nullptr == state) {
        state = createDepthStencilStateInternal(desc, id);
        state->initialize();

        CachedDepthStencilState cachedData(id);
        cachedData.state = state;

        notifyDepthStencilStateCreated(desc, cachedData);
      }

      return state;
    }

    SPtr<RasterizerState>
    RenderStateManager::createRasterizerState(const RASTERIZER_STATE_DESC& desc) const {
      uint32 id = 0;
      SPtr<RasterizerState> state = findCachedState(desc, id);
      if (nullptr == state) {
        state = createRasterizerStateInternal(desc, id);
        state->initialize();

        CachedRasterizerState cachedData(id);
        cachedData.state = state;

        notifyRasterizerStateCreated(desc, cachedData);
      }

      return state;
    }

    SPtr<BlendState>
    RenderStateManager::createBlendState(const BLEND_STATE_DESC& desc) const {
      uint32 id = 0;
      SPtr<BlendState> state = findCachedState(desc, id);
      if (nullptr == state) {
        state = createBlendStateInternal(desc, id);
        state->initialize();

        CachedBlendState cachedData(id);
        cachedData.state = state;

        notifyBlendStateCreated(desc, cachedData);
      }

      return state;
    }

    SPtr<GraphicsPipelineState>
    RenderStateManager::createGraphicsPipelineState(const PIPELINE_STATE_DESC& desc,
                                                    GPU_DEVICE_FLAGS::E deviceMask) const {
      auto state = _createGraphicsPipelineState(desc, deviceMask);
      state->initialize();

      return state;
    }

    SPtr<ComputePipelineState>
    RenderStateManager::createComputePipelineState(const SPtr<GPUProgram>& program,
                                                   GPU_DEVICE_FLAGS::E deviceMask) const {
      auto state = _createComputePipelineState(program, deviceMask);
      state->initialize();

      return state;
    }

    SPtr<GPUPipelineParamInfo>
    RenderStateManager::createPipelineParamInfo(const GPU_PIPELINE_PARAMS_DESC& desc,
                                                GPU_DEVICE_FLAGS::E deviceMask) const {
      auto paramInfo = _createPipelineParamInfo(desc, deviceMask);
      paramInfo->initialize();

      return paramInfo;
    }

    SPtr<SamplerState>
    RenderStateManager::_createSamplerState(const SAMPLER_STATE_DESC& desc,
                                            GPU_DEVICE_FLAGS::E deviceMask) const {
      SPtr<SamplerState> state = findCachedState(desc);
      if (nullptr == state) {
        state = createSamplerStateInternal(desc, deviceMask);
        notifySamplerStateCreated(desc, state);
      }

      return state;
    }

    SPtr<DepthStencilState>
    RenderStateManager::_createDepthStencilState(const DEPTH_STENCIL_STATE_DESC& desc) const {
      uint32 id = 0;
      SPtr<DepthStencilState> state = findCachedState(desc, id);
      if (nullptr == state) {
        state = createDepthStencilStateInternal(desc, id);

        CachedDepthStencilState cachedData(id);
        cachedData.state = state;

        notifyDepthStencilStateCreated(desc, cachedData);
      }

      return state;
    }

    SPtr<RasterizerState>
    RenderStateManager::_createRasterizerState(const RASTERIZER_STATE_DESC& desc) const {
      uint32 id = 0;
      SPtr<RasterizerState> state = findCachedState(desc, id);
      if (nullptr == state) {
        state = createRasterizerStateInternal(desc, id);

        CachedRasterizerState cachedData(id);
        cachedData.state = state;

        notifyRasterizerStateCreated(desc, cachedData);
      }

      return state;
    }

    SPtr<BlendState>
    RenderStateManager::_createBlendState(const BLEND_STATE_DESC& desc) const {
      uint32 id = 0;
      SPtr<BlendState> state = findCachedState(desc, id);
      if (nullptr == state) {
        state = createBlendStateInternal(desc, id);

        CachedBlendState cachedData(id);
        cachedData.state = state;

        notifyBlendStateCreated(desc, cachedData);
      }

      return state;
    }

    SPtr<GraphicsPipelineState>
    RenderStateManager::_createGraphicsPipelineState(const PIPELINE_STATE_DESC& desc,
                                                     GPU_DEVICE_FLAGS::E deviceMask) const {
      auto pipelineState =
        ge_shared_ptr<GraphicsPipelineState>(GE_PVT_NEW(GraphicsPipelineState,
                                                        desc,
                                                        deviceMask));
      pipelineState->_setThisPtr(pipelineState);

      return pipelineState;
    }

    SPtr<ComputePipelineState>
    RenderStateManager::_createComputePipelineState(const SPtr<GPUProgram>& program,
                                                    GPU_DEVICE_FLAGS::E deviceMask) const {
      auto pipelineState =
        ge_shared_ptr<ComputePipelineState>(GE_PVT_NEW(ComputePipelineState,
                                                       program,
                                                       deviceMask));
      pipelineState->_setThisPtr(pipelineState);

      return pipelineState;
    }

    SPtr<GPUPipelineParamInfo>
    RenderStateManager::_createPipelineParamInfo(const GPU_PIPELINE_PARAMS_DESC& desc,
                                                 GPU_DEVICE_FLAGS::E deviceMask) const {
      auto paramInfo =
        ge_shared_ptr<GPUPipelineParamInfo>(GE_PVT_NEW(GPUPipelineParamInfo,
                                                       desc,
                                                       deviceMask));
      paramInfo->_setThisPtr(paramInfo);

      return paramInfo;
    }

    void
    RenderStateManager::onShutDown() {
      mDefaultBlendState = nullptr;
      mDefaultDepthStencilState = nullptr;
      mDefaultRasterizerState = nullptr;
      mDefaultSamplerState = nullptr;
    }

    const SPtr<SamplerState>&
    RenderStateManager::getDefaultSamplerState() const {
      if (nullptr == mDefaultSamplerState) {
        mDefaultSamplerState = createSamplerState(SAMPLER_STATE_DESC());
      }

      return mDefaultSamplerState;
    }

    const SPtr<BlendState>&
    RenderStateManager::getDefaultBlendState() const {
      if (nullptr == mDefaultBlendState) {
        mDefaultBlendState = createBlendState(BLEND_STATE_DESC());
      }

      return mDefaultBlendState;
    }

    const SPtr<RasterizerState>&
    RenderStateManager::getDefaultRasterizerState() const {
      if (nullptr == mDefaultRasterizerState) {
        mDefaultRasterizerState = createRasterizerState(RASTERIZER_STATE_DESC());
      }

      return mDefaultRasterizerState;
    }

    const SPtr<DepthStencilState>&
    RenderStateManager::getDefaultDepthStencilState() const {
      if (nullptr == mDefaultDepthStencilState) {
        mDefaultDepthStencilState = createDepthStencilState(DEPTH_STENCIL_STATE_DESC());
      }

      return mDefaultDepthStencilState;
    }

    void
    RenderStateManager::notifySamplerStateCreated(const SAMPLER_STATE_DESC& desc,
                                                  const SPtr<SamplerState>& state) const {
      Lock lock(mMutex);
      mCachedSamplerStates[desc] = state;
    }

    void
    RenderStateManager::notifyBlendStateCreated(const BLEND_STATE_DESC& desc,
                                                const CachedBlendState& state) const {
      Lock lock(mMutex);
      mCachedBlendStates[desc] = state;
    }

    void
    RenderStateManager::notifyRasterizerStateCreated(const RASTERIZER_STATE_DESC& desc,
                                                   const CachedRasterizerState& state) const {
      Lock lock(mMutex);
      mCachedRasterizerStates[desc] = state;
    }

    void
    RenderStateManager::notifyDepthStencilStateCreated(const DEPTH_STENCIL_STATE_DESC& desc,
                                                 const CachedDepthStencilState& state) const {
      Lock lock(mMutex);
      mCachedDepthStencilStates[desc] = state;
    }

    void
    RenderStateManager::notifySamplerStateDestroyed(const SAMPLER_STATE_DESC& desc) const {
      Lock lock(mMutex);
      mCachedSamplerStates.erase(desc);
    }

    SPtr<SamplerState>
    RenderStateManager::findCachedState(const SAMPLER_STATE_DESC& desc) const {
      Lock lock(mMutex);

      auto iterFind = mCachedSamplerStates.find(desc);
      if (mCachedSamplerStates.end() != iterFind) {
        return iterFind->second.lock();
      }

      return nullptr;
    }

    SPtr<BlendState>
    RenderStateManager::findCachedState(const BLEND_STATE_DESC& desc, uint32& id) const {
      Lock lock(mMutex);

      auto iterFind = mCachedBlendStates.find(desc);
      if (mCachedBlendStates.end() != iterFind) {
        id = iterFind->second.id;

        if (!iterFind->second.state.expired()) {
          return iterFind->second.state.lock();
        }

        return nullptr;
      }

      id = mNextBlendStateId++;
      GE_ASSERT(0x3FF >= id); //10 bits maximum

      return nullptr;
    }

    SPtr<RasterizerState>
    RenderStateManager::findCachedState(const RASTERIZER_STATE_DESC& desc, uint32& id) const {
      Lock lock(mMutex);

      auto iterFind = mCachedRasterizerStates.find(desc);
      if (mCachedRasterizerStates.end() != iterFind) {
        id = iterFind->second.id;

        if (!iterFind->second.state.expired()) {
          return iterFind->second.state.lock();
        }

        return nullptr;
      }

      id = mNextRasterizerStateId++;
      GE_ASSERT(0x3FF >= id); //10 bits maximum

      return nullptr;
    }

    SPtr<DepthStencilState>
    RenderStateManager::findCachedState(const DEPTH_STENCIL_STATE_DESC& desc,
                                        uint32& id) const {
      Lock lock(mMutex);

      auto iterFind = mCachedDepthStencilStates.find(desc);
      if (mCachedDepthStencilStates.end() != iterFind) {
        id = iterFind->second.id;

        if (!iterFind->second.state.expired()) {
          return iterFind->second.state.lock();
        }

        return nullptr;
      }

      id = mNextDepthStencilStateId++;
      GE_ASSERT(0x3FF >= id); //10 bits maximum

      return nullptr;
    }

    SPtr<SamplerState>
    RenderStateManager::createSamplerStateInternal(const SAMPLER_STATE_DESC& desc,
                                                   GPU_DEVICE_FLAGS::E deviceMask) const {
      auto state = ge_shared_ptr<SamplerState>(GE_PVT_NEW(SamplerState, desc, deviceMask));
      state->_setThisPtr(state);

      return state;
    }

    SPtr<DepthStencilState>
    RenderStateManager::createDepthStencilStateInternal(const DEPTH_STENCIL_STATE_DESC& desc,
                                                        uint32 id) const {
      auto state = ge_shared_ptr<DepthStencilState>(GE_PVT_NEW(DepthStencilState, desc, id));
      state->_setThisPtr(state);

      return state;
    }

    SPtr<RasterizerState>
    RenderStateManager::createRasterizerStateInternal(const RASTERIZER_STATE_DESC& desc,
                                                      uint32 id) const {
      auto state = ge_shared_ptr<RasterizerState>(GE_PVT_NEW(RasterizerState, desc, id));
      state->_setThisPtr(state);

      return state;
    }

    SPtr<BlendState>
    RenderStateManager::createBlendStateInternal(const BLEND_STATE_DESC& desc,
                                                 uint32 id) const {
      auto state = ge_shared_ptr<BlendState>(GE_PVT_NEW(BlendState, desc, id));
      state->_setThisPtr(state);

      return state;
    }
  }
}
