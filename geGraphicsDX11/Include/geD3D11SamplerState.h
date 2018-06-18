/*****************************************************************************/
/**
 * @file    geD3D11SamplerState.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2018/06/17
 * @brief   DirectX 11 implementation of a sampler state.
 *
 * DirectX 11 implementation of a sampler state.
 * Wraps a DX11 sampler state object.
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

#include <geSamplerState.h>

namespace geEngineSDK {
  namespace geCoreThread {
    class D3D11SamplerState : public SamplerState
    {
     public:
      ~D3D11SamplerState();

      ID3D11SamplerState*
      getInternal() const {
        return m_samplerState;
      }

     protected:
      friend class D3D11RenderStateManager;

      D3D11SamplerState(const SAMPLER_STATE_DESC& desc,
                        GPU_DEVICE_FLAGS::E deviceMask);

      /**
       * @copydoc SamplerState::createInternal
       */
      void
      createInternal() override;

      ID3D11SamplerState* m_samplerState;
    };
  }
}
