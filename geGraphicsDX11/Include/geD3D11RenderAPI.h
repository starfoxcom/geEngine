/*****************************************************************************/
/**
 * @file    geD3D11RenderAPI.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2018/05/11
 * @brief   Implementation of a render system using DirectX 11.
 *
 * Implementation of a render system using DirectX 11. Provides abstracted
 * access to various low level DX11 methods.
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
#include "gePrerequisitesD3D11.h"

#include <geRenderAPI.h>

namespace geEngineSDK {
  namespace geCoreThread {
    class D3D11RenderAPI final : public RenderAPI
    {
     public:
      D3D11RenderAPI();
      ~D3D11RenderAPI() = default;

      /**
       * @copydoc RenderAPI::getName
       */
      const StringID&
      getName() const override;

      /**
       * @copydoc RenderAPI::setGraphicsPipeline
       */
      /*
      void
      setGraphicsPipeline(const SPtr<GraphicsPipelineState>& pipelineState,
                          const SPtr<CommandBuffer>& commandBuffer = nullptr) override;
      */

      /**
       * @copydoc RenderAPI::setComputePipeline
       */
      /*
      void
      setComputePipeline(const SPtr<ComputePipelineState>& pipelineState,
                         const SPtr<CommandBuffer>& commandBuffer = nullptr) override;
      */

      /**
       * @copydoc RenderAPI::setGpuParams
       */
      /*
      void
      setGPUParams(const SPtr<GPUParams>& gpuParams,
                   const SPtr<CommandBuffer>& commandBuffer = nullptr) override;
      */

      /**
       * @copydoc RenderAPI::clearRenderTarget
       */
      void
      clearRenderTarget(uint32 buffers,
                        const LinearColor& color = LinearColor::Black,
                        float depth = 1.0f,
                        uint16 stencil = 0,
                        uint8 targetMask = 0xFF,
                        const SPtr<CommandBuffer>& commandBuffer = nullptr) override;

      /**
       * @copydoc RenderAPI::clearViewport
       */
      void clearViewport(uint32 buffers,
                         const LinearColor& color = LinearColor::Black,
                         float depth = 1.0f,
                         uint16 stencil = 0,
                         uint8 targetMask = 0xFF,
                         const SPtr<CommandBuffer>& commandBuffer = nullptr) override;

      /**
       * @copydoc RenderAPI::setRenderTarget
       */
      void
      setRenderTarget(const SPtr<RenderTarget>& target,
                      uint32 readOnlyFlags,
                      RenderSurfaceMask loadMask = RENDER_SURFACE_MASK_BITS::kNONE,
                      const SPtr<CommandBuffer>& commandBuffer = nullptr) override;

      /**
       * @copydoc RenderAPI::setViewport
       */
      void
      setViewport(const Box2D& area,
                  const SPtr<CommandBuffer>& commandBuffer = nullptr) override;

      /**
       * @copydoc RenderAPI::setScissorRect
       */
      void
      setScissorRect(uint32 left,
                     uint32 top,
                     uint32 right,
                     uint32 bottom,
                     const SPtr<CommandBuffer>& commandBuffer = nullptr) override;

      /**
       * @copydoc RenderAPI::setStencilRef
       */
      void
      setStencilRef(uint32 value,
                    const SPtr<CommandBuffer>& commandBuffer = nullptr) override;

      /**
       * @copydoc RenderAPI::setVertexBuffers
       */
      /*
      void
      setVertexBuffers(uint32 index,
                       SPtr<VertexBuffer>* buffers,
                       uint32 numBuffers,
                       const SPtr<CommandBuffer>& commandBuffer = nullptr) override;
      */

      /**
       * @copydoc RenderAPI::setIndexBuffer
       */
      /*
      void
      setIndexBuffer(const SPtr<IndexBuffer>& buffer,
                     const SPtr<CommandBuffer>& commandBuffer = nullptr) override;
      */

      /**
       * @copydoc RenderAPI::setVertexDeclaration
       */
      void
      setVertexDeclaration(const SPtr<VertexDeclaration>& vertexDeclaration,
                           const SPtr<CommandBuffer>& commandBuffer = nullptr) override;

      /**
       * @copydoc RenderAPI::setDrawOperation
       */
      void
      setDrawOperation(DrawOperationType op,
                       const SPtr<CommandBuffer>& commandBuffer = nullptr) override;

      /**
       * @copydoc RenderAPI::draw
       */
      void
      draw(uint32 vertexOffset,
           uint32 vertexCount,
           uint32 instanceCount = 0,
           const SPtr<CommandBuffer>& commandBuffer = nullptr) override;

      /**
       * @copydoc RenderAPI::drawIndexed
       */
      void
      drawIndexed(uint32 startIndex,
                  uint32 indexCount,
                  uint32 vertexOffset,
                  uint32 vertexCount,
                  uint32 instanceCount = 0,
                  const SPtr<CommandBuffer>& commandBuffer = nullptr) override;

      /**
       * @copydoc RenderAPI::dispatchCompute
       */
      void
      dispatchCompute(uint32 numGroupsX,
                      uint32 numGroupsY = 1,
                      uint32 numGroupsZ = 1,
                      const SPtr<CommandBuffer>& commandBuffer = nullptr) override;

      /**
       * @copydoc RenderAPI::swapBuffers()
       */
      void
      swapBuffers(const SPtr<RenderTarget>& target,
                  uint32 syncMask = 0xFFFFFFFF) override;

      /**
       * @copydoc RenderAPI::addCommands()
       */
      void
      addCommands(const SPtr<CommandBuffer>& commandBuffer,
                  const SPtr<CommandBuffer>& secondary) override;

      /**
       * @copydoc RenderAPI::submitCommandBuffer()
       */
      void
      submitCommandBuffer(const SPtr<CommandBuffer>& commandBuffer,
                          uint32 syncMask = 0xFFFFFFFF) override;

      /**
       * @copydoc RenderAPI::convertProjectionMatrix
       */
      void
      convertProjectionMatrix(const Matrix4& matrix,
                              Matrix4& dest) override;

      /**
       * @copydoc RenderAPI::getAPIInfo
       */
      const RenderAPIInfo&
      getAPIInfo() const override;

      /**
       * @copydoc RenderAPI::generateParamBlockDesc()
       */
      GPUParamBlockDesc
      generateParamBlockDesc(const String& name,
                             Vector<GPUParamDataDesc>& params) override;

      /***********************************************************************/
      /**
       * Internal use by DX11 backend only
       */
      /***********************************************************************/

      /**
       * @brief Determines DXGI multisample settings from the provided
       *        parameters.
       * @param[in] multisampleCount  Number of requested samples.
       * @param[in] format            Pixel format used by the render target.
       * @param[out]  outputSampleDesc  Output structure that will contain the
       *                                requested multisample settings.
       */
      void
      determineMultisampleSettings(uint32 multisampleCount,
                                   DXGI_FORMAT format,
                                   DXGI_SAMPLE_DESC* outputSampleDesc);

      /**
       * @brief Returns the main DXGI factory object.
       */
      IDXGIFactory*
      getDXGIFactory() const {
        return mDXGIFactory;
      }

      /**
       * @brief Returns the primary DX11 device object.
       */
      D3D11Device&
      getPrimaryDevice() const {
        return *mDevice;
      }

      /**
       * @brief Returns information describing all available drivers.
       */
      D3D11DriverList*
      getDriverList() const {
        return mDriverList;
      }

     protected:
      friend class D3D11RenderAPIFactory;

      /**
       * @copydoc RenderAPI::initialize
       */
      void
      initialize() override;

      /**
       * @copydoc RenderAPI::initializeWithWindow
       */
      void
      initializeWithWindow(const SPtr<RenderWindow>& primaryWindow) override;

      /**
       * @copydoc RenderAPI::destroyCore
       */
      void
      destroyCore() override;

      /**
       * @brief Creates or retrieves a proper input layout depending on the
       *        currently set vertex shader and vertex buffer.
       *        Applies the input layout to the pipeline.
       */
      void
      applyInputLayout();

      /**
       * @brief Recalculates actual viewport dimensions based on currently set
       *        viewport normalized dimensions and render target and applies
       *        them for further rendering.
       */
      void
      applyViewport();

      /**
       * @brief Creates and populates a set of render system capabilities
       *        describing which functionality is available.
       */
      void
      initCapabilites(IDXGIAdapter* adapter,
                      RenderAPICapabilities& caps) const;

     private:
      IDXGIFactory * mDXGIFactory;
      D3D11Device* mDevice;

      D3D11DriverList* mDriverList;
      D3D11Driver* mActiveD3DDriver;

      D3D_FEATURE_LEVEL mFeatureLevel;

      D3D11HLSLProgramFactory* mHLSLFactory;
      D3D11InputLayoutManager* mIAManager;

      bool mPSUAVsBound;
      bool mCSUAVsBound;

      uint32 mStencilRef;
      Box2D mViewportNorm;
      D3D11_VIEWPORT mViewport;
      D3D11_RECT mScissorRect;

      SPtr<VertexDeclaration> mActiveVertexDeclaration;
      SPtr<D3D11GPUProgram> mActiveVertexShader;
      SPtr<D3D11DepthStencilState> mActiveDepthStencilState;

      DrawOperationType mActiveDrawOp;
    };
  }
}
