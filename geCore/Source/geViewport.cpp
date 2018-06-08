/*****************************************************************************/
/**
 * @file    geViewport.cpp
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2018/05/12
 * @brief   Determines to which RenderTarget should rendering be performed.
 *
 * Viewport determines to which RenderTarget should rendering be performed.
 * It allows you to render to a sub-region of the target by specifying the area
 * rectangle, and allows you to set up color / depth / stencil clear values for
 * that specific region.
 *
 * @bug     No known bugs.
 */
/*****************************************************************************/

/*****************************************************************************/
/**
 * Includes
 */
/*****************************************************************************/
#include "geViewport.h"
#include "geViewportRTTI.h"
#include "geRenderTarget.h"
#include "geRenderAPI.h"

#include <geException.h>
#include <geMath.h>

namespace geEngineSDK {
  using std::static_pointer_cast;

  const LinearColor ViewportBase::DEFAULT_CLEAR_COLOR = LinearColor(0.0f, 0.3685f, 0.7969f);

  ViewportBase::ViewportBase(float x, float y, float width, float height)
    : m_normArea(Vector2(x, y), Vector2(x + width, y + height)),
      m_clearFlags(ClearFlagBits::kColor | ClearFlagBits::kDepth),
      m_clearColorValue(DEFAULT_CLEAR_COLOR),
      m_clearDepthValue(1.0f),
    m_clearStencilValue(0)
  {}

  void
  ViewportBase::setArea(const Box2D& area) {
    m_normArea = area;
    _markCoreDirty();
  }

  Box2DI
  ViewportBase::getPixelArea() const {
    float width = static_cast<float>(getTargetWidth());
    float height = static_cast<float>(getTargetHeight());

    Box2DI area;
    area.m_min.x = static_cast<int>(m_normArea.m_min.x * width);
    area.m_min.y = static_cast<int>(m_normArea.m_min.y * height);
    area.m_max.x = static_cast<int>(m_normArea.m_max.x * width);
    area.m_max.y = static_cast<int>(m_normArea.m_max.y * height);

    return area;
  }

  void
  ViewportBase::setClearFlags(ClearFlags flags) {
    m_clearFlags = flags;
    _markCoreDirty();
  }

  void
  ViewportBase::setClearValues(const LinearColor& clearColor,
                               float clearDepth,
                               uint16 clearStencil) {
    m_clearColorValue = clearColor;
    m_clearDepthValue = clearDepth;
    m_clearStencilValue = clearStencil;
    _markCoreDirty();
  }

  void
  ViewportBase::setClearColorValue(const LinearColor& color) {
    m_clearColorValue = color;
    _markCoreDirty();
  }

  void
  ViewportBase::setClearDepthValue(float depth) {
    m_clearDepthValue = depth;
    _markCoreDirty();
  }

  void
  ViewportBase::setClearStencilValue(uint16 value) {
    m_clearStencilValue = value;
    _markCoreDirty();
  }

  Viewport::Viewport()
    : ViewportBase()
  {}

  Viewport::Viewport(const SPtr<RenderTarget>& target,
                     float x,
                     float y,
                     float width,
                     float height)
    : ViewportBase(x, y, width, height),
      m_target(target)
  {}

  void
  Viewport::setTarget(const SPtr<RenderTarget>& target) {
    m_target = target;
    markDependenciesDirty();
    _markCoreDirty();
  }

  SPtr<geCoreThread::Viewport>
  Viewport::getCore() const {
    return static_pointer_cast<geCoreThread::Viewport>(m_coreSpecific);
  }

  void
  Viewport::_markCoreDirty() {
    markCoreDirty();
  }

  uint32
  Viewport::getTargetWidth() const {
    if (nullptr != m_target) {
      return m_target->getProperties().m_width;
    }
    return 0;
  }

  uint32
  Viewport::getTargetHeight() const {
    if (nullptr != m_target) {
      return m_target->getProperties().m_height;
    }
    return 0;
  }

  SPtr<geCoreThread::CoreObject>
  Viewport::createCore() const {
    SPtr<geCoreThread::RenderTarget> targetCore;
    if (nullptr != m_target) {
      targetCore = m_target->getCore();
    }

    auto viewport = ge_new<geCoreThread::Viewport>(targetCore,
                                                   m_normArea.m_min.x,
                                                   m_normArea.m_min.y,
                                                   m_normArea.m_max.x,
                                                   m_normArea.m_max.y);

    SPtr<geCoreThread::Viewport>
      viewportPtr = ge_shared_ptr<geCoreThread::Viewport>(viewport);
    viewportPtr->_setThisPtr(viewportPtr);

    return viewportPtr;
  }

  CoreSyncData
  Viewport::syncToCore(FrameAlloc* allocator) {
    uint32 size = 0;
    size += rttiGetElementSize(m_normArea);
    size += rttiGetElementSize(m_clearFlags);
    size += rttiGetElementSize(m_clearColorValue);
    size += rttiGetElementSize(m_clearDepthValue);
    size += rttiGetElementSize(m_clearStencilValue);
    size += sizeof(SPtr<geCoreThread::RenderTarget>);

    uint8* buffer = allocator->alloc(size);

    char* dataPtr = (char*)buffer;
    dataPtr = rttiWriteElement(m_normArea, dataPtr);
    dataPtr = rttiWriteElement(m_clearFlags, dataPtr);
    dataPtr = rttiWriteElement(m_clearColorValue, dataPtr);
    dataPtr = rttiWriteElement(m_clearDepthValue, dataPtr);
    dataPtr = rttiWriteElement(m_clearStencilValue, dataPtr);

    SPtr<geCoreThread::RenderTarget>*
      rtPtr = new (dataPtr) SPtr<geCoreThread::RenderTarget>();

    if (nullptr != m_target) {
      *rtPtr = m_target->getCore();
    }
    else {
      *rtPtr = nullptr;
    }

    return CoreSyncData(buffer, size);
  }

  void
  Viewport::getCoreDependencies(Vector<CoreObject*>& dependencies) {
    if (nullptr != m_target) {
      dependencies.push_back(m_target.get());
    }
  }

  SPtr<Viewport>
  Viewport::create(const SPtr<RenderTarget>& target,
                   float x,
                   float y,
                   float width,
                   float height) {
    auto viewport = ge_new<Viewport>(target, x, y, width, height);
    SPtr<Viewport> viewportPtr = ge_core_ptr<Viewport>(viewport);
    viewportPtr->_setThisPtr(viewportPtr);
    viewportPtr->initialize();

    return viewportPtr;
  }

  SPtr<Viewport>
  Viewport::createEmpty() {
    auto viewport = ge_new<Viewport>();
    SPtr<Viewport> viewportPtr = ge_core_ptr<Viewport>(viewport);
    viewportPtr->_setThisPtr(viewportPtr);
    return viewportPtr;
  }

  RTTITypeBase*
  Viewport::getRTTIStatic() {
    return ViewportRTTI::instance();
  }

  RTTITypeBase*
  Viewport::getRTTI() const {
    return Viewport::getRTTIStatic();
  }

  namespace geCoreThread {
    Viewport::Viewport(const SPtr<RenderTarget>& target,
                       float x,
                       float y,
                       float width,
                       float height)
      : ViewportBase(x, y, width, height),
        m_target(target)
    {}

    SPtr<Viewport>
    Viewport::create(const SPtr<RenderTarget>& target,
                     float x,
                     float y,
                     float width,
                     float height) {
      auto viewport = ge_new<Viewport>(target, x, y, width, height);
      SPtr<Viewport> viewportPtr = ge_shared_ptr<Viewport>(viewport);
      viewportPtr->_setThisPtr(viewportPtr);
      viewportPtr->initialize();
      return viewportPtr;
    }

    uint32
    Viewport::getTargetWidth() const {
      if (nullptr != m_target) {
        return m_target->getProperties().m_width;
      }
      return 0;
    }

    uint32
    Viewport::getTargetHeight() const {
      if (nullptr != m_target) {
        return m_target->getProperties().m_height;
      }
      return 0;
    }

    void
    Viewport::syncToCore(const CoreSyncData& data) {
      char* dataPtr = (char*)data.getBuffer();
      dataPtr = rttiReadElement(m_normArea, dataPtr);
      dataPtr = rttiReadElement(m_clearFlags, dataPtr);
      dataPtr = rttiReadElement(m_clearColorValue, dataPtr);
      dataPtr = rttiReadElement(m_clearDepthValue, dataPtr);
      dataPtr = rttiReadElement(m_clearStencilValue, dataPtr);

      SPtr<RenderTarget>* rtPtr = (SPtr<RenderTarget>*)dataPtr;
      m_target = *rtPtr;

      rtPtr->~SPtr<RenderTarget>();
    }
  }
}
