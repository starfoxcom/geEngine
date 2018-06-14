/*****************************************************************************/
/**
 * @file    geD3D11Mappings.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2018/06/06
 * @brief   Helper class that maps engine types to DirectX 11 types.
 *
 * Helper class that maps engine types to DirectX 11 types.
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

#include <geTexture.h>
#include <gePixelData.h>
#include <geIndexBuffer.h>
#include <geVertexData.h>
#include <geSamplerState.h>

namespace geEngineSDK {
  namespace geCoreThread {
    class D3D11Mappings
    {
     public:
      /**
       * @brief Converts engine to DX11 specific texture addressing mode.
       */
      static D3D11_TEXTURE_ADDRESS_MODE
      get(TEXTURE_ADDRESSING_MODE::E tam);

      /**
       * @brief Converts engine to DX11 specific blend factor.
       */
      static D3D11_BLEND
      get(BLEND_FACTOR::E bf);

      /**
       * @brief Converts engine to DX11 specific blend operation.
       */
      static D3D11_BLEND_OP
      get(BLEND_OPERATION::E bo);

      /**
       * @brief Converts engine to DX11 specific comparison function.
       */
      static D3D11_COMPARISON_FUNC
      get(COMPARE_FUNCTION::E cf);

      /**
       * @brief Converts engine to DX11 specific culling mode.
       */
      static D3D11_CULL_MODE
      get(CULLING_MODE::E cm);

      /**
       * @brief Converts engine to DX11 specific polygon fill mode.
       */
      static D3D11_FILL_MODE
      get(POLYGON_MODE::E mode);

      /**
       * @brief Return DirectX 11 stencil operation and optionally invert it
       *        (greater than becomes less than, etc.).
       */
      static D3D11_STENCIL_OP
      get(STENCIL_OPERATION::E op, bool invert = false);

      /**
       * @brief Converts engine texture filter type to DirectX 11 filter shift
       *        (used for combining to get actual min/mag/mip filter bit
       *        location).
       */
      static DWORD
      get(FILTER_TYPE::E ft);

      /**
       * @brief Returns DirectX 11 texture filter from the provided min, mag
       *        and mip filter options, and optionally a filter with comparison
       *        support.
       */
      static D3D11_FILTER
      get(const FILTER_OPTIONS::E min,
          const FILTER_OPTIONS::E mag,
          const FILTER_OPTIONS::E mip,
          const bool comparison = false);

      /**
       * @brief Converts engine to DX11 buffer usage.
       */
      static DWORD
      get(GPU_BUFFER_USAGE::E usage);

      /**
       * @brief Converts engine to DX11 lock options, while also constraining
       *        the options depending on provided usage type.
       */
      static D3D11_MAP
      get(GPU_LOCK_OPTIONS::E options, GPU_BUFFER_USAGE::E usage);

      /**
       * @brief Converts engine to DX11 vertex element type.
       */
      static DXGI_FORMAT
      get(VERTEX_ELEMENT_TYPE::E type);

      /**
       * @brief Returns a string describing the provided vertex element
       *        semantic.
       */
      static LPCSTR
      get(VERTEX_ELEMENT_SEMANTIC::E sem);

      /**
       * @brief Returns engine semantic from the provided semantic string.
       *        Throws an exception for semantics that do not exist.
       */
      static VERTEX_ELEMENT_SEMANTIC::E
      get(LPCSTR sem);

      /**
       * @brief Converts DirectX 11 GPU parameter component type to engine
       *        vertex element type.
       */
      static VERTEX_ELEMENT_TYPE::E
      getInputType(D3D_REGISTER_COMPONENT_TYPE type);

      /**
       * @brief Returns DX11 primitive topology based on the provided draw
       *        operation type.
       */
      static D3D11_PRIMITIVE_TOPOLOGY
      getPrimitiveType(DrawOperationType type);

      /**
       * @brief Converts engine color to DX11 color.
       */
      static void
      get(const LinearColor& inColor, float* outColor);

      /**
       * @brief Checks does the provided map value include writing.
       */
      static bool
      isMappingWrite(D3D11_MAP map);

      /**
       * @brief Checks does the provided map value include reading.
       */
      static bool
      isMappingRead(D3D11_MAP map);

      /**
       * @brief Converts DX11 pixel format to engine pixel format.
       */
      static PixelFormat
      getPF(DXGI_FORMAT d3dPF);

      /**
       * @brief Converts engine pixel format to DX11 pixel format.
       *        Some formats depend on whether hardware gamma is used or not,
       *        in which case set the @p hwGamma parameter as needed.
       */
      static DXGI_FORMAT
      getPF(PixelFormat format, bool hwGamma);

      /**
       * @brief Converts engine GPU buffer format to DX11 GPU buffer format.
       */
      static DXGI_FORMAT
      getBF(GPU_BUFFER_FORMAT::E format);

      /**
       * @brief Returns a typeless version of a depth stencil format. Required
       *        for creating a depth stencil texture it can be bound both for
       *        shader reading and depth/stencil writing.
       */
      static DXGI_FORMAT
      getTypelessDepthStencilPF(PixelFormat format);

      /**
       * @brief Returns a format of a depth stencil texture that can be used
       *        for reading the texture in the shader.
       */
      static DXGI_FORMAT
      getShaderResourceDepthStencilPF(PixelFormat format);

      /**
       * @brief Converts engine to DX11 buffer usage.
       */
      static D3D11_USAGE
      getUsage(GPU_BUFFER_USAGE::E usage);

      /**
       * @brief Converts engine to DX11 buffer access flags.
       */
      static UINT
      getAccessFlags(GPU_BUFFER_USAGE::E usage);

      /**
       * @brief Converts engine to DX11 lock options.
       */
      static D3D11_MAP
      getLockOptions(GPU_LOCK_OPTIONS::E lockOptions);

      /**
       * @brief Checks is the provided buffer usage dynamic.
       */
      static bool
      isDynamic(GPU_BUFFER_USAGE::E usage);

      /**
       * @brief Finds the closest pixel format that DX11 supports.
       */
      static PixelFormat
      getClosestSupportedPF(PixelFormat format,
                            TextureType texType,
                            int32 usage);

      /**
       * @brief Returns size in bytes of a pixel surface of the specified size
       *        and format, while using DX11 allocation rules for padding.
       */
      static uint32
      getSizeInBytes(PixelFormat pf, uint32 width = 1, uint32 height = 1);
    };
  }
}
