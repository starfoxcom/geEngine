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

#include "geD3D11QueryManager.h"
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
                             requestedLevels.size(),
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
      //geEngineSDK::TextureManager::startUp<geEngineSDK::D3D11TextureManager>();
      //TextureManager::startUp<D3D11TextureManager>();

      //Create hardware buffer manager
      //geEngineSDK::HardwareBufferManager::startUp();
      //HardwareBufferManager::startUp<D3D11HardwareBufferManager>(ref(*mDevice));

      //Create render window manager
      //geEngineSDK::RenderWindowManager::startUp<geEngineSDK::D3D11RenderWindowManager>(this);
      //RenderWindowManager::startUp();

      //Create & register HLSL factory
      //mHLSLFactory = ge_new<D3D11HLSLProgramFactory>();

      //Create render state manager
      //RenderStateManager::startUp<D3D11RenderStateManager>();

      m_numDevices = 1;
      //m_currentCapabilities = ge_newN<RenderAPICapabilities>(m_numDevices);
      //initCapabilites(selectedAdapter, m_currentCapabilities[0]);

      //GPUProgramManager::instance().addFactory("hlsl", mHLSLFactory);

      //mIAManager = ge_new<D3D11InputLayoutManager>();

      RenderAPI::initialize();
    }

    void
    D3D11RenderAPI::initializeWithWindow(const SPtr<RenderWindow>& primaryWindow) {
      //D3D11RenderUtility::startUp(mDevice);
      //QueryManager::startUp<D3D11QueryManager>();
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
  }
}
