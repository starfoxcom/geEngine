/*****************************************************************************/
/**
 * @file    geSamplerState.cpp
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2018/06/09
 * @brief   Class representing the state of a texture sampler.
 *
 * Class representing the state of a texture sampler.
 *
 * @bug     No known bugs.
 */
/*****************************************************************************/

/*****************************************************************************/
/**
 * Includes
 */
/*****************************************************************************/
#include "geSamplerState.h"
#include "geSamplerStateRTTI.h"
#include "geRenderStateManager.h"

namespace geEngineSDK {
  using std::static_pointer_cast;

  bool
  SAMPLER_STATE_DESC::operator == (const SAMPLER_STATE_DESC& rhs) const {
    return addressMode == rhs.addressMode &&
           minFilter == rhs.minFilter &&
           magFilter == rhs.magFilter &&
           mipFilter == rhs.mipFilter &&
           maxAniso == rhs.maxAniso &&
           mipmapBias == rhs.mipmapBias &&
           mipMin == rhs.mipMin &&
           mipMax == rhs.mipMax &&
           borderColor == rhs.borderColor &&
           comparisonFunc == rhs.comparisonFunc;
  }

  SamplerProperties::SamplerProperties(const SAMPLER_STATE_DESC& desc)
    : m_data(desc),
      m_hash(SamplerState::generateHash(desc))
  {}

  FILTER_OPTIONS::E
  SamplerProperties::getTextureFiltering(FILTER_TYPE::E ft) const {
    switch (ft)
    {
      case FILTER_TYPE::kMIN:
        return m_data.minFilter;
      case FILTER_TYPE::kMAG:
        return m_data.magFilter;
      case FILTER_TYPE::kMIP:
        return m_data.mipFilter;
    }

    return m_data.minFilter;
  }

  const LinearColor&
  SamplerProperties::getBorderColor() const {
    return m_data.borderColor;
  }

  SamplerState::SamplerState(const SAMPLER_STATE_DESC& desc)
    : m_properties(desc)
  {}

  SPtr<geCoreThread::SamplerState>
  SamplerState::getCore() const {
    return static_pointer_cast<geCoreThread::SamplerState>(m_coreSpecific);
  }

  SPtr<geCoreThread::CoreObject>
  SamplerState::createCore() const {
    return geCoreThread::RenderStateManager::instance().
             _createSamplerState(m_properties.m_data);
  }

  SPtr<SamplerState>
  SamplerState::create(const SAMPLER_STATE_DESC& desc) {
    return RenderStateManager::instance().createSamplerState(desc);
  }

  const SPtr<SamplerState>&
  SamplerState::getDefault() {
    return RenderStateManager::instance().getDefaultSamplerState();
  }

  uint64
  SamplerState::generateHash(const SAMPLER_STATE_DESC& desc) {
    size_t hash = 0;
    hash_combine(hash, static_cast<uint32>(desc.addressMode.u));
    hash_combine(hash, static_cast<uint32>(desc.addressMode.v));
    hash_combine(hash, static_cast<uint32>(desc.addressMode.w));
    hash_combine(hash, static_cast<uint32>(desc.minFilter));
    hash_combine(hash, static_cast<uint32>(desc.magFilter));
    hash_combine(hash, static_cast<uint32>(desc.mipFilter));
    hash_combine(hash, desc.maxAniso);
    hash_combine(hash, desc.mipmapBias);
    hash_combine(hash, desc.mipMin);
    hash_combine(hash, desc.mipMax);
    hash_combine(hash, desc.borderColor);
    hash_combine(hash, static_cast<uint32>(desc.comparisonFunc));

    return static_cast<uint64>(hash);
  }

  const SamplerProperties&
  SamplerState::getProperties() const {
    return m_properties;
  }

  /***************************************************************************/
  /**
   * RTTI
   */
  /***************************************************************************/

  RTTITypeBase*
  SamplerState::getRTTIStatic() {
    return SamplerStateRTTI::instance();
  }

  RTTITypeBase*
  SamplerState::getRTTI() const {
    return SamplerState::getRTTIStatic();
  }

  namespace geCoreThread {
    SamplerState::SamplerState(const SAMPLER_STATE_DESC& desc,
                               GPU_DEVICE_FLAGS::E /*deviceMask*/)
      : m_properties(desc)
    {}

    SamplerState::~SamplerState() {
      RenderStateManager::instance().notifySamplerStateDestroyed(m_properties.m_data);
    }

    void
    SamplerState::initialize() {
      //Since we cache states it's possible this object was already initialized
      //(i.e. multiple sim-states can share a single core-state)
      if (isInitialized()) {
        return;
      }

      createInternal();
      CoreObject::initialize();
    }

    const SamplerProperties&
    SamplerState::getProperties() const {
      return m_properties;
    }

    SPtr<SamplerState>
    SamplerState::create(const SAMPLER_STATE_DESC& desc,
                         GPU_DEVICE_FLAGS::E deviceMask) {
      return RenderStateManager::instance().createSamplerState(desc, deviceMask);
    }

    const SPtr<SamplerState>&
    SamplerState::getDefault() {
      return RenderStateManager::instance().getDefaultSamplerState();
    }
  }
}
