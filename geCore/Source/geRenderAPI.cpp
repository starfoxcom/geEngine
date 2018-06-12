/*****************************************************************************/
/**
 * @file    geRenderAPI.cpp
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2018/05/22
 * @brief   Provides access to geCoreThread::RenderAPI from the sim thread.
 *
 * Provides access to geCoreThread::RenderAPI from the simulation thread.
 * All the commands get queued on the core thread queue for the calling thread.
 *
 * @bug     No known bugs.
 */
/*****************************************************************************/

/*****************************************************************************/
/**
 * Includes
 */
/*****************************************************************************/
#include "geRenderAPI.h"
#include "geCoreThread.h"
#include "geViewport.h"
#include "geRenderTarget.h"
#include "geRenderWindow.h"
#include "geVertexBuffer.h"
#include "geIndexBuffer.h"

/*
#include "geMesh.h"
*/
#include "geGPUParams.h"
#include "geDepthStencilState.h"
#include "geRasterizerState.h"
#include "geGPUBuffer.h"
#include "geGPUPipelineState.h"

namespace geEngineSDK {
  using namespace std::placeholders;
  using std::function;
  using std::bind;

  void
  RenderAPI::setGPUParams(const SPtr<GPUParams>& gpuParams) {
    g_coreThread().queueCommand(bind(&geCoreThread::RenderAPI::setGPUParams,
                                     geCoreThread::RenderAPI::instancePtr(),
                                     gpuParams->getCore(),
                                     nullptr));
  }

  void
  RenderAPI::setGraphicsPipeline(const SPtr<GraphicsPipelineState>& pipelineState) {
    g_coreThread().queueCommand(bind(&geCoreThread::RenderAPI::setGraphicsPipeline,
                                     geCoreThread::RenderAPI::instancePtr(),
                                     pipelineState->getCore(),
                                     nullptr));
  }

  void
  RenderAPI::setComputePipeline(const SPtr<ComputePipelineState>& pipelineState) {
    g_coreThread().queueCommand(bind(&geCoreThread::RenderAPI::setComputePipeline,
                                     geCoreThread::RenderAPI::instancePtr(),
                                     pipelineState->getCore(),
                                     nullptr));
  }

  void
  RenderAPI::setVertexBuffers(uint32 index,
                              const Vector<SPtr<VertexBuffer>>& buffers) {
    Vector<SPtr<geCoreThread::VertexBuffer>> coreBuffers(buffers.size());
    for (SIZE_T i = 0; i < buffers.size(); ++i) {
      coreBuffers[i] = buffers[i] != nullptr ? buffers[i]->getCore() : nullptr;
    }

    function<void(geCoreThread::RenderAPI*,
                  uint32,
                  const Vector<SPtr<geCoreThread::VertexBuffer>>&)>
      resizeFunc =
    [](geCoreThread::RenderAPI* rs,
       uint32 idx,
       const Vector<SPtr<geCoreThread::VertexBuffer>>& _buffers) {
      rs->setVertexBuffers(idx,
                           (SPtr<geCoreThread::VertexBuffer>*)_buffers.data(),
                           static_cast<uint32>(_buffers.size()));
    };

    g_coreThread().queueCommand(bind(resizeFunc,
                                     geCoreThread::RenderAPI::instancePtr(),
                                     index,
                                     coreBuffers));
  }

  void
  RenderAPI::setIndexBuffer(const SPtr<IndexBuffer>& buffer) {
    g_coreThread().queueCommand(bind(&geCoreThread::RenderAPI::setIndexBuffer,
                                     geCoreThread::RenderAPI::instancePtr(),
                                     buffer->getCore(),
                                     nullptr));
  }

  void
  RenderAPI::setVertexDeclaration(const SPtr<VertexDeclaration>& vertexDeclaration) {
    g_coreThread().queueCommand(bind(&geCoreThread::RenderAPI::setVertexDeclaration,
                                     geCoreThread::RenderAPI::instancePtr(),
                                     vertexDeclaration->getCore(),
                                     nullptr));
  }

  void
  RenderAPI::setViewport(const Box2D& vp) {
    g_coreThread().queueCommand(bind(&geCoreThread::RenderAPI::setViewport,
                                     geCoreThread::RenderAPI::instancePtr(),
                                     vp,
                                     nullptr));
  }

  void
  RenderAPI::setStencilRef(uint32 value) {
    g_coreThread().queueCommand(bind(&geCoreThread::RenderAPI::setStencilRef,
                                     geCoreThread::RenderAPI::instancePtr(),
                                     value,
                                     nullptr));
  }

  void
  RenderAPI::setDrawOperation(DrawOperationType op) {
    g_coreThread().queueCommand(bind(&geCoreThread::RenderAPI::setDrawOperation,
                                     geCoreThread::RenderAPI::instancePtr(),
                                     op,
                                     nullptr));
  }

  void
  RenderAPI::setScissorRect(uint32 left,
                            uint32 top,
                            uint32 right,
                            uint32 bottom) {
    g_coreThread().queueCommand(bind(&geCoreThread::RenderAPI::setScissorRect,
                                     geCoreThread::RenderAPI::instancePtr(),
                                     left,
                                     top,
                                     right,
                                     bottom,
                                     nullptr));
  }

  void
  RenderAPI::setRenderTarget(const SPtr<RenderTarget>& target,
                             uint32 readOnlyFlags,
                             RenderSurfaceMask loadMask) {
    g_coreThread().queueCommand(bind(&geCoreThread::RenderAPI::setRenderTarget,
                                     geCoreThread::RenderAPI::instancePtr(),
                                     target->getCore(),
                                     readOnlyFlags,
                                     loadMask,
                                     nullptr));
  }

  void
  RenderAPI::clearRenderTarget(uint32 buffers,
                               const LinearColor& color,
                               float depth,
                               uint16 stencil,
                               uint8 targetMask) {
    g_coreThread().queueCommand(bind(&geCoreThread::RenderAPI::clearRenderTarget,
                                     geCoreThread::RenderAPI::instancePtr(),
                                     buffers,
                                     color,
                                     depth,
                                     stencil,
                                     targetMask,
                                     nullptr));
  }

  void
  RenderAPI::clearViewport(uint32 buffers,
                           const LinearColor& color,
                           float depth,
                           uint16 stencil,
                           uint8 targetMask) {
    g_coreThread().queueCommand(bind(&geCoreThread::RenderAPI::clearViewport,
                                     geCoreThread::RenderAPI::instancePtr(),
                                     buffers,
                                     color,
                                     depth,
                                     stencil,
                                     targetMask,
                                     nullptr));
  }

  void
  RenderAPI::swapBuffers(const SPtr<RenderTarget>& target) {
    g_coreThread().queueCommand(bind(&geCoreThread::RenderAPI::swapBuffers,
                                     geCoreThread::RenderAPI::instancePtr(),
                                     target->getCore(),
                                     1));
  }

  void
  RenderAPI::draw(uint32 vertexOffset,
                  uint32 vertexCount,
                  uint32 instanceCount) {
    g_coreThread().queueCommand(bind(&geCoreThread::RenderAPI::draw,
                                     geCoreThread::RenderAPI::instancePtr(),
                                     vertexOffset,
                                     vertexCount,
                                     instanceCount,
                                     nullptr));
  }

  void
  RenderAPI::drawIndexed(uint32 startIndex,
                         uint32 indexCount,
                         uint32 vertexOffset,
                         uint32 vertexCount,
                         uint32 instanceCount) {
    g_coreThread().queueCommand(bind(&geCoreThread::RenderAPI::drawIndexed,
                                     geCoreThread::RenderAPI::instancePtr(),
                                     startIndex,
                                     indexCount,
                                     vertexOffset,
                                     vertexCount,
                                     instanceCount,
                                     nullptr));
  }

  void
  RenderAPI::dispatchCompute(uint32 numGroupsX,
                             uint32 numGroupsY,
                             uint32 numGroupsZ) {
    g_coreThread().queueCommand(bind(&geCoreThread::RenderAPI::dispatchCompute,
                                     geCoreThread::RenderAPI::instancePtr(),
                                     numGroupsX,
                                     numGroupsY,
                                     numGroupsZ,
                                     nullptr));
  }

  const VideoModeInfo&
  RenderAPI::getVideoModeInfo() {
    return geCoreThread::RenderAPI::instance().getVideoModeInfo();
  }

  void
  RenderAPI::convertProjectionMatrix(const Matrix4& matrix, Matrix4& dest) {
    geCoreThread::RenderAPI::instance().convertProjectionMatrix(matrix, dest);
  }

  const RenderAPIInfo&
  RenderAPI::getAPIInfo() {
    return geCoreThread::RenderAPI::instance().getAPIInfo();
  }

  namespace geCoreThread {
    RenderAPI::RenderAPI()
      : m_currentCapabilities(nullptr),
        m_numDevices(0)
    {}

    RenderAPI::~RenderAPI() {
      //Base classes need to call virtual destroy_internal method instead of a
      //destructor
      //ge_deleteN(m_currentCapabilities, m_numDevices);
      m_currentCapabilities = nullptr;
    }

    SPtr<geEngineSDK::RenderWindow>
    RenderAPI::initialize(const RENDER_WINDOW_DESC& primaryWindowDesc) {
      g_coreThread().queueCommand(bind(
                          (void(RenderAPI::*)())&RenderAPI::initialize, this),
                          CTQF::kInternalQueue | CTQF::kBlockUntilComplete);

      RENDER_WINDOW_DESC windowDesc = primaryWindowDesc;
      SPtr<geEngineSDK::RenderWindow>
        renderWindow = geEngineSDK::RenderWindow::create(windowDesc, nullptr);

      g_coreThread().queueCommand(bind(&RenderAPI::initializeWithWindow,
                                       this,
                                       renderWindow->getCore()),
                                       CTQF::kInternalQueue | CTQF::kBlockUntilComplete);

      return renderWindow;
    }

    void
    RenderAPI::initialize() {}

    void
    RenderAPI::initializeWithWindow(const SPtr<RenderWindow>& /*primaryWindow*/) {
      THROW_IF_NOT_CORE_THREAD;
    }

    void
    RenderAPI::destroy() {
      g_coreThread().queueCommand(bind(&RenderAPI::destroyCore, this));
      g_coreThread().submitAll(true);
    }

    void
    RenderAPI::destroyCore() {
      m_activeRenderTarget = nullptr;
    }

    /*
    const RenderAPICapabilities&
    RenderAPI::getCapabilities(uint32 deviceIdx) const {
      if (deviceIdx >= m_numDevices) {
        LOGWRN("Invalid device index provided: " + toString(deviceIdx) +
               ". Valid range is: [0, " + toString(m_numDevices) + ").");
        return m_currentCapabilities[0];
      }

      return m_currentCapabilities[deviceIdx];
    }
    */

    uint32
    RenderAPI::vertexCountToPrimCount(DrawOperationType type,
                                      uint32 elementCount) {
      uint32 primCount = 0;
      switch (type)
      {
        case DOT_POINT_LIST:
          primCount = elementCount;
          break;
        case DOT_LINE_LIST:
          primCount = elementCount >> 1;
          break;
        case DOT_LINE_STRIP:
          primCount = elementCount - 1;
          break;
        case DOT_TRIANGLE_LIST:
          primCount = elementCount / 3;
          break;
        case DOT_TRIANGLE_STRIP:
          primCount = elementCount - 2;
          break;
        case DOT_TRIANGLE_FAN:
          primCount = elementCount - 2;
          break;
      }

      return primCount;
    }
  }
}
