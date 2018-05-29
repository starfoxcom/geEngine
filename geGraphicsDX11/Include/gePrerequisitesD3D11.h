/*****************************************************************************/
/**
 * @file    gePrerequisitesD3D11.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2018/05/11
 * @brief   Wrapper around the DirectX 11 render API.
 *
 * Wrapper around the DirectX 11 render API.
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
#include <gePrerequisitesCore.h>

#define SAFE_RELEASE(p) { if(p) { (p)->Release(); (p)=nullptr; } }

#if GE_DEBUG_MODE
#	define D3D_DEBUG_INFO
#endif

#define WIN32_LEAN_AND_MEAN
#if !defined(NOMINMAX) && defined(_MSC_VER)
#	define NOMINMAX //Required to stop windows.h messing up std::min
#endif

#include <d3d11.h>
#include <d3dcompiler.h>
#include <d3d11shader.h>

namespace geEngineSDK {
  namespace geCoreThread {
    class D3D11RenderAPI;
    class D3D11Driver;
    class D3D11DriverList;
    class D3D11VideoMode;
    class D3D11VideoModeList;
    class D3D11HLSLProgramFactory;
    class D3D11Device;
    class D3D11HardwareBuffer;
    class D3D11DepthStencilState;
    class D3D11InputLayoutManager;
    class D3D11GPUBuffer;
    class D3D11RenderUtility;
    class D3D11GPUProgram;
    class D3D11TextureView;
    class D3D11RenderWindow;
    class GPUBufferView;

    /**
     * @brief DirectX 11 specific types to track resource statistics for.
     */
    namespace D3D11_RENDER_STAT_RESOURCE_TYPE {
      enum E {
        kDepthStencilState = 100,
        kRasterizerState,
        kBlendState,
        kSamplerState,
        kInputLayout,
        kResourceView,
        kSwapChain
      };
    }

    using HLSLMicroCode = Vector<int8*>;
  }
}
