/*****************************************************************************/
/**
 * @file    geD3D11RenderUtility.cpp
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2018/06/20
 * @brief   Helper class for DX11 rendering.
 *
 * Helper class for DX11 rendering.
 *
 * @bug     No known bugs.
 */
/*****************************************************************************/

/*****************************************************************************/
/**
 * Includes
 */
/*****************************************************************************/
#include "geD3D11RenderUtility.h"
#include "geD3D11Device.h"
#include "geD3D11BlendState.h"
#include "geD3D11RasterizerState.h"
#include "geD3D11DepthStencilState.h"

#include <geRenderStateManager.h>
#include <geVector3.h>
#include <geColor.h>
#include <geBox2DI.h>
#include <geNumericLimits.h>

namespace geEngineSDK {
  namespace geCoreThread {
    struct ClearVertex
    {
      Vector3 pos;
      uint32 col;
    };

    D3D11RenderUtility::D3D11RenderUtility(D3D11Device* device)
      : m_device(device),
        m_clearQuadIB(nullptr),
        m_clearQuadVB(nullptr),
        m_clearQuadIL(nullptr),
        m_clearQuadVS(nullptr),
        m_clearQuadPS(nullptr) {
      initClearQuadResources();
    }

    D3D11RenderUtility::~D3D11RenderUtility() {
      SAFE_RELEASE(m_clearQuadPS);
      SAFE_RELEASE(m_clearQuadVS);
      SAFE_RELEASE(m_clearQuadIL);
      SAFE_RELEASE(m_clearQuadIB);
      SAFE_RELEASE(m_clearQuadVB);
    }

    void
    D3D11RenderUtility::drawClearQuad(uint32 clearBuffers,
                                      const LinearColor& color,
                                      float depth,
                                      uint16 stencil) {
      ID3D11DeviceContext* pImmContext = m_device->getImmediateContext();

      //Set states
      if ((clearBuffers & FRAME_BUFFER_TYPE::kCOLOR) != 0) {
        auto d3d11BlendState = static_cast<D3D11BlendState*>
          (const_cast<BlendState*>(m_clearQuadBlendStateYesC.get()));

        pImmContext->OMSetBlendState(d3d11BlendState->getInternal(),
                                     nullptr,
                                     NumLimit::MAX_UINT32);
      }
      else {
        auto d3d11BlendState = static_cast<D3D11BlendState*>
          (const_cast<BlendState*>(m_clearQuadBlendStateNoC.get()));

        pImmContext->OMSetBlendState(d3d11BlendState->getInternal(),
                                     nullptr,
                                     NumLimit::MAX_UINT32);
      }

      auto d3d11RasterizerState = static_cast<D3D11RasterizerState*>
        (const_cast<RasterizerState*>(m_clearQuadRasterizerState.get()));

      pImmContext->RSSetState(d3d11RasterizerState->getInternal());

      if ((clearBuffers & FRAME_BUFFER_TYPE::kDEPTH) != 0) {
        if ((clearBuffers & FRAME_BUFFER_TYPE::kSTENCIL) != 0) {
          auto d3d11DepthStencilState = static_cast<D3D11DepthStencilState*>
            (const_cast<DepthStencilState*>(m_clearQuadDSStateYesD_YesS.get()));

          pImmContext->OMSetDepthStencilState(d3d11DepthStencilState->getInternal(),
                                              stencil);
        }
        else {
          auto d3d11DepthStencilState = static_cast<D3D11DepthStencilState*>
            (const_cast<DepthStencilState*>(m_clearQuadDSStateYesD_NoS.get()));

          pImmContext->OMSetDepthStencilState(d3d11DepthStencilState->getInternal(),
                                              stencil);
        }
      }
      else {
        if ((clearBuffers & FRAME_BUFFER_TYPE::kSTENCIL) != 0) {
          auto d3d11DepthStencilState = static_cast<D3D11DepthStencilState*>
            (const_cast<DepthStencilState*>(m_clearQuadDSStateNoD_YesS.get()));
          pImmContext->OMSetDepthStencilState(d3d11DepthStencilState->getInternal(),
                                              stencil);
        }
        else {
          auto d3d11DepthStencilState = static_cast<D3D11DepthStencilState*>
            (const_cast<DepthStencilState*>(m_clearQuadDSStateNoD_NoS.get()));
          pImmContext->OMSetDepthStencilState(d3d11DepthStencilState->getInternal(),
                                              stencil);
        }
      }

      //TODO: How smart it is to update buffer right before drawing it!?
      //(cache the clip area)
      ClearVertex vertexData[4];
      vertexData[0].pos = Vector3(-1.0f, 1.0f, depth);
      vertexData[1].pos = Vector3(1.0f, 1.0f, depth);
      vertexData[2].pos = Vector3(-1.0f, -1.0f, depth);
      vertexData[3].pos = Vector3(1.0f, -1.0f, depth);

      RGBA rgbaColor = color.toColor(false).toPackedRGBA();
      vertexData[0].col = rgbaColor;
      vertexData[1].col = rgbaColor;
      vertexData[2].col = rgbaColor;
      vertexData[3].col = rgbaColor;

      pImmContext->UpdateSubresource(m_clearQuadVB,
                                     0,
                                     nullptr,
                                     vertexData,
                                     0,
                                     sizeof(ClearVertex) * 4);

      pImmContext->VSSetShader(m_clearQuadVS, nullptr, 0);
      pImmContext->PSSetShader(m_clearQuadPS, nullptr, 0);

      ID3D11Buffer* buffers[1];
      buffers[0] = m_clearQuadVB;

      uint32 strides[1] = { sizeof(ClearVertex) };
      uint32 offsets[1] = { 0 };

      pImmContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
      pImmContext->IASetIndexBuffer(m_clearQuadIB, DXGI_FORMAT_R16_UINT, 0);
      pImmContext->IASetVertexBuffers(0, 1, buffers, strides, offsets);
      pImmContext->IASetInputLayout(m_clearQuadIL);
      pImmContext->DrawIndexed(6, 0, 0);
    }

    void
    D3D11RenderUtility::initClearQuadResources() {
      BLEND_STATE_DESC blendStateDescYesC;
      m_clearQuadBlendStateYesC = RenderStateManager::instance().
                                    createBlendState(blendStateDescYesC);

      BLEND_STATE_DESC blendStateDescNoC;
      for (auto& renderTargetDesc : blendStateDescNoC.renderTargetDesc) {
        renderTargetDesc.renderTargetWriteMask = 0;
      }

      m_clearQuadBlendStateNoC = RenderStateManager::instance().
                                   createBlendState(blendStateDescNoC);

      DEPTH_STENCIL_STATE_DESC depthStateDescNoD_NoS;
      depthStateDescNoD_NoS.depthReadEnable = false;
      depthStateDescNoD_NoS.depthWriteEnable = false;
      depthStateDescNoD_NoS.depthComparisonFunc = COMPARE_FUNCTION::kALWAYS_PASS;
      m_clearQuadDSStateNoD_NoS = RenderStateManager::instance().
                                    createDepthStencilState(depthStateDescNoD_NoS);

      DEPTH_STENCIL_STATE_DESC depthStateDescYesD_NoS;
      depthStateDescYesD_NoS.depthReadEnable = false;
      depthStateDescYesD_NoS.depthWriteEnable = true;
      depthStateDescYesD_NoS.depthComparisonFunc = COMPARE_FUNCTION::kALWAYS_PASS;
      m_clearQuadDSStateYesD_NoS = RenderStateManager::instance().
                                     createDepthStencilState(depthStateDescYesD_NoS);

      DEPTH_STENCIL_STATE_DESC depthStateDescYesD_YesS;
      depthStateDescYesD_YesS.depthReadEnable = false;
      depthStateDescYesD_YesS.depthWriteEnable = true;
      depthStateDescYesD_YesS.depthComparisonFunc = COMPARE_FUNCTION::kALWAYS_PASS;
      depthStateDescYesD_YesS.stencilEnable = true;
      depthStateDescYesD_YesS.frontStencilComparisonFunc = COMPARE_FUNCTION::kALWAYS_PASS;
      depthStateDescYesD_YesS.frontStencilPassOp = STENCIL_OPERATION::kREPLACE;
      m_clearQuadDSStateYesD_YesS = RenderStateManager::instance().
                                      createDepthStencilState(depthStateDescYesD_YesS);

      DEPTH_STENCIL_STATE_DESC depthStateDescNoD_YesS;
      depthStateDescNoD_YesS.depthReadEnable = false;
      depthStateDescNoD_YesS.depthWriteEnable = false;
      depthStateDescNoD_YesS.depthComparisonFunc = COMPARE_FUNCTION::kALWAYS_PASS;
      depthStateDescNoD_YesS.stencilEnable = true;
      depthStateDescNoD_YesS.frontStencilComparisonFunc = COMPARE_FUNCTION::kALWAYS_PASS;
      m_clearQuadDSStateNoD_YesS = RenderStateManager::instance().
                                     createDepthStencilState(depthStateDescNoD_YesS);

      RASTERIZER_STATE_DESC rasterizerStateDesc;
      m_clearQuadRasterizerState = RenderStateManager::instance().
                                     createRasterizerState(rasterizerStateDesc);

      String vsShaderCode = R"(
               void
               main(in float3 inPos : POSITION,
                    in float4 color : COLOR0,
                    out float4 oPosition : SV_Position,
                    out float4 oColor : COLOR0) {
                 oPosition = float4(inPos.xyz, 1);
                 oColor = color;
						   })";

      String psShaderCode = R"(
               float4
               main(in float4 inPos : SV_Position,
                    float4 color : COLOR0) : SV_Target	{
                      return color;
               })";

      HRESULT hr;
      ID3DBlob* microcode = nullptr;
      ID3DBlob* errors = nullptr;

      //Compile pixel shader
      hr = D3DCompile(psShaderCode.c_str(),
                      psShaderCode.size(),
                      nullptr,
                      nullptr,
                      nullptr,
                      "main",
                      "ps_5_0",
                      D3DCOMPILE_PACK_MATRIX_COLUMN_MAJOR,
                      0,
                      &microcode,
                      &errors);

      if (FAILED(hr)) {
        String message = "Cannot assemble D3D11 high-level shader. Errors:\n" +
          String(reinterpret_cast<const ANSICHAR*>(errors->GetBufferPointer()));

        SAFE_RELEASE(errors);
        GE_EXCEPT(RenderingAPIException, message);
      }

      SAFE_RELEASE(errors);

      auto device = m_device->getD3D11Device();
      hr = device->CreatePixelShader(reinterpret_cast<DWORD*>(microcode->GetBufferPointer()),
                                     microcode->GetBufferSize(),
                                     m_device->getClassLinkage(),
                                     &m_clearQuadPS);

      if (FAILED(hr)) {
        SAFE_RELEASE(microcode);
        GE_EXCEPT(RenderingAPIException,
                  "Cannot create D3D11 high-level shader.");
      }

      //Compile vertex shader
      hr = D3DCompile(vsShaderCode.c_str(),
                      vsShaderCode.size(),
                      nullptr,
                      nullptr,
                      nullptr,
                      "main",
                      "vs_5_0",
                      D3DCOMPILE_PACK_MATRIX_COLUMN_MAJOR,
                      0,
                      &microcode,
                      &errors);

      if (FAILED(hr)) {
        String message = "Cannot assemble D3D11 high-level shader. Errors:\n" +
          String(reinterpret_cast<const ANSICHAR*>(errors->GetBufferPointer()));

        SAFE_RELEASE(errors);
        GE_EXCEPT(RenderingAPIException, message);
      }

      SAFE_RELEASE(errors);

      hr = device->CreateVertexShader(reinterpret_cast<DWORD*>(microcode->GetBufferPointer()),
                                      microcode->GetBufferSize(),
                                      m_device->getClassLinkage(),
                                      &m_clearQuadVS);

      if (FAILED(hr)) {
        SAFE_RELEASE(microcode);
        GE_EXCEPT(RenderingAPIException,
                  "Cannot create D3D11 high-level shader.");
      }

      //Create input layout
      auto declElements = ge_newN<D3D11_INPUT_ELEMENT_DESC>(2);
      memset(declElements, 0, sizeof(D3D11_INPUT_ELEMENT_DESC) * 2);

      declElements[0].SemanticName = "POSITION";
      declElements[0].SemanticIndex = 0;
      declElements[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
      declElements[0].InputSlot = 0;
      declElements[0].AlignedByteOffset = 0;
      declElements[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
      declElements[0].InstanceDataStepRate = 0;

      declElements[1].SemanticName = "COLOR";
      declElements[1].SemanticIndex = 0;
      declElements[1].Format = DXGI_FORMAT_R8G8B8A8_UNORM;
      declElements[1].InputSlot = 0;
      declElements[1].AlignedByteOffset = sizeof(float) * 3;
      declElements[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
      declElements[1].InstanceDataStepRate = 0;

      hr = device->CreateInputLayout(declElements,
                                     2,
                                     microcode->GetBufferPointer(),
                                     microcode->GetBufferSize(),
                                     &m_clearQuadIL);

      ge_deleteN(declElements, 2);

      if (FAILED(hr)) {
        GE_EXCEPT(RenderingAPIException, "Cannot create D3D11 input layout.");
      }

      //Create vertex buffer
      D3D11_BUFFER_DESC mVBDesc;

      mVBDesc.ByteWidth = sizeof(ClearVertex) * 4;
      mVBDesc.MiscFlags = 0;
      mVBDesc.StructureByteStride = 0;

      mVBDesc.Usage = D3D11_USAGE_DEFAULT;
      mVBDesc.CPUAccessFlags = 0;
      mVBDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

      ClearVertex vertexData[4];
      vertexData[0].pos = Vector3(0, 0, 0);
      vertexData[1].pos = Vector3(0, 0, 0);
      vertexData[2].pos = Vector3(0, 0, 0);
      vertexData[3].pos = Vector3(0, 0, 0);

      vertexData[0].col = Color::White.toPackedBGRA();
      vertexData[1].col = Color::White.toPackedBGRA();
      vertexData[2].col = Color::White.toPackedBGRA();
      vertexData[3].col = Color::White.toPackedBGRA();

      D3D11_SUBRESOURCE_DATA vertexSubresourceData;
      vertexSubresourceData.pSysMem = vertexData;

      hr = device->CreateBuffer(&mVBDesc,
                                &vertexSubresourceData,
                                &m_clearQuadVB);
      if (FAILED(hr) || m_device->hasError()) {
        String msg = m_device->getErrorDescription();
        GE_EXCEPT(RenderingAPIException, "Cannot create D3D11 buffer: " + msg);
      }

      //Create index buffer
      D3D11_BUFFER_DESC mIBDesc;

      mIBDesc.ByteWidth = sizeof(uint16) * 6;
      mIBDesc.MiscFlags = 0;
      mIBDesc.StructureByteStride = 0;

      mIBDesc.Usage = D3D11_USAGE_DEFAULT;
      mIBDesc.CPUAccessFlags = 0;
      mIBDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

      uint16 indexData[6];
      indexData[0] = 0;
      indexData[1] = 1;
      indexData[2] = 2;
      indexData[3] = 1;
      indexData[4] = 3;
      indexData[5] = 2;

      D3D11_SUBRESOURCE_DATA indexSubresourceData;
      indexSubresourceData.pSysMem = indexData;

      hr = device->CreateBuffer(&mIBDesc,
                                &indexSubresourceData,
                                &m_clearQuadIB);

      if (FAILED(hr) || m_device->hasError()) {
        String msg = m_device->getErrorDescription();
        GE_EXCEPT(RenderingAPIException, "Cannot create D3D11 buffer: " + msg);
      }
    }
  }
}
