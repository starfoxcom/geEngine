/*****************************************************************************/
/**
 * @file    geBlendState.cpp
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

/*****************************************************************************/
/**
 * Includes
 */
/*****************************************************************************/
#include "geBlendState.h"

#include "geRenderStateManager.h"
#include "geRenderAPI.h"
#include "geBlendStateRTTI.h"
#include "geResources.h"

namespace geEngineSDK {
  using std::static_pointer_cast;

  bool
  RENDER_TARGET_BLEND_STATE_DESC::
  operator==(const RENDER_TARGET_BLEND_STATE_DESC& rhs) const {
    return blendEnable == rhs.blendEnable &&
           srcBlend == rhs.srcBlend &&
           dstBlend == rhs.dstBlend &&
           blendOp == rhs.blendOp &&
           srcBlendAlpha == rhs.srcBlendAlpha &&
           dstBlendAlpha == rhs.dstBlendAlpha &&
           blendOpAlpha == rhs.blendOpAlpha &&
           renderTargetWriteMask == rhs.renderTargetWriteMask;
  }

  bool
  BLEND_STATE_DESC::operator==(const BLEND_STATE_DESC& rhs) const {
    bool equals = alphaToCoverageEnable == rhs.alphaToCoverageEnable &&
                  independantBlendEnable == rhs.independantBlendEnable;

    if (equals) {
      for (uint32 i = 0; i < GE_MAX_MULTIPLE_RENDER_TARGETS; ++i) {
        equals &= renderTargetDesc[i] == rhs.renderTargetDesc[i];
      }
    }

    return equals;
  }

  BlendProperties::BlendProperties(const BLEND_STATE_DESC& desc)
    : m_data(desc),
      m_hash(BlendState::generateHash(desc))
  {}

  bool
  BlendProperties::getBlendEnabled(uint32 renderTargetIdx) const {
    GE_ASSERT(renderTargetIdx < GE_MAX_MULTIPLE_RENDER_TARGETS);
    return m_data.renderTargetDesc[renderTargetIdx].blendEnable;
  }

  BLEND_FACTOR::E
  BlendProperties::getSrcBlend(uint32 renderTargetIdx) const {
    GE_ASSERT(renderTargetIdx < GE_MAX_MULTIPLE_RENDER_TARGETS);
    return m_data.renderTargetDesc[renderTargetIdx].srcBlend;
  }

  BLEND_FACTOR::E
  BlendProperties::getDstBlend(uint32 renderTargetIdx) const {
    GE_ASSERT(renderTargetIdx < GE_MAX_MULTIPLE_RENDER_TARGETS);
    return m_data.renderTargetDesc[renderTargetIdx].dstBlend;
  }

  BLEND_OPERATION::E
  BlendProperties::getBlendOperation(uint32 renderTargetIdx) const {
    GE_ASSERT(renderTargetIdx < GE_MAX_MULTIPLE_RENDER_TARGETS);
    return m_data.renderTargetDesc[renderTargetIdx].blendOp;
  }

  BLEND_FACTOR::E
  BlendProperties::getAlphaSrcBlend(uint32 renderTargetIdx) const {
    GE_ASSERT(renderTargetIdx < GE_MAX_MULTIPLE_RENDER_TARGETS);
    return m_data.renderTargetDesc[renderTargetIdx].srcBlendAlpha;
  }

  BLEND_FACTOR::E
  BlendProperties::getAlphaDstBlend(uint32 renderTargetIdx) const {
    GE_ASSERT(renderTargetIdx < GE_MAX_MULTIPLE_RENDER_TARGETS);
    return m_data.renderTargetDesc[renderTargetIdx].dstBlendAlpha;
  }

  BLEND_OPERATION::E
  BlendProperties::getAlphaBlendOperation(uint32 renderTargetIdx) const {
    GE_ASSERT(renderTargetIdx < GE_MAX_MULTIPLE_RENDER_TARGETS);
    return m_data.renderTargetDesc[renderTargetIdx].blendOpAlpha;
  }

  uint8
  BlendProperties::getRenderTargetWriteMask(uint32 renderTargetIdx) const {
    GE_ASSERT(renderTargetIdx < GE_MAX_MULTIPLE_RENDER_TARGETS);
    return m_data.renderTargetDesc[renderTargetIdx].renderTargetWriteMask;
  }

  BlendState::BlendState(const BLEND_STATE_DESC& desc)
    : m_properties(desc),
      m_id(0)
  {}

  SPtr<geCoreThread::BlendState>
  BlendState::getCore() const {
    return static_pointer_cast<geCoreThread::BlendState>(m_coreSpecific);
  }

  SPtr<geCoreThread::CoreObject>
  BlendState::createCore() const {
    SPtr<geCoreThread::BlendState> core =
      geCoreThread::RenderStateManager::instance()._createBlendState(m_properties.m_data);
    
    //Accessing core from sim thread is okay here since core ID is immutable
    m_id = core->getId();

    return core;
  }

  const BlendProperties&
  BlendState::getProperties() const {
    return m_properties;
  }

  const SPtr<BlendState>&
  BlendState::getDefault() {
    return RenderStateManager::instance().getDefaultBlendState();
  }

  SPtr<BlendState>
  BlendState::create(const BLEND_STATE_DESC& desc) {
    return RenderStateManager::instance().createBlendState(desc);
  }

  uint64
  BlendState::generateHash(const BLEND_STATE_DESC& desc) {
    size_t hash = 0;
    hash_combine(hash, desc.alphaToCoverageEnable);
    hash_combine(hash, desc.independantBlendEnable);

    for (const auto& i : desc.renderTargetDesc) {
      hash_combine(hash, i.blendEnable);
      hash_combine(hash, static_cast<uint32>(i.srcBlend));
      hash_combine(hash, static_cast<uint32>(i.dstBlend));
      hash_combine(hash, static_cast<uint32>(i.blendOp));
      hash_combine(hash, static_cast<uint32>(i.srcBlendAlpha));
      hash_combine(hash, static_cast<uint32>(i.dstBlendAlpha));
      hash_combine(hash, static_cast<uint32>(i.blendOpAlpha));
      hash_combine(hash, i.renderTargetWriteMask);
    }

    return static_cast<uint64>(hash);
  }

  /***************************************************************************/
  /**
   * RTTI
   */
  /***************************************************************************/

  RTTITypeBase*
  BlendState::getRTTIStatic() {
    return BlendStateRTTI::instance();
  }

  RTTITypeBase*
  BlendState::getRTTI() const {
    return BlendState::getRTTIStatic();
  }

  namespace geCoreThread {
    BlendState::BlendState(const BLEND_STATE_DESC& desc, uint32 id)
      : m_properties(desc),
        m_id(id)
    {}

    void
    BlendState::initialize() {
      //Since we cache states it's possible this object was already initialized
      //(i.e. multiple sim-states can share a single core-state)
      if (isInitialized()) {
        return;
      }

      createInternal();
      CoreObject::initialize();
    }

    const BlendProperties&
    BlendState::getProperties() const {
      return m_properties;
    }

    SPtr<BlendState>
    BlendState::create(const BLEND_STATE_DESC& desc) {
      return RenderStateManager::instance().createBlendState(desc);
    }

    const SPtr<BlendState>&
    BlendState::getDefault() {
      return RenderStateManager::instance().getDefaultBlendState();
    }
  }
}
