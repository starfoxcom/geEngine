/*****************************************************************************/
/**
 * @file    geGPUPipelineState.cpp
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2018/06/10
 * @brief   State of the GPU pipeline that determines how are primitives drawn.
 *
 * Describes the state of the GPU pipeline that determines how are primitives
 * rendered. It consists of programmable states (vertex, fragment, geometry,
 * etc. GPU programs), as well as a set of fixed states (blend, rasterizer,
 * depth-stencil). Once created the state is immutable, and can be bound to
 * RenderAPI for rendering.
 *
 * @bug     No known bugs.
 */
/*****************************************************************************/

/*****************************************************************************/
/**
 * Includes
 */
/*****************************************************************************/
#include "geGPUPipelineState.h"
#include "geRasterizerState.h"
#include "geBlendState.h"
#include "geDepthStencilState.h"
#include "geGPUProgram.h"
#include "geGPUParamDesc.h"
#include "geGPUPipelineParamInfo.h"
#include "geRenderStateManager.h"

namespace geEngineSDK {
  using std::static_pointer_cast;

  /**
   * @brief Converts a sim thread pipeline state descriptor to a core thread
   *        one.
   */
  void
  convertPassDesc(const PIPELINE_STATE_DESC& input,
                  geCoreThread::PIPELINE_STATE_DESC& output) {
    output.blendState = nullptr != input.blendState ?
      input.blendState->getCore() : nullptr;

    output.rasterizerState = nullptr != input.rasterizerState ?
      input.rasterizerState->getCore() : nullptr;

    output.depthStencilState = nullptr != input.depthStencilState ?
      input.depthStencilState->getCore() : nullptr;

    output.vertexProgram = nullptr != input.vertexProgram ?
      input.vertexProgram->getCore() : nullptr;

    output.fragmentProgram = nullptr != input.fragmentProgram ?
      input.fragmentProgram->getCore() : nullptr;

    output.geometryProgram = nullptr != input.geometryProgram ?
      input.geometryProgram->getCore() : nullptr;

    output.hullProgram = nullptr != input.hullProgram ?
      input.hullProgram->getCore() : nullptr;

    output.domainProgram = nullptr != input.domainProgram ?
      input.domainProgram->getCore() : nullptr;
  }

  template<bool Core>
  TGraphicsPipelineState<Core>::TGraphicsPipelineState() {}

  template<bool Core>
  TGraphicsPipelineState<Core>::TGraphicsPipelineState(const StateDescType& data)
    : m_data(data)
  {}

  template class TGraphicsPipelineState<false>;
  template class TGraphicsPipelineState<true>;

  GraphicsPipelineState::GraphicsPipelineState(const PIPELINE_STATE_DESC& desc)
    : TGraphicsPipelineState(desc) {
    GPU_PIPELINE_PARAMS_DESC paramsDesc;

    if (nullptr != desc.vertexProgram) {
      desc.vertexProgram->blockUntilCoreInitialized();
      paramsDesc.vertexParams = desc.vertexProgram->getParamDesc();
    }

    if (nullptr != desc.fragmentProgram) {
      desc.fragmentProgram->blockUntilCoreInitialized();
      paramsDesc.fragmentParams = desc.fragmentProgram->getParamDesc();
    }

    if (nullptr != desc.geometryProgram) {
      desc.geometryProgram->blockUntilCoreInitialized();
      paramsDesc.geometryParams = desc.geometryProgram->getParamDesc();
    }

    if (nullptr != desc.hullProgram) {
      desc.hullProgram->blockUntilCoreInitialized();
      paramsDesc.hullParams = desc.hullProgram->getParamDesc();
    }

    if (nullptr != desc.domainProgram) {
      desc.domainProgram->blockUntilCoreInitialized();
      paramsDesc.domainParams = desc.domainProgram->getParamDesc();
    }

    m_paramInfo = GPUPipelineParamInfo::create(paramsDesc);
  }

  SPtr<geCoreThread::GraphicsPipelineState>
  GraphicsPipelineState::getCore() const {
    return static_pointer_cast<geCoreThread::GraphicsPipelineState>(m_coreSpecific);
  }

  SPtr<geCoreThread::CoreObject>
  GraphicsPipelineState::createCore() const {
    geCoreThread::PIPELINE_STATE_DESC desc;
    convertPassDesc(m_data, desc);
    return geCoreThread::RenderStateManager::instance()._createGraphicsPipelineState(desc);
  }

  SPtr<GraphicsPipelineState>
  GraphicsPipelineState::create(const PIPELINE_STATE_DESC& desc) {
    return RenderStateManager::instance().createGraphicsPipelineState(desc);
  }

  template<bool Core>
  TComputePipelineState<Core>::TComputePipelineState() {}

  template<bool Core>
  TComputePipelineState<Core>::TComputePipelineState(const GPUProgramType& program)
    : m_program(program)
  {}

  template class TComputePipelineState<false>;
  template class TComputePipelineState<true>;

  ComputePipelineState::ComputePipelineState(const SPtr<GPUProgram>& program)
    : TComputePipelineState(program) {
    GPU_PIPELINE_PARAMS_DESC paramsDesc;
    program->blockUntilCoreInitialized();
    paramsDesc.computeParams = program->getParamDesc();
    m_paramInfo = GPUPipelineParamInfo::create(paramsDesc);
  }

  SPtr<geCoreThread::ComputePipelineState>
  ComputePipelineState::getCore() const {
    return static_pointer_cast<geCoreThread::ComputePipelineState>(m_coreSpecific);
  }

  SPtr<geCoreThread::CoreObject>
  ComputePipelineState::createCore() const {
    return geCoreThread::RenderStateManager::instance().
      _createComputePipelineState(m_program->getCore());
  }

  SPtr<ComputePipelineState>
  ComputePipelineState::create(const SPtr<GPUProgram>& program) {
    return RenderStateManager::instance().createComputePipelineState(program);
  }

  namespace geCoreThread {
    GraphicsPipelineState::GraphicsPipelineState(const PIPELINE_STATE_DESC& desc,
                                                 GPU_DEVICE_FLAGS::E deviceMask)
      : TGraphicsPipelineState(desc),
        m_deviceMask(deviceMask)
    {}

    void
    GraphicsPipelineState::initialize() {
      GPU_PIPELINE_PARAMS_DESC paramsDesc;

      if (nullptr != m_data.vertexProgram) {
        paramsDesc.vertexParams = m_data.vertexProgram->getParamDesc();
      }

      if (nullptr != m_data.fragmentProgram) {
        paramsDesc.fragmentParams = m_data.fragmentProgram->getParamDesc();
      }

      if (nullptr != m_data.geometryProgram) {
        paramsDesc.geometryParams = m_data.geometryProgram->getParamDesc();
      }

      if (nullptr != m_data.hullProgram) {
        paramsDesc.hullParams = m_data.hullProgram->getParamDesc();
      }

      if (nullptr != m_data.domainProgram) {
        paramsDesc.domainParams = m_data.domainProgram->getParamDesc();
      }

      m_paramInfo = GPUPipelineParamInfo::create(paramsDesc, m_deviceMask);

      CoreObject::initialize();
    }

    SPtr<GraphicsPipelineState>
    GraphicsPipelineState::create(const PIPELINE_STATE_DESC& desc,
                                  GPU_DEVICE_FLAGS::E deviceMask) {
      return RenderStateManager::instance().createGraphicsPipelineState(desc, deviceMask);
    }

    ComputePipelineState::ComputePipelineState(const SPtr<GPUProgram>& program,
                                               GPU_DEVICE_FLAGS::E deviceMask)
      : TComputePipelineState(program),
        m_deviceMask(deviceMask)
    {}

    void
    ComputePipelineState::initialize() {
      GPU_PIPELINE_PARAMS_DESC paramsDesc;
      paramsDesc.computeParams = m_program->getParamDesc();

      m_paramInfo = GPUPipelineParamInfo::create(paramsDesc, m_deviceMask);

      CoreObject::initialize();
    }

    SPtr<ComputePipelineState>
    ComputePipelineState::create(const SPtr<GPUProgram>& program,
                                 GPU_DEVICE_FLAGS::E deviceMask) {
      return RenderStateManager::instance().createComputePipelineState(program, deviceMask);
    }
  }
}
