/*****************************************************************************/
/**
 * @file    geDepthStencilState.cpp
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

/*****************************************************************************/
/**
 * Includes
 */
/*****************************************************************************/
#include "geDepthStencilState.h"
#include "geDepthStencilStateRTTI.h"
#include "geRenderStateManager.h"
#include "geRenderAPI.h"
#include "geResources.h"

#include <geException.h>

namespace geEngineSDK {
  using std::static_pointer_cast;

  bool
  DEPTH_STENCIL_STATE_DESC::operator==(const DEPTH_STENCIL_STATE_DESC& rhs) const {
    return depthReadEnable == rhs.depthReadEnable &&
           depthWriteEnable == rhs.depthWriteEnable &&
           depthComparisonFunc == rhs.depthComparisonFunc &&
           stencilEnable == rhs.stencilEnable &&
           stencilReadMask == rhs.stencilReadMask &&
           stencilWriteMask == rhs.stencilWriteMask &&
           frontStencilFailOp == rhs.frontStencilFailOp &&
           frontStencilZFailOp == rhs.frontStencilZFailOp &&
           frontStencilPassOp == rhs.frontStencilPassOp &&
           frontStencilComparisonFunc == rhs.frontStencilComparisonFunc &&
           backStencilFailOp == rhs.backStencilFailOp &&
           backStencilZFailOp == rhs.backStencilZFailOp &&
           backStencilPassOp == rhs.backStencilPassOp &&
           backStencilComparisonFunc == rhs.backStencilComparisonFunc;
  }

  DepthStencilProperties::DepthStencilProperties(const DEPTH_STENCIL_STATE_DESC& desc)
    : m_data(desc),
      m_hash(DepthStencilState::generateHash(desc))
  {}

  DepthStencilState::DepthStencilState(const DEPTH_STENCIL_STATE_DESC& desc)
    : m_properties(desc)
  {}

  SPtr<geCoreThread::DepthStencilState>
  DepthStencilState::getCore() const {
    return static_pointer_cast<geCoreThread::DepthStencilState>(m_coreSpecific);
  }

  SPtr<geCoreThread::CoreObject>
  DepthStencilState::createCore() const {
    auto core = geCoreThread::RenderStateManager::instance().
                  _createDepthStencilState(m_properties.m_data);
    //Accessing core from sim thread is okay here since core ID is immutable
    m_id = core->getId();

    return core;
  }

  const SPtr<DepthStencilState>&
  DepthStencilState::getDefault() {
    return RenderStateManager::instance().getDefaultDepthStencilState();
  }

  const DepthStencilProperties&
  DepthStencilState::getProperties() const {
    return m_properties;
  }

  SPtr<DepthStencilState>
  DepthStencilState::create(const DEPTH_STENCIL_STATE_DESC& desc) {
    return RenderStateManager::instance().createDepthStencilState(desc);
  }

  uint64
  DepthStencilState::generateHash(const DEPTH_STENCIL_STATE_DESC& desc) {
    size_t hash = 0;
    hash_combine(hash, desc.depthReadEnable);
    hash_combine(hash, desc.depthWriteEnable);
    hash_combine(hash, static_cast<uint32>(desc.depthComparisonFunc));
    hash_combine(hash, desc.stencilEnable);
    hash_combine(hash, desc.stencilReadMask);
    hash_combine(hash, desc.stencilWriteMask);
    hash_combine(hash, static_cast<uint32>(desc.frontStencilFailOp));
    hash_combine(hash, static_cast<uint32>(desc.frontStencilZFailOp));
    hash_combine(hash, static_cast<uint32>(desc.frontStencilPassOp));
    hash_combine(hash, static_cast<uint32>(desc.frontStencilComparisonFunc));
    hash_combine(hash, static_cast<uint32>(desc.backStencilFailOp));
    hash_combine(hash, static_cast<uint32>(desc.backStencilZFailOp));
    hash_combine(hash, static_cast<uint32>(desc.backStencilPassOp));
    hash_combine(hash, static_cast<uint32>(desc.backStencilComparisonFunc));

    return static_cast<uint64>(hash);
  }

  /***************************************************************************/
  /**
   * RTTI
   */
  /***************************************************************************/

  RTTITypeBase*
  DepthStencilState::getRTTIStatic() {
    return DepthStencilStateRTTI::instance();
  }

  RTTITypeBase*
  DepthStencilState::getRTTI() const {
    return DepthStencilState::getRTTIStatic();
  }

  namespace geCoreThread {
    DepthStencilState::DepthStencilState(const DEPTH_STENCIL_STATE_DESC& desc,
                                         uint32 id)
      : m_properties(desc),
        m_id(id)
    {}

    void
    DepthStencilState::initialize() {
      //Since we cache states it's possible this object was already initialized
      //(i.e. multiple sim-states can share a single core-state)
      if (isInitialized()) {
        return;
      }

      createInternal();
      CoreObject::initialize();
    }

    const DepthStencilProperties&
    DepthStencilState::getProperties() const {
      return m_properties;
    }

    SPtr<DepthStencilState>
    DepthStencilState::create(const DEPTH_STENCIL_STATE_DESC& desc) {
      return RenderStateManager::instance().createDepthStencilState(desc);
    }

    const SPtr<DepthStencilState>&
    DepthStencilState::getDefault() {
      return RenderStateManager::instance().getDefaultDepthStencilState();
    }
  }
}
