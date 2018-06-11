/*****************************************************************************/
/**
 * @file    geRasterizerState.cpp
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

/*****************************************************************************/
/**
 * Includes
 */
/*****************************************************************************/
#include "geRasterizerState.h"
#include "geRasterizerStateRTTI.h"
#include "geRenderStateManager.h"
#include "geRenderAPI.h"
#include "geResources.h"

namespace geEngineSDK {
  using std::static_pointer_cast;

  bool
  RASTERIZER_STATE_DESC::operator == (const RASTERIZER_STATE_DESC& rhs) const {
    return polygonMode == rhs.polygonMode &&
           cullMode == rhs.cullMode &&
           depthBias == rhs.depthBias &&
           depthBiasClamp == rhs.depthBiasClamp &&
           slopeScaledDepthBias == rhs.slopeScaledDepthBias &&
           depthClipEnable == rhs.depthClipEnable &&
           scissorEnable == rhs.scissorEnable &&
           multisampleEnable == rhs.multisampleEnable &&
           antialiasedLineEnable == rhs.antialiasedLineEnable;
  }

  RasterizerProperties::RasterizerProperties(const RASTERIZER_STATE_DESC& desc)
    : m_data(desc),
      m_hash(RasterizerState::generateHash(desc))
  {}

  RasterizerState::RasterizerState(const RASTERIZER_STATE_DESC& desc)
    : m_properties(desc)
  {}

  SPtr<geCoreThread::RasterizerState>
  RasterizerState::getCore() const {
    return static_pointer_cast<geCoreThread::RasterizerState>(m_coreSpecific);
  }

  SPtr<geCoreThread::CoreObject>
  RasterizerState::createCore() const {
    auto core = geCoreThread::RenderStateManager::instance().
                  _createRasterizerState(m_properties.m_data);
    
    //Accessing core from sim thread is okay here since core ID is immutable
    m_id = core->getId();

    return core;
  }

  const RasterizerProperties&
  RasterizerState::getProperties() const {
    return m_properties;
  }

  const SPtr<RasterizerState>&
  RasterizerState::getDefault() {
    return RenderStateManager::instance().getDefaultRasterizerState();
  }

  SPtr<RasterizerState>
  RasterizerState::create(const RASTERIZER_STATE_DESC& desc) {
    return RenderStateManager::instance().createRasterizerState(desc);
  }

  uint64
  RasterizerState::generateHash(const RASTERIZER_STATE_DESC& desc) {
    size_t hash = 0;
    hash_combine(hash, static_cast<uint32>(desc.polygonMode));
    hash_combine(hash, static_cast<uint32>(desc.cullMode));
    hash_combine(hash, desc.depthBias);
    hash_combine(hash, desc.depthBiasClamp);
    hash_combine(hash, desc.slopeScaledDepthBias);
    hash_combine(hash, desc.depthClipEnable);
    hash_combine(hash, desc.scissorEnable);
    hash_combine(hash, desc.multisampleEnable);
    hash_combine(hash, desc.antialiasedLineEnable);

    return static_cast<uint64>(hash);
  }

  /***************************************************************************/
  /**
   * RTTI
   */
  /***************************************************************************/

  RTTITypeBase*
  RasterizerState::getRTTIStatic() {
    return RasterizerStateRTTI::instance();
  }

  RTTITypeBase*
  RasterizerState::getRTTI() const {
    return RasterizerState::getRTTIStatic();
  }

  namespace geCoreThread {
    RasterizerState::RasterizerState(const RASTERIZER_STATE_DESC& desc,
                                     uint32 id)
      : m_properties(desc),
        m_id(id)
    {}

    void
    RasterizerState::initialize() {
      //Since we cache states it's possible this object was already initialized
      //(i.e. multiple sim-states can share a single core-state)
      if (isInitialized()) {
        return;
      }

      createInternal();
      CoreObject::initialize();
    }

    const RasterizerProperties&
    RasterizerState::getProperties() const {
      return m_properties;
    }

    SPtr<RasterizerState>
    RasterizerState::create(const RASTERIZER_STATE_DESC& desc) {
      return RenderStateManager::instance().createRasterizerState(desc);
    }

    const SPtr<RasterizerState>&
    RasterizerState::getDefault() {
      return RenderStateManager::instance().getDefaultRasterizerState();
    }
  }
}
