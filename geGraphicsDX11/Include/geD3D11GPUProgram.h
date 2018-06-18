/*****************************************************************************/
/**
 * @file    geD3D11GPUProgram.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2018/06/17
 * @brief   Abstraction of a DirectX 11 shader object.
 *
 * Abstraction of a DirectX 11 shader object.
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

#include <geGPUProgram.h>

namespace geEngineSDK {
  namespace geCoreThread {
    class D3D11GPUProgram : public GPUProgram
    {
     public:
      virtual ~D3D11GPUProgram();

      /**
       * @brief Returns compiled shader microcode.
       */
      const DataBlob&
      getMicroCode() const {
        return m_bytecode->instructions;
      }

      /**
       * @brief Returns unique GPU program ID.
       */
      uint32
      getProgramId() const {
        return m_programId;
      }

     protected:
      D3D11GPUProgram(const GPU_PROGRAM_DESC& desc,
                      GPU_DEVICE_FLAGS::E deviceMask);

      /**
       * @copydoc GPUProgram::initialize
       */
      void
      initialize() override;

      /**
       * @brief Loads the shader from microcode.
       */
      virtual void
      loadFromMicrocode(D3D11Device& device, const DataBlob& microcode) = 0;

     protected:
      static uint32 s_globalProgramId;

      uint32 m_programId;
    };

    /**
     * @brief Implementation of a DX11 vertex shader.
     */
    class D3D11GPUVertexProgram final : public D3D11GPUProgram
    {
     public:
      ~D3D11GPUVertexProgram();

      /**
       * @brief Returns internal DX11 vertex shader object.
       */
      ID3D11VertexShader*
      getVertexShader() const;

     protected:
      friend class D3D11HLSLProgramFactory;

      D3D11GPUVertexProgram(const GPU_PROGRAM_DESC& desc,
                            GPU_DEVICE_FLAGS::E deviceMask);

      /**
       * @copydoc D3D11GPUProgram::loadFromMicrocode
       */
      void
      loadFromMicrocode(D3D11Device& device,
                        const DataBlob& microcode) override;

     protected:
      ID3D11VertexShader* m_vertexShader;
    };

    /**
     * @brief Implementation of a DX11 pixel shader.
     */
    class D3D11GPUFragmentProgram final : public D3D11GPUProgram
    {
     public:
      ~D3D11GPUFragmentProgram();

      /**
       * @brief Returns internal DX11 pixel shader object.
       */
      ID3D11PixelShader*
      getPixelShader() const;

     protected:
      friend class D3D11HLSLProgramFactory;

      D3D11GPUFragmentProgram(const GPU_PROGRAM_DESC& desc,
                              GPU_DEVICE_FLAGS::E deviceMask);

      /**
       * @copydoc D3D11GPUProgram::loadFromMicrocode
       */
      void
      loadFromMicrocode(D3D11Device& device,
                        const DataBlob& microcode) override;

     protected:
      ID3D11PixelShader* m_pixelShader;
    };

    /**
     * @brief Implementation of a DX11 domain shader.
     */
    class D3D11GPUDomainProgram : public D3D11GPUProgram
    {
     public:
      ~D3D11GPUDomainProgram();

      /**
       * @brief Returns internal DX11 domain shader object.
       */
      ID3D11DomainShader*
      getDomainShader() const;

     protected:
      friend class D3D11HLSLProgramFactory;

      D3D11GPUDomainProgram(const GPU_PROGRAM_DESC& desc,
                            GPU_DEVICE_FLAGS::E deviceMask);

      /**
       * @copydoc D3D11GPUProgram::loadFromMicrocode
       */
      void
      loadFromMicrocode(D3D11Device& device,
                       const DataBlob& microcode) override;

     protected:
      ID3D11DomainShader* m_domainShader;
    };

    /**
     * @brief Implementation of a DX11 hull shader.
     */
    class D3D11GPUHullProgram final : public D3D11GPUProgram
    {
     public:
      ~D3D11GPUHullProgram();

      /**
       * @brief Returns internal DX11 hull shader object.
       */
      ID3D11HullShader*
      getHullShader() const;

     protected:
      friend class D3D11HLSLProgramFactory;

      D3D11GPUHullProgram(const GPU_PROGRAM_DESC& desc,
                          GPU_DEVICE_FLAGS::E deviceMask);

      /**
       * @copydoc D3D11GPUProgram::loadFromMicrocode
       */
      void
      loadFromMicrocode(D3D11Device& device,
                        const DataBlob& microcode) override;

     protected:
      ID3D11HullShader* m_hullShader;
    };

    /**
     * @brief Implementation of a DX11 geometry shader.
     */
    class D3D11GPUGeometryProgram final : public D3D11GPUProgram
    {
     public:
      ~D3D11GPUGeometryProgram();

      /**
       * @brief Returns internal DX11 geometry shader object.
       */
      ID3D11GeometryShader*
      getGeometryShader() const;

     protected:
      friend class D3D11HLSLProgramFactory;

      D3D11GPUGeometryProgram(const GPU_PROGRAM_DESC& desc,
                              GPU_DEVICE_FLAGS::E deviceMask);

      /**
       * @copydoc D3D11GPUProgram::loadFromMicrocode
       */
      void
      loadFromMicrocode(D3D11Device& device,
                        const DataBlob& microcode) override;

     protected:
      ID3D11GeometryShader* m_geometryShader;
    };

    /**
     * @brief Implementation of a DX11 compute shader.
     */
    class D3D11GPUComputeProgram final : public D3D11GPUProgram
    {
     public:
      ~D3D11GPUComputeProgram();

      /**
       * @brief Returns internal DX11 compute shader object.
       */
      ID3D11ComputeShader*
      getComputeShader() const;

     protected:
      friend class D3D11HLSLProgramFactory;

      D3D11GPUComputeProgram(const GPU_PROGRAM_DESC& desc,
                             GPU_DEVICE_FLAGS::E deviceMask);

      /**
       * @copydoc D3D11GPUProgram::loadFromMicrocode
       */
      void
      loadFromMicrocode(D3D11Device& device,
                        const DataBlob& microcode) override;

     protected:
      ID3D11ComputeShader* m_computeShader;
    };

    /**
     * Identifier of the compiler used for compiling DirectX 11 GPU programs.
     */
    static constexpr const ANSICHAR* DIRECTX_COMPILER_ID = "DirectX11";
  }
}
