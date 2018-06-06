/*****************************************************************************/
/**
 * @file    geD3D11BlendState.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2018/06/02
 * @brief   DirectX 11 implementation of a blend state.
 *
 * DirectX 11 implementation of a blend state.
 * Corresponds directly with a DX11 blend state object.
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

#include <geBlendState.h>

namespace geEngineSDK {
  namespace geCoreThread {
    class D3D11BlendState : public BlendState
    {
     public:
      ~D3D11BlendState();

      /**
       * @brief Returns the internal DX11 blend state object.
       */
      ID3D11BlendState*
      getInternal() const {
        return m_blendState;
      }

     protected:
      friend class D3D11RenderStateManager;

      D3D11BlendState(const BLEND_STATE_DESC& desc, uint32 id);

      /**
       * @copydoc BlendState::createInternal
       */
      void
      createInternal() override;

      ID3D11BlendState* m_blendState;
    };
  }
}
