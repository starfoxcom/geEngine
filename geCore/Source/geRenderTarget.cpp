/*****************************************************************************/
/**
 * @file    geRenderTarget.cpp
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2018/05/12
 * @brief   Buffer or texture that the render system renders the scene to.
 *
 * Render target is a frame buffer or a texture that the render system renders
 * the scene to.
 *
 * @bug     No known bugs.
 */
/*****************************************************************************/

/*****************************************************************************/
/**
 * Includes
 */
/*****************************************************************************/
#include "geRenderTarget.h"
#include "geViewport.h"
//#include "gesRenderAPI.h"
#include "geCoreThread.h"

#include <geException.h>

namespace geEngineSDK {
  using std::function;
  using std::bind;
  using std::static_pointer_cast;

  RenderTarget::RenderTarget() {
    //We never sync from sim to core, so mark it clean to avoid overwriting
    //core thread changes
    markCoreClean();
  }

  void
  RenderTarget::setPriority(int32 priority) {
    function<void(SPtr<geCoreThread::RenderTarget>, int32)> windowedFunc =
      [](SPtr<geCoreThread::RenderTarget> renderTarget, int32 priority) {
      renderTarget->setPriority(priority);
    };

    g_coreThread().queueCommand(bind(windowedFunc, getCore(), priority));
  }

  SPtr<geCoreThread::RenderTarget>
  RenderTarget::getCore() const {
    return static_pointer_cast<geCoreThread::RenderTarget>(m_coreSpecific);
  }

  const RenderTargetProperties&
  RenderTarget::getProperties() const {
    THROW_IF_CORE_THREAD;
    return getPropertiesInternal();
  }

  void
  RenderTarget::getCustomAttribute(const String& /*name*/,
                                   void* /*pData*/) const {
    GE_EXCEPT(InvalidParametersException, "Attribute not found.");
  }

  namespace geCoreThread {
    void
    RenderTarget::setPriority(int32 priority) {
      RenderTargetProperties& props = const_cast<RenderTargetProperties&>(getProperties());
      props.m_priority = priority;
    }

    const RenderTargetProperties&
    RenderTarget::getProperties() const {
      return getPropertiesInternal();
    }

    void
    RenderTarget::getCustomAttribute(const String& /*name*/,
                                     void* /*pData*/) const {
      GE_EXCEPT(InvalidParametersException, "Attribute not found.");
    }
  }
}
