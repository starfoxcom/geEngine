/*****************************************************************************/
/**
 * @file    geD3D11RenderUtility.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2018/06/20
 * @brief   Helper class for DX11 rendering.
 *
 * Helper class for DX11 rendering.
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

#include <geModule.h>

namespace geEngineSDK {
  namespace geCoreThread {
    class D3D11RenderUtility final : public Module<D3D11RenderUtility>
    {
     public:
      D3D11RenderUtility(D3D11Device* device);
      ~D3D11RenderUtility();

      /**
       * @brief Draws a quad that clears the current viewport. This is supposed
       *        to emulate functionality available with other APIs like DX9 and
       *        OpenGL where you can clear only a part of the render target.
       *        (DX11 API only provides a way to clear the entire render
       *        target).
       * @param[in] clearBuffers  Combination of one or more elements of
       *                          FrameBufferType denoting which buffers are to
       *                          be cleared.
       * @param[in] color         (optional) The color to clear the color
       *                          buffer with, if enabled.
       * @param[in] depth         (optional) The value to initialize the depth
       *                          buffer with, if enabled.
       * @param[in] stencil       (optional) The value to initialize the
       *                          stencil buffer with, if enabled.
       */
      void
      drawClearQuad(uint32 clearBuffers,
                    const LinearColor& color,
                    float depth,
                    uint16 stencil);

     protected:
      /**
       * @brief Initializes resources needed for drawing the clear quad.
       *        Should be called one time at start-up.
       */
      void
      initClearQuadResources();

      D3D11Device* m_device;

      ID3D11Buffer* m_clearQuadIB;
      ID3D11Buffer* m_clearQuadVB;
      ID3D11InputLayout* m_clearQuadIL;
      ID3D11VertexShader* m_clearQuadVS;
      ID3D11PixelShader* m_clearQuadPS;

      SPtr<BlendState> m_clearQuadBlendStateYesC;
      SPtr<BlendState> m_clearQuadBlendStateNoC;
      SPtr<RasterizerState> m_clearQuadRasterizerState;
      SPtr<DepthStencilState> m_clearQuadDSStateNoD_NoS;
      SPtr<DepthStencilState> m_clearQuadDSStateYesD_NoS;
      SPtr<DepthStencilState> m_clearQuadDSStateYesD_YesS;
      SPtr<DepthStencilState> m_clearQuadDSStateNoD_YesS;
    };
  }
}
