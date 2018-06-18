/*****************************************************************************/
/**
 * @file    geD3D11RenderStateManager.cpp
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2018/06/17
 * @brief   Handles creation of DirectX 11 pipeline states.
 *
 * Handles creation of DirectX 11 pipeline states.
 *
 * @bug     No known bugs.
 */
/*****************************************************************************/

/*****************************************************************************/
/**
 * Includes
 */
/*****************************************************************************/
#include "geD3D11RenderStateManager.h"
#include "geD3D11SamplerState.h"
#include "geD3D11DepthStencilState.h"
#include "geD3D11RasterizerState.h"
#include "geD3D11BlendState.h"

namespace geEngineSDK {
  namespace geCoreThread {
    SPtr<SamplerState>
    D3D11RenderStateManager::createSamplerStateInternal(const SAMPLER_STATE_DESC& desc,
                                                       GPU_DEVICE_FLAGS::E deviceMask) const {
      SPtr<SamplerState> ret = ge_shared_ptr<D3D11SamplerState>
        (GE_PVT_NEW(D3D11SamplerState, desc, deviceMask));
      ret->_setThisPtr(ret);

      return ret;
    }

    SPtr<BlendState>
    D3D11RenderStateManager::createBlendStateInternal(const BLEND_STATE_DESC& desc,
                                                      uint32 id) const {
      SPtr<BlendState> ret = ge_shared_ptr<D3D11BlendState>
        (GE_PVT_NEW(D3D11BlendState, desc, id));
      ret->_setThisPtr(ret);

      return ret;
    }

    SPtr<RasterizerState>
    D3D11RenderStateManager::createRasterizerStateInternal(const RASTERIZER_STATE_DESC& desc,
                                                           uint32 id) const {
      SPtr<RasterizerState> ret = ge_shared_ptr<D3D11RasterizerState>
        (GE_PVT_NEW(D3D11RasterizerState, desc, id));
      ret->_setThisPtr(ret);

      return ret;
    }

    SPtr<DepthStencilState>
    D3D11RenderStateManager::createDepthStencilStateInternal(
                                                        const DEPTH_STENCIL_STATE_DESC& desc,
                                                        uint32 id) const {
      SPtr<DepthStencilState> ret = ge_shared_ptr<D3D11DepthStencilState>
        (GE_PVT_NEW(D3D11DepthStencilState, desc, id));
      ret->_setThisPtr(ret);

      return ret;
    }
  }
}
