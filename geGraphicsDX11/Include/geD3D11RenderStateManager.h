/*****************************************************************************/
/**
 * @file    geD3D11RenderStateManager.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2018/06/17
 * @brief   Handles creation of DirectX 11 pipeline states.
 *
 * Handles creation of DirectX 11 pipeline states.
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

#include <geRenderStateManager.h>

namespace geEngineSDK {
  namespace geCoreThread {
    class D3D11RenderStateManager : public RenderStateManager
    {
     protected:
      /**
       * @copydoc RenderStateManager::createSamplerStateInternal
       */
      SPtr<SamplerState>
      createSamplerStateInternal(const SAMPLER_STATE_DESC& desc,
                                 GPU_DEVICE_FLAGS::E deviceMask) const override;

      /**
       * @copydoc RenderStateManager::createBlendStateInternal
       */
      SPtr<BlendState>
      createBlendStateInternal(const BLEND_STATE_DESC& desc,
                               uint32 id) const override;

      /**
       * @copydoc RenderStateManager::createRasterizerStateInternal
       */
      SPtr<RasterizerState>
      createRasterizerStateInternal(const RASTERIZER_STATE_DESC& desc,
                                    uint32 id) const override;

      /**
       * @copydoc RenderStateManager::createDepthStencilStateInternal
       */
      SPtr<DepthStencilState>
      createDepthStencilStateInternal(const DEPTH_STENCIL_STATE_DESC& desc,
                                      uint32 id) const override;
    };
  }
}
