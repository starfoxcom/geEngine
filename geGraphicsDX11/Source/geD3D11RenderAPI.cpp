/*****************************************************************************/
/**
 * @file    geD3D11RenderAPI.cpp
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

/*****************************************************************************/
/**
 * Includes
 */
/*****************************************************************************/
#include "geD3D11RenderAPI.h"
#include "geD3D11DriverList.h"
#include "geD3D11Driver.h"
#include "geD3D11Device.h"
/*
#include "geD3D11TextureManager.h"
#include "geD3D11Texture.h"
#include "geD3D11HardwareBufferManager.h"
#include "geD3D11RenderWindowManager.h"
#include "geD3D11HLSLProgramFactory.h"
#include "geD3D11BlendState.h"
#include "geD3D11RasterizerState.h"
#include "geD3D11DepthStencilState.h"
#include "geD3D11SamplerState.h"
#include "geD3D11GPUProgram.h"
#include "geD3D11Mappings.h"
#include "geD3D11VertexBuffer.h"
#include "geD3D11IndexBuffer.h"
#include "geD3D11RenderStateManager.h"
#include "geD3D11GPUParamBlockBuffer.h"
#include "geD3D11InputLayoutManager.h"
#include "geD3D11TextureView.h"
#include "geD3D11RenderUtility.h"
*/
#include "geD3D11QueryManager.h"
/*
#include "geD3D11GPUBuffer.h"
*/
#include "geD3D11CommandBuffer.h"
#include "geD3D11CommandBufferManager.h"

//#include <geGPUParams.h>
//#include <geGPUParamDesc.h>
#include <geCoreThread.h>

#include <geRenderStats.h>
#include <geDebug.h>
#include <geException.h>

namespace geEngineSDK {
  using std::min;
  using std::ref;

  namespace geCoreThread {
    D3D11RenderAPI::D3D11RenderAPI()
      : mDXGIFactory(nullptr),
        mDevice(nullptr),
        mDriverList(nullptr),
        mActiveD3DDriver(nullptr),
        mFeatureLevel(D3D_FEATURE_LEVEL_11_0),
        mHLSLFactory(nullptr),
        mIAManager(nullptr),
        mPSUAVsBound(false),
        mCSUAVsBound(false),
        mStencilRef(0),
        mActiveDrawOp(DOT_TRIANGLE_LIST),
        mViewportNorm(Vector2(0.0f, 0.0f), Vector2(1.0f, 1.0f))
    {}

    const StringID&
    D3D11RenderAPI::getName() const {
      static StringID strName("D3D11RenderAPI");
      return strName;
    }

    void
    D3D11RenderAPI::initialize() {
      THROW_IF_NOT_CORE_THREAD;

      HRESULT hr = CreateDXGIFactory(__uuidof(IDXGIFactory),
                                     reinterpret_cast<void**>(&mDXGIFactory));
      if (FAILED(hr)) {
        GE_EXCEPT(RenderingAPIException,
                  "Failed to create Direct3D 11 DXGIFactory");
      }

      mDriverList = ge_new<D3D11DriverList>(mDXGIFactory);
      
      //TODO: Always get first driver, for now
      mActiveD3DDriver = mDriverList->item(0);
      m_videoModeInfo = mActiveD3DDriver->getVideoModeInfo();

      GPUInfo gpuInfo;
      gpuInfo.numGPUs = min(5U, mDriverList->count());

      for (uint32 i = 0; i < gpuInfo.numGPUs; ++i) {
        gpuInfo.names[i] = mDriverList->item(i)->getDriverName();
      }

      PlatformUtility::_setGPUInfo(gpuInfo);

      IDXGIAdapter* selectedAdapter = mActiveD3DDriver->getDeviceAdapter();

      Vector<D3D_FEATURE_LEVEL> requestedLevels = {
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,
        D3D_FEATURE_LEVEL_9_3,
        D3D_FEATURE_LEVEL_9_2,
        D3D_FEATURE_LEVEL_9_1
      };

      uint32 deviceFlags = 0;
#if GE_DEBUG_MODE
      deviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
      ID3D11Device* device;
      hr = D3D11CreateDevice(selectedAdapter,
                             D3D_DRIVER_TYPE_UNKNOWN,
                             nullptr,
                             deviceFlags,
                             requestedLevels.data(),
                             static_cast<UINT>(requestedLevels.size()),
                             D3D11_SDK_VERSION,
                             &device,
                             &mFeatureLevel,
                             nullptr);

      if (FAILED(hr)) {
        GE_EXCEPT(RenderingAPIException,
                  "Failed to create Direct3D11 object. D3D11CreateDevice "
                  "returned this error code: " + toString(hr));
      }

      mDevice = ge_new<D3D11Device>(device);

      CommandBufferManager::startUp<D3D11CommandBufferManager>();

      //Create the texture manager for use by others
      geEngineSDK::TextureManager::startUp<geEngineSDK::D3D11TextureManager>();
      TextureManager::startUp<D3D11TextureManager>();

      //Create hardware buffer manager
      geEngineSDK::HardwareBufferManager::startUp();
      HardwareBufferManager::startUp<D3D11HardwareBufferManager>(ref(*mDevice));

      //Create render window manager
      geEngineSDK::RenderWindowManager::startUp<geEngineSDK::D3D11RenderWindowManager>(this);
      RenderWindowManager::startUp();

      //Create & register HLSL factory
      mHLSLFactory = ge_new<D3D11HLSLProgramFactory>();

      //Create render state manager
      RenderStateManager::startUp<D3D11RenderStateManager>();

      m_numDevices = 1;
      m_currentCapabilities = ge_newN<RenderAPICapabilities>(m_numDevices);
      initCapabilites(selectedAdapter, m_currentCapabilities[0]);

      GPUProgramManager::instance().addFactory("hlsl", mHLSLFactory);

      mIAManager = ge_new<D3D11InputLayoutManager>();

      RenderAPI::initialize();
    }

    void
    D3D11RenderAPI::initializeWithWindow(const SPtr<RenderWindow>& primaryWindow) {
      D3D11RenderUtility::startUp(mDevice);
      QueryManager::startUp<D3D11QueryManager>();
      RenderAPI::initializeWithWindow(primaryWindow);
    }

    void
    D3D11RenderAPI::destroyCore() {
      THROW_IF_NOT_CORE_THREAD;

      //Ensure that all GPU commands finish executing before shutting down the
      //device. If we don't do this a crash on shutdown may occur as the driver
      //is still executing the commands, and we unload this library.
      mDevice->getImmediateContext()->Flush();
      SPtr<EventQuery> query = EventQuery::create();
      query->begin();
      while (!query->isReady()) { /* Spin */ }
      query = nullptr;

      QueryManager::shutDown();
      D3D11RenderUtility::shutDown();

      if (nullptr != mIAManager) {
        ge_delete(mIAManager);
        mIAManager = nullptr;
      }

      if (nullptr != mHLSLFactory) {
        ge_delete(mHLSLFactory);
        mHLSLFactory = nullptr;
      }

      mActiveVertexDeclaration = nullptr;
      mActiveVertexShader = nullptr;
      m_activeRenderTarget = nullptr;
      mActiveDepthStencilState = nullptr;

      RenderStateManager::shutDown();
      RenderWindowManager::shutDown();
      geEngineSDK::RenderWindowManager::shutDown();
      HardwareBufferManager::shutDown();
      geEngineSDK::HardwareBufferManager::shutDown();
      TextureManager::shutDown();
      geEngineSDK::TextureManager::shutDown();
      CommandBufferManager::shutDown();

      SAFE_RELEASE(mDXGIFactory);

      if (nullptr != mDevice) {
        ge_delete(mDevice);
        mDevice = nullptr;
      }

      if (nullptr != mDriverList) {
        ge_delete(mDriverList);
        mDriverList = nullptr;
      }

      mActiveD3DDriver = nullptr;

      RenderAPI::destroyCore();
    }

    void
    D3D11RenderAPI::setGraphicsPipeline(const SPtr<GraphicsPipelineState>& pipelineState,
                                        const SPtr<CommandBuffer>& commandBuffer) {
      auto executeRef = [&](const SPtr<GraphicsPipelineState>& pipelineState) {
        THROW_IF_NOT_CORE_THREAD;

        D3D11BlendState* d3d11BlendState;
        D3D11RasterizerState* d3d11RasterizerState;

        D3D11GpuFragmentProgram* d3d11FragmentProgram;
        D3D11GpuGeometryProgram* d3d11GeometryProgram;
        D3D11GpuDomainProgram* d3d11DomainProgram;
        D3D11GpuHullProgram* d3d11HullProgram;

        if (nullptr != pipelineState) {
          d3d11BlendState = static_cast<D3D11BlendState*>(pipelineState->getBlendState().get());
          d3d11RasterizerState = static_cast<D3D11RasterizerState*>(pipelineState->getRasterizerState().get());
          mActiveDepthStencilState = std::static_pointer_cast<D3D11DepthStencilState>(pipelineState->getDepthStencilState());

          mActiveVertexShader = std::static_pointer_cast<D3D11GpuVertexProgram>(pipelineState->getVertexProgram());
          d3d11FragmentProgram = static_cast<D3D11GpuFragmentProgram*>(pipelineState->getFragmentProgram().get());
          d3d11GeometryProgram = static_cast<D3D11GpuGeometryProgram*>(pipelineState->getGeometryProgram().get());
          d3d11DomainProgram = static_cast<D3D11GpuDomainProgram*>(pipelineState->getDomainProgram().get());
          d3d11HullProgram = static_cast<D3D11GpuHullProgram*>(pipelineState->getHullProgram().get());

          if (d3d11BlendState == nullptr)
            d3d11BlendState = static_cast<D3D11BlendState*>(BlendState::getDefault().get());

          if (d3d11RasterizerState == nullptr)
            d3d11RasterizerState = static_cast<D3D11RasterizerState*>(RasterizerState::getDefault().get());

          if (mActiveDepthStencilState == nullptr)
            mActiveDepthStencilState = std::static_pointer_cast<D3D11DepthStencilState>(DepthStencilState::getDefault());
        }
        else
        {
          d3d11BlendState = static_cast<D3D11BlendState*>(BlendState::getDefault().get());
          d3d11RasterizerState = static_cast<D3D11RasterizerState*>(RasterizerState::getDefault().get());
          mActiveDepthStencilState = std::static_pointer_cast<D3D11DepthStencilState>(DepthStencilState::getDefault());

          mActiveVertexShader = nullptr;
          d3d11FragmentProgram = nullptr;
          d3d11GeometryProgram = nullptr;
          d3d11DomainProgram = nullptr;
          d3d11HullProgram = nullptr;
        }

        ID3D11DeviceContext* d3d11Context = mDevice->getImmediateContext();
        d3d11Context->OMSetBlendState(d3d11BlendState->getInternal(), nullptr, 0xFFFFFFFF);
        d3d11Context->RSSetState(d3d11RasterizerState->getInternal());
        d3d11Context->OMSetDepthStencilState(mActiveDepthStencilState->getInternal(), mStencilRef);

        if (nullptr != mActiveVertexShader) {
          D3D11GpuVertexProgram* vertexProgram = static_cast<D3D11GpuVertexProgram*>(mActiveVertexShader.get());
          d3d11Context->VSSetShader(vertexProgram->getVertexShader(), nullptr, 0);
        }
        else
          d3d11Context->VSSetShader(nullptr, nullptr, 0);

        if (d3d11FragmentProgram != nullptr)
          d3d11Context->PSSetShader(d3d11FragmentProgram->getPixelShader(), nullptr, 0);
        else
          d3d11Context->PSSetShader(nullptr, nullptr, 0);

        if (d3d11GeometryProgram != nullptr)
          d3d11Context->GSSetShader(d3d11GeometryProgram->getGeometryShader(), nullptr, 0);
        else
          d3d11Context->GSSetShader(nullptr, nullptr, 0);

        if (d3d11DomainProgram != nullptr)
          d3d11Context->DSSetShader(d3d11DomainProgram->getDomainShader(), nullptr, 0);
        else
          d3d11Context->DSSetShader(nullptr, nullptr, 0);

        if (d3d11HullProgram != nullptr)
          d3d11Context->HSSetShader(d3d11HullProgram->getHullShader(), nullptr, 0);
        else
          d3d11Context->HSSetShader(nullptr, nullptr, 0);

      };

      if (commandBuffer == nullptr)
        executeRef(pipelineState);
      else
      {
        auto execute = [=]() { executeRef(pipelineState); };

        SPtr<D3D11CommandBuffer> cb = std::static_pointer_cast<D3D11CommandBuffer>(commandBuffer);
        cb->queueCommand(execute);
      }

      GE_INC_RENDER_STAT(NumPipelineStateChanges);
    }

    void D3D11RenderAPI::setComputePipeline(const SPtr<ComputePipelineState>& pipelineState,
      const SPtr<CommandBuffer>& commandBuffer)
    {
      auto executeRef = [&](const SPtr<ComputePipelineState>& pipelineState)
      {
        THROW_IF_NOT_CORE_THREAD;

        SPtr<GpuProgram> program;
        if (pipelineState != nullptr)
          program = pipelineState->getProgram();

        if (program != nullptr && program->getType() == GPT_COMPUTE_PROGRAM)
        {
          D3D11GpuComputeProgram *d3d11ComputeProgram = static_cast<D3D11GpuComputeProgram*>(program.get());
          mDevice->getImmediateContext()->CSSetShader(d3d11ComputeProgram->getComputeShader(), nullptr, 0);
        }
        else
          mDevice->getImmediateContext()->CSSetShader(nullptr, nullptr, 0);
      };

      if (commandBuffer == nullptr)
        executeRef(pipelineState);
      else
      {
        auto execute = [=]() { executeRef(pipelineState); };

        SPtr<D3D11CommandBuffer> cb = std::static_pointer_cast<D3D11CommandBuffer>(commandBuffer);
        cb->queueCommand(execute);
      }

      GE_INC_RENDER_STAT(NumPipelineStateChanges);
    }

    void D3D11RenderAPI::setGpuParams(const SPtr<GpuParams>& gpuParams, const SPtr<CommandBuffer>& commandBuffer)
    {
      auto executeRef = [&](const SPtr<GpuParams>& gpuParams)
      {
        THROW_IF_NOT_CORE_THREAD;

        ID3D11DeviceContext* context = mDevice->getImmediateContext();

        // Clear any previously bound UAVs (otherwise shaders attempting to read resources viewed by those views will
        // be unable to)
        if (mPSUAVsBound || mCSUAVsBound)
        {
          ID3D11UnorderedAccessView* emptyUAVs[D3D11_PS_CS_UAV_REGISTER_COUNT];
          ge_zero_out(emptyUAVs);

          if (mPSUAVsBound)
          {
            context->OMSetRenderTargetsAndUnorderedAccessViews(
              D3D11_KEEP_RENDER_TARGETS_AND_DEPTH_STENCIL, nullptr, nullptr, 0,
              D3D11_PS_CS_UAV_REGISTER_COUNT, emptyUAVs, nullptr);

            mPSUAVsBound = false;
          }

          if (mCSUAVsBound)
          {
            context->CSSetUnorderedAccessViews(0, D3D11_PS_CS_UAV_REGISTER_COUNT, emptyUAVs, nullptr);

            mCSUAVsBound = false;
          }
        }

        ge_frame_mark();
        {
          FrameVector<ID3D11ShaderResourceView*> srvs(8);
          FrameVector<ID3D11UnorderedAccessView*> uavs(8);
          FrameVector<ID3D11Buffer*> constBuffers(8);
          FrameVector<ID3D11SamplerState*> samplers(8);

          auto populateViews = [&](GPUProgramType type)
          {
            srvs.clear();
            uavs.clear();
            constBuffers.clear();
            samplers.clear();

            SPtr<GPUParamDesc> paramDesc = gpuParams->getParamDesc(type);
            if (paramDesc == nullptr)
              return;

            for (auto iter = paramDesc->textures.begin(); iter != paramDesc->textures.end(); ++iter)
            {
              uint32 slot = iter->second.slot;

              SPtr<Texture> texture = gpuParams->getTexture(iter->second.set, slot);
              const TextureSurface& surface = gpuParams->getTextureSurface(iter->second.set, slot);

              while (slot >= (uint32)srvs.size())
                srvs.push_back(nullptr);

              if (texture != nullptr)
              {
                SPtr<TextureView> texView = texture->requestView(surface.mipLevel, surface.numMipLevels,
                  surface.face, surface.numFaces, GVU_DEFAULT);

                D3D11TextureView* d3d11texView = static_cast<D3D11TextureView*>(texView.get());
                srvs[slot] = d3d11texView->getSRV();
              }
            }

            for (auto iter = paramDesc->buffers.begin(); iter != paramDesc->buffers.end(); ++iter)
            {
              uint32 slot = iter->second.slot;
              SPtr<GPUBuffer> buffer = gpuParams->getBuffer(iter->second.set, slot);

              bool isLoadStore = iter->second.type != GPOT_BYTE_BUFFER &&
                iter->second.type != GPOT_STRUCTURED_BUFFER;

              if (!isLoadStore)
              {
                while (slot >= (uint32)srvs.size())
                  srvs.push_back(nullptr);

                if (buffer != nullptr)
                {
                  D3D11GPUBuffer* d3d11buffer = static_cast<D3D11GPUBuffer*>(buffer.get());
                  srvs[slot] = d3d11buffer->getSRV();
                }
              }
              else
              {
                while (slot >= (uint32)uavs.size())
                  uavs.push_back(nullptr);

                if (buffer != nullptr)
                {
                  D3D11GPUBuffer* d3d11buffer = static_cast<D3D11GPUBuffer*>(buffer.get());
                  uavs[slot] = d3d11buffer->getUAV();
                }
              }
            }

            for (auto iter = paramDesc->loadStoreTextures.begin(); iter != paramDesc->loadStoreTextures.end(); ++iter)
            {
              uint32 slot = iter->second.slot;

              SPtr<Texture> texture = gpuParams->getLoadStoreTexture(iter->second.set, slot);
              const TextureSurface& surface = gpuParams->getLoadStoreSurface(iter->second.set, slot);

              while (slot >= (uint32)uavs.size())
                uavs.push_back(nullptr);

              if (texture != nullptr)
              {
                SPtr<TextureView> texView = texture->requestView(surface.mipLevel, 1,
                  surface.face, surface.numFaces, GVU_RANDOMWRITE);

                D3D11TextureView* d3d11texView = static_cast<D3D11TextureView*>(texView.get());
                uavs[slot] = d3d11texView->getUAV();
              }
              else
              {
                uavs[slot] = nullptr;
              }
            }

            for (auto iter = paramDesc->samplers.begin(); iter != paramDesc->samplers.end(); ++iter)
            {
              uint32 slot = iter->second.slot;
              SPtr<SamplerState> samplerState = gpuParams->getSamplerState(iter->second.set, slot);

              while (slot >= (uint32)samplers.size())
                samplers.push_back(nullptr);

              if (samplerState == nullptr)
                samplerState = SamplerState::getDefault();

              D3D11SamplerState* d3d11SamplerState =
                static_cast<D3D11SamplerState*>(const_cast<SamplerState*>(samplerState.get()));
              samplers[slot] = d3d11SamplerState->getInternal();
            }

            for (auto iter = paramDesc->paramBlocks.begin(); iter != paramDesc->paramBlocks.end(); ++iter)
            {
              uint32 slot = iter->second.slot;
              SPtr<GpuParamBlockBuffer> buffer = gpuParams->getParamBlockBuffer(iter->second.set, slot);

              while (slot >= (uint32)constBuffers.size())
                constBuffers.push_back(nullptr);

              if (buffer != nullptr)
              {
                buffer->flushToGPU();

                const D3D11GpuParamBlockBuffer* d3d11paramBlockBuffer =
                  static_cast<const D3D11GpuParamBlockBuffer*>(buffer.get());
                constBuffers[slot] = d3d11paramBlockBuffer->getD3D11Buffer();
              }
            }
          };

          uint32 numSRVs = 0;
          uint32 numUAVs = 0;
          uint32 numConstBuffers = 0;
          uint32 numSamplers = 0;

          populateViews(GPT_VERTEX_PROGRAM);
          numSRVs = (uint32)srvs.size();
          numConstBuffers = (uint32)constBuffers.size();
          numSamplers = (uint32)samplers.size();

          if (numSRVs > 0)
            context->VSSetShaderResources(0, numSRVs, srvs.data());

          if (numConstBuffers > 0)
            context->VSSetConstantBuffers(0, numConstBuffers, constBuffers.data());

          if (numSamplers > 0)
            context->VSSetSamplers(0, numSamplers, samplers.data());

          populateViews(GPT_FRAGMENT_PROGRAM);
          numSRVs = (uint32)srvs.size();
          numUAVs = (uint32)uavs.size();
          numConstBuffers = (uint32)constBuffers.size();
          numSamplers = (uint32)samplers.size();

          if (numSRVs > 0)
            context->PSSetShaderResources(0, numSRVs, srvs.data());

          if (numUAVs > 0)
          {
            context->OMSetRenderTargetsAndUnorderedAccessViews(
              D3D11_KEEP_RENDER_TARGETS_AND_DEPTH_STENCIL, nullptr, nullptr, 0, numUAVs, uavs.data(), nullptr);
            mPSUAVsBound = true;
          }

          if (numConstBuffers > 0)
            context->PSSetConstantBuffers(0, numConstBuffers, constBuffers.data());

          if (numSamplers > 0)
            context->PSSetSamplers(0, numSamplers, samplers.data());

          populateViews(GPT_GEOMETRY_PROGRAM);
          numSRVs = (uint32)srvs.size();
          numConstBuffers = (uint32)constBuffers.size();
          numSamplers = (uint32)samplers.size();

          if (numSRVs > 0)
            context->GSSetShaderResources(0, numSRVs, srvs.data());

          if (numConstBuffers > 0)
            context->GSSetConstantBuffers(0, numConstBuffers, constBuffers.data());

          if (numSamplers > 0)
            context->GSSetSamplers(0, numSamplers, samplers.data());

          populateViews(GPT_HULL_PROGRAM);
          numSRVs = (uint32)srvs.size();
          numConstBuffers = (uint32)constBuffers.size();
          numSamplers = (uint32)samplers.size();

          if (numSRVs > 0)
            context->HSSetShaderResources(0, numSRVs, srvs.data());

          if (numConstBuffers > 0)
            context->HSSetConstantBuffers(0, numConstBuffers, constBuffers.data());

          if (numSamplers > 0)
            context->HSSetSamplers(0, numSamplers, samplers.data());

          populateViews(GPT_DOMAIN_PROGRAM);
          numSRVs = (uint32)srvs.size();
          numConstBuffers = (uint32)constBuffers.size();
          numSamplers = (uint32)samplers.size();

          if (numSRVs > 0)
            context->DSSetShaderResources(0, numSRVs, srvs.data());

          if (numConstBuffers > 0)
            context->DSSetConstantBuffers(0, numConstBuffers, constBuffers.data());

          if (numSamplers > 0)
            context->DSSetSamplers(0, numSamplers, samplers.data());

          populateViews(GPT_COMPUTE_PROGRAM);
          numSRVs = (uint32)srvs.size();
          numUAVs = (uint32)uavs.size();
          numConstBuffers = (uint32)constBuffers.size();
          numSamplers = (uint32)samplers.size();

          if (numSRVs > 0)
            context->CSSetShaderResources(0, numSRVs, srvs.data());

          if (numUAVs > 0)
          {
            context->CSSetUnorderedAccessViews(0, numUAVs, uavs.data(), nullptr);
            mCSUAVsBound = true;
          }

          if (numConstBuffers > 0)
            context->CSSetConstantBuffers(0, numConstBuffers, constBuffers.data());

          if (numSamplers > 0)
            context->CSSetSamplers(0, numSamplers, samplers.data());

        }
        ge_frame_clear();

        if (mDevice->hasError())
          GE_EXCEPT(RenderingAPIException, "Failed to set GPU parameters: " + mDevice->getErrorDescription());
      };

      if (commandBuffer == nullptr)
        executeRef(gpuParams);
      else
      {
        auto execute = [=]() { executeRef(gpuParams); };

        SPtr<D3D11CommandBuffer> cb = std::static_pointer_cast<D3D11CommandBuffer>(commandBuffer);
        cb->queueCommand(execute);
      }

      GE_INC_RENDER_STAT(NumGpuParamBinds);
    }

    void D3D11RenderAPI::setViewport(const Rect2& vp, const SPtr<CommandBuffer>& commandBuffer)
    {
      auto executeRef = [&](const Rect2& vp)
      {
        THROW_IF_NOT_CORE_THREAD;

        mViewportNorm = vp;
        applyViewport();
      };

      if (commandBuffer == nullptr)
        executeRef(vp);
      else
      {
        auto execute = [=]() { executeRef(vp); };

        SPtr<D3D11CommandBuffer> cb = std::static_pointer_cast<D3D11CommandBuffer>(commandBuffer);
        cb->queueCommand(execute);
      }
    }

    void D3D11RenderAPI::setVertexBuffers(uint32 index, SPtr<VertexBuffer>* buffers, uint32 numBuffers,
      const SPtr<CommandBuffer>& commandBuffer)
    {
      auto executeRef = [&](uint32 index, SPtr<VertexBuffer>* buffers, uint32 numBuffers)
      {
        THROW_IF_NOT_CORE_THREAD;

        uint32 maxBoundVertexBuffers = mCurrentCapabilities[0].getMaxBoundVertexBuffers();
        if (index < 0 || (index + numBuffers) >= maxBoundVertexBuffers)
        {
          GE_EXCEPT(InvalidParametersException, "Invalid vertex index: " + toString(index) +
            ". Valid range is 0 .. " + toString(maxBoundVertexBuffers - 1));
        }

        ID3D11Buffer* dx11buffers[GE_MAX_BOUND_VERTEX_BUFFERS];
        uint32 strides[GE_MAX_BOUND_VERTEX_BUFFERS];
        uint32 offsets[GE_MAX_BOUND_VERTEX_BUFFERS];

        for (uint32 i = 0; i < numBuffers; i++)
        {
          SPtr<D3D11VertexBuffer> vertexBuffer = std::static_pointer_cast<D3D11VertexBuffer>(buffers[i]);
          const VertexBufferProperties& vbProps = vertexBuffer->getProperties();

          dx11buffers[i] = vertexBuffer->getD3DVertexBuffer();

          strides[i] = vbProps.getVertexSize();
          offsets[i] = 0;
        }

        mDevice->getImmediateContext()->IASetVertexBuffers(index, numBuffers, dx11buffers, strides, offsets);
      };

      if (commandBuffer == nullptr)
        executeRef(index, buffers, numBuffers);
      else
      {
        auto execute = [=]() { executeRef(index, buffers, numBuffers); };

        SPtr<D3D11CommandBuffer> cb = std::static_pointer_cast<D3D11CommandBuffer>(commandBuffer);
        cb->queueCommand(execute);
      }

      GE_INC_RENDER_STAT(NumVertexBufferBinds);
    }

    void D3D11RenderAPI::setIndexBuffer(const SPtr<IndexBuffer>& buffer, const SPtr<CommandBuffer>& commandBuffer)
    {
      auto executeRef = [&](const SPtr<IndexBuffer>& buffer)
      {
        THROW_IF_NOT_CORE_THREAD;

        SPtr<D3D11IndexBuffer> indexBuffer = std::static_pointer_cast<D3D11IndexBuffer>(buffer);

        DXGI_FORMAT indexFormat = DXGI_FORMAT_R16_uint;
        if (indexBuffer->getProperties().getType() == IT_16BIT)
          indexFormat = DXGI_FORMAT_R16_uint;
        else if (indexBuffer->getProperties().getType() == IT_32BIT)
          indexFormat = DXGI_FORMAT_R32_uint;
        else
          GE_EXCEPT(InternalErrorException, "Unsupported index format: " + toString(indexBuffer->getProperties().getType()));

        mDevice->getImmediateContext()->IASetIndexBuffer(indexBuffer->getD3DIndexBuffer(), indexFormat, 0);
      };

      if (commandBuffer == nullptr)
        executeRef(buffer);
      else
      {
        auto execute = [=]() { executeRef(buffer); };

        SPtr<D3D11CommandBuffer> cb = std::static_pointer_cast<D3D11CommandBuffer>(commandBuffer);
        cb->queueCommand(execute);
      }

      GE_INC_RENDER_STAT(NumIndexBufferBinds);
    }

    void D3D11RenderAPI::setVertexDeclaration(const SPtr<VertexDeclaration>& vertexDeclaration,
      const SPtr<CommandBuffer>& commandBuffer)
    {
      auto executeRef = [&](const SPtr<VertexDeclaration>& vertexDeclaration)
      {
        THROW_IF_NOT_CORE_THREAD;

        mActiveVertexDeclaration = vertexDeclaration;
      };

      if (commandBuffer == nullptr)
        executeRef(vertexDeclaration);
      else
      {
        auto execute = [=]() { executeRef(vertexDeclaration); };

        SPtr<D3D11CommandBuffer> cb = std::static_pointer_cast<D3D11CommandBuffer>(commandBuffer);
        cb->queueCommand(execute);
      }
    }

    void D3D11RenderAPI::setDrawOperation(DrawOperationType op, const SPtr<CommandBuffer>& commandBuffer)
    {
      auto executeRef = [&](DrawOperationType op)
      {
        THROW_IF_NOT_CORE_THREAD;

        mDevice->getImmediateContext()->IASetPrimitiveTopology(D3D11Mappings::getPrimitiveType(op));
        mActiveDrawOp = op;
      };

      if (commandBuffer == nullptr)
        executeRef(op);
      else
      {
        auto execute = [=]() { executeRef(op); };

        SPtr<D3D11CommandBuffer> cb = std::static_pointer_cast<D3D11CommandBuffer>(commandBuffer);
        cb->queueCommand(execute);

        cb->mActiveDrawOp = op;
      }
    }

    void D3D11RenderAPI::draw(uint32 vertexOffset, uint32 vertexCount, uint32 instanceCount,
      const SPtr<CommandBuffer>& commandBuffer)
    {
      auto executeRef = [&](uint32 vertexOffset, uint32 vertexCount, uint32 instanceCount)
      {
        THROW_IF_NOT_CORE_THREAD;

        applyInputLayout();

        if (instanceCount <= 1)
          mDevice->getImmediateContext()->Draw(vertexCount, vertexOffset);
        else
          mDevice->getImmediateContext()->DrawInstanced(vertexCount, instanceCount, vertexOffset, 0);

#if GE_DEBUG_MODE
        if (mDevice->hasError())
          LOGWRN(mDevice->getErrorDescription());
#endif
      };

      uint32 primCount;
      if (commandBuffer == nullptr)
      {
        executeRef(vertexOffset, vertexCount, instanceCount);
        primCount = vertexCountToPrimCount(mActiveDrawOp, vertexCount);
      }
      else
      {
        auto execute = [=]() { executeRef(vertexOffset, vertexCount, instanceCount); };

        SPtr<D3D11CommandBuffer> cb = std::static_pointer_cast<D3D11CommandBuffer>(commandBuffer);
        cb->queueCommand(execute);

        primCount = vertexCountToPrimCount(cb->mActiveDrawOp, vertexCount);
      }

      GE_INC_RENDER_STAT(NumDrawCalls);
      GE_ADD_RENDER_STAT(NumVertices, vertexCount);
      GE_ADD_RENDER_STAT(NumPrimitives, primCount);
    }

    void D3D11RenderAPI::drawIndexed(uint32 startIndex, uint32 indexCount, uint32 vertexOffset, uint32 vertexCount,
      uint32 instanceCount, const SPtr<CommandBuffer>& commandBuffer)
    {
      auto executeRef = [&](uint32 startIndex, uint32 indexCount, uint32 vertexOffset, uint32 vertexCount,
        uint32 instanceCount)
      {
        THROW_IF_NOT_CORE_THREAD;

        applyInputLayout();

        if (instanceCount <= 1)
          mDevice->getImmediateContext()->DrawIndexed(indexCount, startIndex, vertexOffset);
        else
          mDevice->getImmediateContext()->DrawIndexedInstanced(indexCount, instanceCount, startIndex, vertexOffset, 0);

#if GE_DEBUG_MODE
        if (mDevice->hasError())
          LOGWRN(mDevice->getErrorDescription());
#endif
      };

      uint32 primCount;
      if (commandBuffer == nullptr)
      {
        executeRef(startIndex, indexCount, vertexOffset, vertexCount, instanceCount);
        primCount = vertexCountToPrimCount(mActiveDrawOp, indexCount);
      }
      else
      {
        auto execute = [=]() { executeRef(startIndex, indexCount, vertexOffset, vertexCount, instanceCount); };

        SPtr<D3D11CommandBuffer> cb = std::static_pointer_cast<D3D11CommandBuffer>(commandBuffer);
        cb->queueCommand(execute);

        primCount = vertexCountToPrimCount(cb->mActiveDrawOp, indexCount);
      }

      BS_INC_RENDER_STAT(NumDrawCalls);
      BS_ADD_RENDER_STAT(NumVertices, vertexCount);
      BS_ADD_RENDER_STAT(NumPrimitives, primCount);
    }

    void D3D11RenderAPI::dispatchCompute(uint32 numGroupsX, uint32 numGroupsY, uint32 numGroupsZ,
      const SPtr<CommandBuffer>& commandBuffer)
    {
      auto executeRef = [&](uint32 numGroupsX, uint32 numGroupsY, uint32 numGroupsZ)
      {
        THROW_IF_NOT_CORE_THREAD;

        mDevice->getImmediateContext()->Dispatch(numGroupsX, numGroupsY, numGroupsZ);

#if GE_DEBUG_MODE
        if (mDevice->hasError())
          LOGWRN(mDevice->getErrorDescription());
#endif
      };

      if (commandBuffer == nullptr)
        executeRef(numGroupsX, numGroupsY, numGroupsZ);
      else
      {
        auto execute = [=]() { executeRef(numGroupsX, numGroupsY, numGroupsZ); };

        SPtr<D3D11CommandBuffer> cb = std::static_pointer_cast<D3D11CommandBuffer>(commandBuffer);
        cb->queueCommand(execute);
      }

      GE_INC_RENDER_STAT(NumComputeCalls);
    }

    void D3D11RenderAPI::setScissorRect(uint32 left, uint32 top, uint32 right, uint32 bottom,
      const SPtr<CommandBuffer>& commandBuffer)
    {
      auto executeRef = [&](uint32 left, uint32 top, uint32 right, uint32 bottom)
      {
        THROW_IF_NOT_CORE_THREAD;

        mScissorRect.left = static_cast<LONG>(left);
        mScissorRect.top = static_cast<LONG>(top);
        mScissorRect.bottom = static_cast<LONG>(bottom);
        mScissorRect.right = static_cast<LONG>(right);

        mDevice->getImmediateContext()->RSSetScissorRects(1, &mScissorRect);
      };

      if (commandBuffer == nullptr)
        executeRef(left, top, right, bottom);
      else
      {
        auto execute = [=]() { executeRef(left, top, right, bottom); };

        SPtr<D3D11CommandBuffer> cb = std::static_pointer_cast<D3D11CommandBuffer>(commandBuffer);
        cb->queueCommand(execute);
      }
    }

    void D3D11RenderAPI::setStencilRef(uint32 value, const SPtr<CommandBuffer>& commandBuffer)
    {
      auto executeRef = [&](uint32 value)
      {
        THROW_IF_NOT_CORE_THREAD;

        mStencilRef = value;

        if (mActiveDepthStencilState != nullptr)
          mDevice->getImmediateContext()->OMSetDepthStencilState(mActiveDepthStencilState->getInternal(), mStencilRef);
        else
          mDevice->getImmediateContext()->OMSetDepthStencilState(nullptr, mStencilRef);
      };

      if (commandBuffer == nullptr)
        executeRef(value);
      else
      {
        auto execute = [=]() { executeRef(value); };

        SPtr<D3D11CommandBuffer> cb = std::static_pointer_cast<D3D11CommandBuffer>(commandBuffer);
        cb->queueCommand(execute);
      }
    }

    void D3D11RenderAPI::clearViewport(uint32 buffers, const Color& color, float depth, uint16 stencil, uint8 targetMask,
      const SPtr<CommandBuffer>& commandBuffer)
    {
      auto executeRef = [&](uint32 buffers, const Color& color, float depth, uint16 stencil, uint8 targetMask)
      {
        THROW_IF_NOT_CORE_THREAD;

        if (mActiveRenderTarget == nullptr)
          return;

        const RenderTargetProperties& rtProps = mActiveRenderTarget->getProperties();

        Rect2I clearArea((int)mViewport.TopLeftX, (int)mViewport.TopLeftY, (int)mViewport.Width, (int)mViewport.Height);

        bool clearEntireTarget = clearArea.width == 0 || clearArea.height == 0;
        clearEntireTarget |= (clearArea.x == 0 && clearArea.y == 0 && clearArea.width == rtProps.width &&
          clearArea.height == rtProps.height);

        if (!clearEntireTarget)
        {
          // TODO - Ignoring targetMask here
          D3D11RenderUtility::instance().drawClearQuad(buffers, color, depth, stencil);
          BS_INC_RENDER_STAT(NumClears);
        }
        else
          clearRenderTarget(buffers, color, depth, stencil, targetMask);
      };

      if (commandBuffer == nullptr)
        executeRef(buffers, color, depth, stencil, targetMask);
      else
      {
        auto execute = [=]() { executeRef(buffers, color, depth, stencil, targetMask); };

        SPtr<D3D11CommandBuffer> cb = std::static_pointer_cast<D3D11CommandBuffer>(commandBuffer);
        cb->queueCommand(execute);
      }
    }

    void D3D11RenderAPI::clearRenderTarget(uint32 buffers, const Color& color, float depth, uint16 stencil,
      uint8 targetMask, const SPtr<CommandBuffer>& commandBuffer)
    {
      auto executeRef = [&](uint32 buffers, const Color& color, float depth, uint16 stencil, uint8 targetMask)
      {
        THROW_IF_NOT_CORE_THREAD;

        if (mActiveRenderTarget == nullptr)
          return;

        // Clear render surfaces
        if (buffers & FBT_COLOR)
        {
          uint32 maxRenderTargets = mCurrentCapabilities[0].getNumMultiRenderTargets();

          ID3D11RenderTargetView** views = bs_newN<ID3D11RenderTargetView*>(maxRenderTargets);
          memset(views, 0, sizeof(ID3D11RenderTargetView*) * maxRenderTargets);

          mActiveRenderTarget->getCustomAttribute("RTV", views);
          if (!views[0])
          {
            bs_deleteN(views, maxRenderTargets);
            return;
          }

          float clearColor[4];
          clearColor[0] = color.r;
          clearColor[1] = color.g;
          clearColor[2] = color.b;
          clearColor[3] = color.a;

          for (uint32 i = 0; i < maxRenderTargets; i++)
          {
            if (views[i] != nullptr && ((1 << i) & targetMask) != 0)
              mDevice->getImmediateContext()->ClearRenderTargetView(views[i], clearColor);
          }

          bs_deleteN(views, maxRenderTargets);
        }

        // Clear depth stencil
        if ((buffers & FBT_DEPTH) != 0 || (buffers & FBT_STENCIL) != 0)
        {
          ID3D11DepthStencilView* depthStencilView = nullptr;
          mActiveRenderTarget->getCustomAttribute("DSV", &depthStencilView);

          D3D11_CLEAR_FLAG clearFlag;

          if ((buffers & FBT_DEPTH) != 0 && (buffers & FBT_STENCIL) != 0)
            clearFlag = (D3D11_CLEAR_FLAG)(D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL);
          else if ((buffers & FBT_STENCIL) != 0)
            clearFlag = D3D11_CLEAR_STENCIL;
          else
            clearFlag = D3D11_CLEAR_DEPTH;

          if (depthStencilView != nullptr)
            mDevice->getImmediateContext()->ClearDepthStencilView(depthStencilView, clearFlag, depth, (uint8)stencil);
        }
      };

      if (commandBuffer == nullptr)
        executeRef(buffers, color, depth, stencil, targetMask);
      else
      {
        auto execute = [=]() { executeRef(buffers, color, depth, stencil, targetMask); };

        SPtr<D3D11CommandBuffer> cb = std::static_pointer_cast<D3D11CommandBuffer>(commandBuffer);
        cb->queueCommand(execute);
      }

      GE_INC_RENDER_STAT(NumClears);
    }

    void D3D11RenderAPI::setRenderTarget(const SPtr<RenderTarget>& target, uint32 readOnlyFlags,
      RenderSurfaceMask loadMask, const SPtr<CommandBuffer>& commandBuffer)
    {
      auto executeRef = [&](const SPtr<RenderTarget>& target, uint32 readOnlyFlags)
      {
        THROW_IF_NOT_CORE_THREAD;

        mActiveRenderTarget = target;

        uint32 maxRenderTargets = mCurrentCapabilities[0].getNumMultiRenderTargets();
        ID3D11RenderTargetView** views = bs_newN<ID3D11RenderTargetView*>(maxRenderTargets);
        memset(views, 0, sizeof(ID3D11RenderTargetView*) * maxRenderTargets);

        ID3D11DepthStencilView* depthStencilView = nullptr;

        if (target != nullptr)
        {
          target->getCustomAttribute("RTV", views);

          if ((readOnlyFlags & FBT_DEPTH) == 0)
          {
            if ((readOnlyFlags & FBT_STENCIL) == 0)
              target->getCustomAttribute("DSV", &depthStencilView);
            else
              target->getCustomAttribute("WDROSV", &depthStencilView);
          }
          else
          {
            if ((readOnlyFlags & FBT_STENCIL) == 0)
              target->getCustomAttribute("RODWSV", &depthStencilView);
            else
              target->getCustomAttribute("RODSV", &depthStencilView);
          }
        }

        // Bind render targets
        mDevice->getImmediateContext()->OMSetRenderTargets(maxRenderTargets, views, depthStencilView);
        if (mDevice->hasError())
          GE_EXCEPT(RenderingAPIException, "Failed to setRenderTarget : " + mDevice->getErrorDescription());

        ge_deleteN(views, maxRenderTargets);
        applyViewport();
      };

      if (commandBuffer == nullptr)
        executeRef(target, readOnlyFlags);
      else
      {
        auto execute = [=]() { executeRef(target, readOnlyFlags); };

        SPtr<D3D11CommandBuffer> cb = std::static_pointer_cast<D3D11CommandBuffer>(commandBuffer);
        cb->queueCommand(execute);
      }

      GE_INC_RENDER_STAT(NumRenderTargetChanges);
    }

    void D3D11RenderAPI::swapBuffers(const SPtr<RenderTarget>& target, uint32 syncMask)
    {
      THROW_IF_NOT_CORE_THREAD;
      target->swapBuffers();

      GE_INC_RENDER_STAT(NumPresents);
    }

    void D3D11RenderAPI::addCommands(const SPtr<CommandBuffer>& commandBuffer, const SPtr<CommandBuffer>& secondary)
    {
      SPtr<D3D11CommandBuffer> cb = std::static_pointer_cast<D3D11CommandBuffer>(commandBuffer);
      SPtr<D3D11CommandBuffer> secondaryCb = std::static_pointer_cast<D3D11CommandBuffer>(secondary);

      cb->appendSecondary(secondaryCb);
    }

    void D3D11RenderAPI::submitCommandBuffer(const SPtr<CommandBuffer>& commandBuffer, uint32 syncMask)
    {
      SPtr<D3D11CommandBuffer> cb = std::static_pointer_cast<D3D11CommandBuffer>(commandBuffer);
      if (cb == nullptr)
        return;

      cb->executeCommands();
      cb->clear();
    }

    void D3D11RenderAPI::applyViewport()
    {
      if (mActiveRenderTarget == nullptr)
        return;

      const RenderTargetProperties& rtProps = mActiveRenderTarget->getProperties();

      // Set viewport dimensions
      mViewport.TopLeftX = (FLOAT)(rtProps.width * mViewportNorm.x);
      mViewport.TopLeftY = (FLOAT)(rtProps.height * mViewportNorm.y);
      mViewport.Width = (FLOAT)(rtProps.width * mViewportNorm.width);
      mViewport.Height = (FLOAT)(rtProps.height * mViewportNorm.height);

      if (rtProps.requiresTextureFlipping)
      {
        // Convert "top-left" to "bottom-left"
        mViewport.TopLeftY = rtProps.height - mViewport.Height - mViewport.TopLeftY;
      }

      mViewport.MinDepth = 0.0f;
      mViewport.MaxDepth = 1.0f;

      mDevice->getImmediateContext()->RSSetViewports(1, &mViewport);
    }

    void D3D11RenderAPI::initCapabilites(IDXGIAdapter* adapter, RenderAPICapabilities& caps) const
    {
      THROW_IF_NOT_CORE_THREAD;

      // This must query for DirectX 10 interface as this is unsupported for DX11
      LARGE_INTEGER driverVersionNum;
      DriverVersion driverVersion;
      if (SUCCEEDED(adapter->CheckInterfaceSupport(IID_ID3D10Device, &driverVersionNum)))
      {
        driverVersion.major = HIWORD(driverVersionNum.HighPart);
        driverVersion.minor = LOWORD(driverVersionNum.HighPart);
        driverVersion.release = HIWORD(driverVersionNum.LowPart);
        driverVersion.build = LOWORD(driverVersionNum.LowPart);
      }

      caps.setDriverVersion(driverVersion);
      caps.setDeviceName(mActiveD3DDriver->getDriverDescription());
      caps.setRenderAPIName(getName());

      caps.setCapability(RSC_TEXTURE_COMPRESSION_BC);
      caps.addShaderProfile("hlsl");

      if (mFeatureLevel >= D3D_FEATURE_LEVEL_10_1)
        caps.setMaxBoundVertexBuffers(32);
      else
        caps.setMaxBoundVertexBuffers(16);

      if (mFeatureLevel >= D3D_FEATURE_LEVEL_10_0)
      {
        caps.setCapability(RSC_GEOMETRY_PROGRAM);

        caps.setNumTextureUnits(GPT_FRAGMENT_PROGRAM, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT);
        caps.setNumTextureUnits(GPT_VERTEX_PROGRAM, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT);
        caps.setNumTextureUnits(GPT_GEOMETRY_PROGRAM, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT);

        caps.setNumCombinedTextureUnits(caps.getNumTextureUnits(GPT_FRAGMENT_PROGRAM)
          + caps.getNumTextureUnits(GPT_VERTEX_PROGRAM) + caps.getNumTextureUnits(GPT_GEOMETRY_PROGRAM));

        caps.setNumGpuParamBlockBuffers(GPT_FRAGMENT_PROGRAM, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT);
        caps.setNumGpuParamBlockBuffers(GPT_VERTEX_PROGRAM, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT);
        caps.setNumGpuParamBlockBuffers(GPT_GEOMETRY_PROGRAM, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT);

        caps.setNumCombinedGpuParamBlockBuffers(caps.getNumGpuParamBlockBuffers(GPT_FRAGMENT_PROGRAM)
          + caps.getNumGpuParamBlockBuffers(GPT_VERTEX_PROGRAM) + caps.getNumGpuParamBlockBuffers(GPT_GEOMETRY_PROGRAM));
      }

      if (mFeatureLevel >= D3D_FEATURE_LEVEL_11_0)
      {
        caps.setCapability(RSC_TESSELLATION_PROGRAM);
        caps.setCapability(RSC_COMPUTE_PROGRAM);

        caps.setNumTextureUnits(GPT_HULL_PROGRAM, D3D11_COMMONSHADER_INPUT_RESOURCE_REGISTER_COUNT);
        caps.setNumTextureUnits(GPT_DOMAIN_PROGRAM, D3D11_COMMONSHADER_INPUT_RESOURCE_REGISTER_COUNT);
        caps.setNumTextureUnits(GPT_COMPUTE_PROGRAM, D3D11_COMMONSHADER_INPUT_RESOURCE_REGISTER_COUNT);

        caps.setNumCombinedTextureUnits(caps.getNumTextureUnits(GPT_FRAGMENT_PROGRAM)
          + caps.getNumTextureUnits(GPT_VERTEX_PROGRAM) + caps.getNumTextureUnits(GPT_GEOMETRY_PROGRAM)
          + caps.getNumTextureUnits(GPT_HULL_PROGRAM) + caps.getNumTextureUnits(GPT_DOMAIN_PROGRAM)
          + caps.getNumTextureUnits(GPT_COMPUTE_PROGRAM));

        caps.setNumGpuParamBlockBuffers(GPT_HULL_PROGRAM, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT);
        caps.setNumGpuParamBlockBuffers(GPT_DOMAIN_PROGRAM, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT);
        caps.setNumGpuParamBlockBuffers(GPT_COMPUTE_PROGRAM, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT);

        caps.setNumCombinedGpuParamBlockBuffers(caps.getNumGpuParamBlockBuffers(GPT_FRAGMENT_PROGRAM)
          + caps.getNumGpuParamBlockBuffers(GPT_VERTEX_PROGRAM) + caps.getNumGpuParamBlockBuffers(GPT_GEOMETRY_PROGRAM)
          + caps.getNumGpuParamBlockBuffers(GPT_HULL_PROGRAM) + caps.getNumGpuParamBlockBuffers(GPT_DOMAIN_PROGRAM)
          + caps.getNumGpuParamBlockBuffers(GPT_COMPUTE_PROGRAM));

        caps.setNumLoadStoreTextureUnits(GPT_FRAGMENT_PROGRAM, D3D11_PS_CS_UAV_REGISTER_COUNT);
        caps.setNumLoadStoreTextureUnits(GPT_COMPUTE_PROGRAM, D3D11_PS_CS_UAV_REGISTER_COUNT);

        caps.setNumCombinedLoadStoreTextureUnits(caps.getNumLoadStoreTextureUnits(GPT_FRAGMENT_PROGRAM)
          + caps.getNumLoadStoreTextureUnits(GPT_COMPUTE_PROGRAM));
      }

      // Adapter details
      const DXGI_ADAPTER_DESC& adapterID = mActiveD3DDriver->getAdapterIdentifier();

      // Determine vendor
      switch (adapterID.VendorId)
      {
      case 0x10DE:
        caps.setVendor(GPU_NVIDIA);
        break;
      case 0x1002:
        caps.setVendor(GPU_AMD);
        break;
      case 0x163C:
      case 0x8086:
        caps.setVendor(GPU_INTEL);
        break;
      default:
        caps.setVendor(GPU_UNKNOWN);
        break;
      };

      caps.setNumMultiRenderTargets(D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT);
    }

    void D3D11RenderAPI::determineMultisampleSettings(uint32 multisampleCount, DXGI_FORMAT format, DXGI_SAMPLE_DESC* outputSampleDesc)
    {
      if (multisampleCount == 0 || multisampleCount == 1)
      {
        outputSampleDesc->Count = 1;
        outputSampleDesc->Quality = 0;

        return;
      }

      bool tryCSAA = false; // Note: Disabled for now, but leaving the code for later so it might be useful
      enum CSAAMode { CSAA_Normal, CSAA_Quality };
      CSAAMode csaaMode = CSAA_Normal;

      bool foundValid = false;
      size_t origNumSamples = multisampleCount;
      while (!foundValid)
      {
        // Deal with special cases
        if (tryCSAA)
        {
          switch (multisampleCount)
          {
          case 8:
            if (csaaMode == CSAA_Quality)
            {
              outputSampleDesc->Count = 8;
              outputSampleDesc->Quality = 8;
            }
            else
            {
              outputSampleDesc->Count = 4;
              outputSampleDesc->Quality = 8;
            }
            break;
          case 16:
            if (csaaMode == CSAA_Quality)
            {
              outputSampleDesc->Count = 8;
              outputSampleDesc->Quality = 16;
            }
            else
            {
              outputSampleDesc->Count = 4;
              outputSampleDesc->Quality = 16;
            }
            break;
          }
        }
        else // !CSAA
        {
          outputSampleDesc->Count = multisampleCount == 0 ? 1 : multisampleCount;
          outputSampleDesc->Quality = D3D11_STANDARD_MULTISAMPLE_PATTERN;
        }


        HRESULT hr;
        uint outQuality;
        hr = mDevice->getD3D11Device()->CheckMultisampleQualityLevels(format, outputSampleDesc->Count, &outQuality);

        if (SUCCEEDED(hr) && (!tryCSAA || outQuality > outputSampleDesc->Quality))
        {
          foundValid = true;
        }
        else
        {
          // Downgrade
          if (tryCSAA && multisampleCount == 8)
          {
            // For CSAA, we'll try downgrading with quality mode at all samples.
            // then try without quality, then drop CSAA
            if (csaaMode == CSAA_Quality)
            {
              // Drop quality first
              csaaMode = CSAA_Normal;
            }
            else
            {
              // Drop CSAA entirely 
              tryCSAA = false;
            }

            // Return to original requested samples
            multisampleCount = static_cast<uint32>(origNumSamples);
          }
          else
          {
            // Drop samples
            multisampleCount--;

            if (multisampleCount == 1)
            {
              // Ran out of options, no multisampling
              multisampleCount = 0;
              foundValid = true;
            }
          }
        }
      }
    }

    void D3D11RenderAPI::convertProjectionMatrix(const Matrix4& matrix, Matrix4& dest)
    {
      dest = matrix;

      // Convert depth range from [-1,+1] to [0,1]
      dest[2][0] = (dest[2][0] + dest[3][0]) / 2;
      dest[2][1] = (dest[2][1] + dest[3][1]) / 2;
      dest[2][2] = (dest[2][2] + dest[3][2]) / 2;
      dest[2][3] = (dest[2][3] + dest[3][3]) / 2;
    }

    const RenderAPIInfo& D3D11RenderAPI::getAPIInfo() const
    {
      RenderAPIFeatures featureFlags =
        RenderAPIFeatureFlag::TextureViews |
        RenderAPIFeatureFlag::Compute |
        RenderAPIFeatureFlag::LoadStore |
        RenderAPIFeatureFlag::ByteCodeCaching |
        RenderAPIFeatureFlag::RenderTargetLayers;

      static RenderAPIInfo info(0.0f, 0.0f, 0.0f, 1.0f, VET_COLOR_ABGR, featureFlags);

      return info;
    }

    GpuParamBlockDesc D3D11RenderAPI::generateParamBlockDesc(const String& name, Vector<GpuParamDataDesc>& params)
    {
      GpuParamBlockDesc block;
      block.blockSize = 0;
      block.isShareable = true;
      block.name = name;
      block.slot = 0;
      block.set = 0;

      for (auto& param : params)
      {
        const GpuParamDataTypeInfo& typeInfo = bs::GpuParams::PARAM_SIZES.lookup[param.type];

        if (param.arraySize > 1)
        {
          // Arrays perform no packing and their elements are always padded and aligned to four component vectors
          uint32 size;
          if (param.type == GPDT_STRUCT)
            size = Math::divideAndRoundUp(param.elementSize, 16U) * 4;
          else
            size = Math::divideAndRoundUp(typeInfo.size, 16U) * 4;

          block.blockSize = Math::divideAndRoundUp(block.blockSize, 4U) * 4;

          param.elementSize = size;
          param.arrayElementStride = size;
          param.cpuMemOffset = block.blockSize;
          param.gpuMemOffset = 0;

          // Last array element isn't rounded up to four component vectors unless it's a struct
          if (param.type != GPDT_STRUCT)
          {
            block.blockSize += size * (param.arraySize - 1);
            block.blockSize += typeInfo.size / 4;
          }
          else
            block.blockSize += param.arraySize * size;
        }
        else
        {
          uint32 size;
          if (param.type == GPDT_STRUCT)
          {
            // Structs are always aligned and arounded up to 4 component vectors
            size = Math::divideAndRoundUp(param.elementSize, 16U) * 4;
            block.blockSize = Math::divideAndRoundUp(block.blockSize, 4U) * 4;
          }
          else
          {
            size = typeInfo.baseTypeSize * (typeInfo.numRows * typeInfo.numColumns) / 4;

            // Pack everything as tightly as possible as long as the data doesn't cross 16 byte boundary
            uint32 alignOffset = block.blockSize % 4;
            if (alignOffset != 0 && size > (4 - alignOffset))
            {
              uint32 padding = (4 - alignOffset);
              block.blockSize += padding;
            }
          }

          param.elementSize = size;
          param.arrayElementStride = size;
          param.cpuMemOffset = block.blockSize;
          param.gpuMemOffset = 0;

          block.blockSize += size;
        }

        param.paramBlockSlot = 0;
        param.paramBlockSet = 0;
      }

      // Constant buffer size must always be a multiple of 16
      if (block.blockSize % 4 != 0)
        block.blockSize += (4 - (block.blockSize % 4));

      return block;
    }

    /************************************************************************/
    /* 								PRIVATE		                     		*/
    /************************************************************************/

    void D3D11RenderAPI::applyInputLayout()
    {
      if (mActiveVertexDeclaration == nullptr)
      {
        LOGWRN("Cannot apply input layout without a vertex declaration. Set vertex declaration before calling this method.");
        return;
      }

      if (mActiveVertexShader == nullptr)
      {
        LOGWRN("Cannot apply input layout without a vertex shader. Set vertex shader before calling this method.");
        return;
      }

      ID3D11InputLayout* ia = mIAManager->retrieveInputLayout(mActiveVertexShader->getInputDeclaration(), mActiveVertexDeclaration, *mActiveVertexShader);

      mDevice->getImmediateContext()->IASetInputLayout(ia);
    }
  }
}
