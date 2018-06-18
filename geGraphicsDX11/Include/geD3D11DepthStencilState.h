/*****************************************************************************/
/**
 * @file    geD3D11DepthStencilState.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2018/06/17
 * @brief   DirectX 11 implementation of a depth stencil state.
 *
 * DirectX 11 implementation of a depth stencil state.
 * Corresponds directly with a DX11 depth stencil state object.
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

#include <geDepthStencilState.h>

namespace geEngineSDK {
  namespace geCoreThread {
    class D3D11DepthStencilState : public DepthStencilState
    {
     public:
      ~D3D11DepthStencilState();

      /**
       * @brief Returns the internal DX11 depth stencil state object.
       */
      ID3D11DepthStencilState*
      getInternal() const {
        return m_depthStencilState;
      }

     protected:
      friend class D3D11RenderStateManager;

      D3D11DepthStencilState(const DEPTH_STENCIL_STATE_DESC& desc, uint32 id);

      /**
       * @copydoc DepthStencilState::createInternal()
       */
      void
      createInternal() override;

      ID3D11DepthStencilState* m_depthStencilState;
    };
  }
}
