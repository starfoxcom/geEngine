/*****************************************************************************/
/**
 * @file    geD3D11GPUProgram.cpp
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2018/06/17
 * @brief   Abstraction of a DirectX 11 shader object.
 *
 * Abstraction of a DirectX 11 shader object.
 *
 * @bug     No known bugs.
 */
/*****************************************************************************/

/*****************************************************************************/
/**
 * Includes
 */
/*****************************************************************************/
#include "geD3D11GPUProgram.h"
#include "geD3D11Device.h"
#include "geD3D11RenderAPI.h"
//#include "geD3D11HLSLParamParser.h"

#include <geGPUProgramManager.h>
#include <geHardwareBufferManager.h>
#include <geGPUParams.h>
#include <geRenderStats.h>
#include <geException.h>
#include <geDebug.h>

//#include <regex>

namespace geEngineSDK {
  namespace geCoreThread {
    uint32 D3D11GPUProgram::s_globalProgramId = 0;

    D3D11GPUProgram::D3D11GPUProgram(const GPU_PROGRAM_DESC& desc,
                                     GPU_DEVICE_FLAGS::E deviceMask)
      : GPUProgram(desc, deviceMask), m_programId(0)
    {
      GE_ASSERT((GPU_DEVICE_FLAGS::kDEFAULT == deviceMask ||
                 GPU_DEVICE_FLAGS::kPRIMARY == deviceMask) &&
                "Multiple GPUs not supported natively on DirectX 11.");
    }

    D3D11GPUProgram::~D3D11GPUProgram() {
      m_inputDeclaration = nullptr;
      GE_INC_RENDER_STAT_CAT(ResDestroyed, RENDER_STAT_RESOURCE_TYPE::kGPUProgram);
    }

    void
    D3D11GPUProgram::initialize() {
      if (!isSupported()) {
        m_isCompiled = false;
        m_compileMessages = "Program is not supported by the current render system.";

        GPUProgram::initialize();
        return;
      }

      if (!m_bytecode || DIRECTX_COMPILER_ID != m_bytecode->compilerId) {
        GPU_PROGRAM_DESC desc;
        desc.type = m_type;
        desc.entryPoint = m_entryPoint;
        desc.source = m_source;
        desc.language = "hlsl";

        m_bytecode = compileBytecode(desc);
      }

      m_compileMessages = m_bytecode->messages;
      m_isCompiled = nullptr != m_bytecode->instructions.data;

      if (m_isCompiled) {
        m_parametersDesc = m_bytecode->paramDesc;

        auto rapi = static_cast<D3D11RenderAPI*>(RenderAPI::instancePtr());
        loadFromMicrocode(rapi->getPrimaryDevice(), m_bytecode->instructions);

        if (GPU_PROGRAM_TYPE::kVERTEX_PROGRAM == m_type) {
          m_inputDeclaration = HardwareBufferManager::instance().
                                 createVertexDeclaration(m_bytecode->vertexInput);
        }

      }

      ++m_programId = s_globalProgramId;

      GE_INC_RENDER_STAT_CAT(ResCreated, RENDER_STAT_RESOURCE_TYPE::kGPUProgram);

      GPUProgram::initialize();
    }

    D3D11GPUVertexProgram::D3D11GPUVertexProgram(const GPU_PROGRAM_DESC& desc,
                                                 GPU_DEVICE_FLAGS::E deviceMask)
      : D3D11GPUProgram(desc, deviceMask),
        m_vertexShader(nullptr)
    {}

    D3D11GPUVertexProgram::~D3D11GPUVertexProgram() {
      SAFE_RELEASE(m_vertexShader);
    }

    void
    D3D11GPUVertexProgram::loadFromMicrocode(D3D11Device& device,
                                             const DataBlob& microcode) {
      HRESULT hr = device.getD3D11Device()->CreateVertexShader(microcode.data,
                                                               microcode.size,
                                                               device.getClassLinkage(),
                                                               &m_vertexShader);
      if (FAILED(hr) || device.hasError()) {
        String errorDescription = device.getErrorDescription();
        GE_EXCEPT(RenderingAPIException,
                  "Cannot create D3D11 vertex shader from microcode\n"
                  "Error Description:" + errorDescription);

      }
    }

    ID3D11VertexShader*
    D3D11GPUVertexProgram::getVertexShader() const {
      return m_vertexShader;
    }

    D3D11GPUFragmentProgram::D3D11GPUFragmentProgram(const GPU_PROGRAM_DESC& desc,
                                                     GPU_DEVICE_FLAGS::E deviceMask)
      : D3D11GPUProgram(desc, deviceMask),
        m_pixelShader(nullptr)
    {}

    D3D11GPUFragmentProgram::~D3D11GPUFragmentProgram() {
      SAFE_RELEASE(m_pixelShader);
    }

    void
    D3D11GPUFragmentProgram::loadFromMicrocode(D3D11Device& device,
                                               const DataBlob& microcode) {
      HRESULT hr = device.getD3D11Device()->CreatePixelShader(microcode.data,
                                                              microcode.size,
                                                              device.getClassLinkage(),
                                                              &m_pixelShader);
      if (FAILED(hr) || device.hasError()) {
        String errorDescription = device.getErrorDescription();
        GE_EXCEPT(RenderingAPIException,
                  "Cannot create D3D11 pixel shader from microcode.\n"
                  "Error Description:" + errorDescription);
      }
    }

    ID3D11PixelShader*
    D3D11GPUFragmentProgram::getPixelShader() const {
      return m_pixelShader;
    }


    D3D11GPUGeometryProgram::D3D11GPUGeometryProgram(const GPU_PROGRAM_DESC& desc,
                                                     GPU_DEVICE_FLAGS::E deviceMask)
      : D3D11GPUProgram(desc, deviceMask),
        m_geometryShader(nullptr)
    {}

    D3D11GPUGeometryProgram::~D3D11GPUGeometryProgram() {
      SAFE_RELEASE(m_geometryShader);
    }

    void
    D3D11GPUGeometryProgram::loadFromMicrocode(D3D11Device& device,
                                               const DataBlob& microcode) {
      HRESULT hr = device.getD3D11Device()->CreateGeometryShader(microcode.data,
                                                                 microcode.size,
                                                                 device.getClassLinkage(),
                                                                 &m_geometryShader);
      if (FAILED(hr) || device.hasError()) {
        String errorDescription = device.getErrorDescription();
        GE_EXCEPT(RenderingAPIException,
                  "Cannot create D3D11 geometry shader from microcode.\n"
                  "Error Description:" + errorDescription);
      }
    }

    ID3D11GeometryShader*
    D3D11GPUGeometryProgram::getGeometryShader() const {
      return m_geometryShader;
    }

    D3D11GPUDomainProgram::D3D11GPUDomainProgram(const GPU_PROGRAM_DESC& desc,
                                                 GPU_DEVICE_FLAGS::E deviceMask)
      : D3D11GPUProgram(desc, deviceMask),
        m_domainShader(nullptr)
    {}

    D3D11GPUDomainProgram::~D3D11GPUDomainProgram() {
      SAFE_RELEASE(m_domainShader);
    }

    void
    D3D11GPUDomainProgram::loadFromMicrocode(D3D11Device& device,
                                             const DataBlob& microcode) {
      HRESULT hr = device.getD3D11Device()->CreateDomainShader(microcode.data,
                                                               microcode.size,
                                                               device.getClassLinkage(),
                                                               &m_domainShader);
      if (FAILED(hr) || device.hasError()) {
        String errorDescription = device.getErrorDescription();
        GE_EXCEPT(RenderingAPIException,
                  "Cannot create D3D11 domain shader from microcode.\n"
                  "Error Description:" + errorDescription);
      }
    }

    ID3D11DomainShader*
    D3D11GPUDomainProgram::getDomainShader() const {
      return m_domainShader;
    }

    D3D11GPUHullProgram::D3D11GPUHullProgram(const GPU_PROGRAM_DESC& desc,
                                             GPU_DEVICE_FLAGS::E deviceMask)
      : D3D11GPUProgram(desc, deviceMask),
        m_hullShader(nullptr)
    {}

    D3D11GPUHullProgram::~D3D11GPUHullProgram() {
      SAFE_RELEASE(m_hullShader);
    }

    void
    D3D11GPUHullProgram::loadFromMicrocode(D3D11Device& device,
                                           const DataBlob& microcode) {
      //Create the shader
      HRESULT hr = device.getD3D11Device()->CreateHullShader(microcode.data,
                                                             microcode.size,
                                                             device.getClassLinkage(),
                                                             &m_hullShader);
      if (FAILED(hr) || device.hasError()) {
        String errorDescription = device.getErrorDescription();
        GE_EXCEPT(RenderingAPIException,
                  "Cannot create D3D11 hull shader from microcode.\n"
                  "Error Description:" + errorDescription);
      }
    }

    ID3D11HullShader*
    D3D11GPUHullProgram::getHullShader() const {
      return m_hullShader;
    }

    D3D11GPUComputeProgram::D3D11GPUComputeProgram(const GPU_PROGRAM_DESC& desc,
                                                   GPU_DEVICE_FLAGS::E deviceMask)
      : D3D11GPUProgram(desc, deviceMask),
        m_computeShader(nullptr)
    {}

    D3D11GPUComputeProgram::~D3D11GPUComputeProgram() {
      SAFE_RELEASE(m_computeShader);
    }

    void
    D3D11GPUComputeProgram::loadFromMicrocode(D3D11Device& device,
                                              const DataBlob& microcode) {
      HRESULT hr = device.getD3D11Device()->CreateComputeShader(microcode.data,
                                                                microcode.size,
                                                                device.getClassLinkage(),
                                                                &m_computeShader);
      if (FAILED(hr) || device.hasError()) {
        String errorDescription = device.getErrorDescription();
        GE_EXCEPT(RenderingAPIException,
                  "Cannot create D3D11 compute shader from microcode.\n"
                  "Error Description:" + errorDescription);
      }
    }

    ID3D11ComputeShader*
    D3D11GPUComputeProgram::getComputeShader() const {
      return m_computeShader;
    }
  }
}
