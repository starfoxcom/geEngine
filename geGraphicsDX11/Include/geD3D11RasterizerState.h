/*****************************************************************************/
/**
 * @file    geD3D11RasterizerState.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2018/06/17
 * @brief   DirectX 11 implementation of a rasterizer state.
 *
 * DirectX 11 implementation of a rasterizer state.
 * Wraps a DX11 rasterizer state object.
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

#include "geRasterizerState.h"

namespace geEngineSDK {
  namespace geCoreThread {
    class D3D11RasterizerState : public RasterizerState
    {
     public:
      ~D3D11RasterizerState();

      ID3D11RasterizerState*
      getInternal() const {
        return m_rasterizerState;
      }

     protected:
      friend class D3D11RenderStateManager;

      D3D11RasterizerState(const RASTERIZER_STATE_DESC& desc, uint32 id);

      /**
       * @copydoc RasterizerState::createInternal
       */
      void
      createInternal() override;

      ID3D11RasterizerState* m_rasterizerState;
    };
  }
}
