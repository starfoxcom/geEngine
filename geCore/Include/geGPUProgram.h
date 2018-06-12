/*****************************************************************************/
/**
 * @file    geGPUProgram.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2018/06/12
 * @brief   Contains a GPU program.
 *
 * Contains a GPU program such as vertex or fragment program which gets
 * compiled from the provided source code.
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
#include "gePrerequisitesCore.h"
#include "geCoreObject.h"
#include "geVertexDeclaration.h"

#include <geIReflectable.h>
#include <geDataBlob.h>

namespace geEngineSDK {
  struct GPUProgramBytecode;

  /**
   * @brief Descriptor structure used for initialization of a GPUProgram.
   */
  struct GPU_PROGRAM_DESC
  {
    /**
     * @brief Source code to compile the program from.
     */
    String source;

    /**
     * @brief Name of the entry point function, for example "main".
     */
    String entryPoint;
    
    /**
     * @brief Language the source is written in, for example "hlsl" or "glsl".
     */
    String language;

    /**
     * @brief Type of the program, for example vertex or fragment.
     */
    GPU_PROGRAM_TYPE::E type = GPU_PROGRAM_TYPE::kVERTEX_PROGRAM;

    /**
     * @brief If true, adjacency information will be provided when rendering.
     */
    bool requiresAdjacency = false;

    /**
     * @brief Optional intermediate version of the GPU program.
     *        Can significantly speed up GPU program compilation/creation when
     *        supported by the render back end.
     *        Call geCoreThread::GPUProgram::compileBytecode to generate it.
     */
    SPtr<GPUProgramBytecode> bytecode;
  };

  /**
   * @brief Contains a GPU program such as vertex or fragment program which
   *        gets compiled from the provided source code.
   * @note  Sim thread only.
   */
  class GE_CORE_EXPORT GPUProgram : public IReflectable, public CoreObject
  {
   public:
    /**
     * @brief Information returned when compiling a GPU program.
     */
    struct CompileStatus
    {
      bool success = false;
      String messages;
    };

    virtual ~GPUProgram() = default;

    /**
     * @brief Returns true if the program was successfully compiled.
     * @note  Only valid after core thread has initialized the program.
     */
    bool
    isCompiled() const;

    /**
     * @brief Returns an error message returned by the compiler, if the
     *        compilation failed.
     * @note  Only valid after core thread has initialized the program.
     */
    String
    getCompileErrorMessage() const;

    /**
     * @brief Returns description of all parameters in this GPU program.
     * @note  Only valid after core thread has initialized the program.
     */
    SPtr<GPUParamDesc>
    getParamDesc() const;

    /**
     * @brief Retrieves a core implementation of a GPU program usable only
     *        from the core thread.
     */
    SPtr<geCoreThread::GPUProgram>
    getCore() const;

    /**
     * @brief Creates a new GPU program using the provided source code.
     *        If compilation fails or program is not supported isCompiled()
     *        will return false, and you will be able to retrieve the error
     *        message via getCompileErrorMessage().
     * @param[in] desc  Description of the program to create.
     */
    static SPtr<GPUProgram>
    create(const GPU_PROGRAM_DESC& desc);

   protected:
    friend class GPUProgramManager;

    GPUProgram(const GPU_PROGRAM_DESC& desc);

    /**
     * @copydoc CoreObject::createCore
     */
    SPtr<geCoreThread::CoreObject>
    createCore() const override;

   protected:
    bool m_needsAdjacencyInfo;
    String m_language;
    GPU_PROGRAM_TYPE::E m_type;
    String m_entryPoint;
    String m_source;

    SPtr<GPUProgramBytecode> m_bytecode;
    /*************************************************************************/
    /**
     * Serialization
     */
    /*************************************************************************/
   public:
    friend class GPUProgramRTTI;

    static RTTITypeBase*
    getRTTIStatic();

    RTTITypeBase*
    getRTTI() const override;
  };

  /**
   * @brief A GPU program compiled to an intermediate bytecode format, as well
   *        as any relevant meta-data that could be extracted from that format.
   */
  struct GE_CORE_EXPORT GPUProgramBytecode : IReflectable
  {
    ~GPUProgramBytecode();

    /**
     * @brief Instructions (compiled code) for the GPU program.
     *        Contains no data if compilation was not successful.
     */
    DataBlob instructions;

    /**
     * @brief Reflected information about GPU program parameters.
     */
    SPtr<GPUParamDesc> paramDesc;

    /**
     * @brief Input parameters for a vertex GPU program.
     */
    Vector<VertexElement> vertexInput;

    /**
     * @brief Messages output during the compilation process.
     *        Includes errors in case compilation failed.
     */
    String messages;

    /**
     * @brief Identifier of the compiler that compiled the bytecode.
     */
    String compilerId;

    /**
     * @brief Version of the compiler that compiled the bytecode.
     */
    uint32 compilerVersion = 0;

    /*************************************************************************/
    /**
     * Serialization
     */
    /*************************************************************************/
   public:
    friend class GPUProgramBytecodeRTTI;

    static RTTITypeBase*
    getRTTIStatic();

    RTTITypeBase*
    getRTTI() const override;
  };

  namespace geCoreThread {
    /**
     * @brief Core thread version of a geEngineSDK::GPUProgram.
     * @note  Core thread only.
     */
    class GE_CORE_EXPORT GPUProgram : public CoreObject
    {
     public:
      virtual ~GPUProgram() = default;

      /**
       * @brief Returns whether this program can be supported on the current
       *        renderer and hardware.
       */
      virtual bool
      isSupported() const;

      /**
       * @brief Returns true if program was successfully compiled.
       */
      virtual bool
      isCompiled() const {
        return m_isCompiled;
      }

      /**
       * @brief Returns an error message returned by the compiler, if the
       *        compilation failed.
       */
      virtual String
      getCompileErrorMessage() const {
        return m_compileMessages;
      }

      /**
       * @brief Sets whether this geometry program requires adjacency
       *        information from the input primitives.
       * @note  Only relevant for geometry programs.
       */
      virtual void
      setAdjacencyInfoRequired(bool required) {
        m_needsAdjacencyInfo = required;
      }

      /**
       * @brief Returns whether this geometry program requires adjacency
       *        information from the input primitives.
       * @note  Only relevant for geometry programs.
       */
      virtual bool
      isAdjacencyInfoRequired() const {
        return m_needsAdjacencyInfo;
      }

      /**
       * @brief Type of GPU program (for example fragment, vertex).
       */
      GPU_PROGRAM_TYPE::E
      getType() const {
        return m_type;
      }

      /**
       * @copydoc geEngineSDK::GPUProgram::getParamDesc
       */
      SPtr<GPUParamDesc>
      getParamDesc() const {
        return m_parametersDesc;
      }

      /**
       * @brief Returns GPU program input declaration.
       *        Only relevant for vertex programs.
       */
      SPtr<VertexDeclaration>
      getInputDeclaration() const {
        return m_inputDeclaration;
      }

      /**
       * @brief Returns the compiled bytecode of this program.
       */
      SPtr<GPUProgramBytecode>
      getBytecode() const {
        return m_bytecode;
      }

      /**
       * @copydoc geEngineSDK::GPUProgram::create(const GPU_PROGRAM_DESC&)
       * @param[in] deviceMask  Mask that determines on which GPU devices
       *                        should the object be created on.
       */
      static SPtr<GPUProgram>
      create(const GPU_PROGRAM_DESC& desc,
             GPU_DEVICE_FLAGS::E deviceMask = GPU_DEVICE_FLAGS::kDEFAULT);

      /**
       * @brief Compiles the GPU program to an intermediate bytecode format.
       *        The bytecode can be cached and used for quicker
       *        compilation/creation of GPU programs.
       */
      static SPtr<GPUProgramBytecode>
      compileBytecode(const GPU_PROGRAM_DESC& desc);

     protected:
      friend class GPUProgramRTTI;

      GPUProgram(const GPU_PROGRAM_DESC& desc, GPU_DEVICE_FLAGS::E deviceMask);

      bool m_needsAdjacencyInfo;

      bool m_isCompiled = false;
      String m_compileMessages;

      SPtr<GPUParamDesc> m_parametersDesc;
      SPtr<VertexDeclaration> m_inputDeclaration;

      GPU_PROGRAM_TYPE::E m_type;
      String m_entryPoint;
      String m_source;

      SPtr<GPUProgramBytecode> m_bytecode;
    };
  }
}
