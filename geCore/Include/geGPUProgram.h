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

  /** Descriptor structure used for initialization of a GPUProgram. */
  struct GPU_PROGRAM_DESC
  {
    String source; /**< Source code to compile the program from. */
    String entryPoint; /**< Name of the entry point function, for example "main". */
    String language; /**< Language the source is written in, for example "hlsl" or "glsl". */
    GPU_PROGRAM_TYPE::E type = GPU_PROGRAM_TYPE::kVERTEX_PROGRAM; /**< Type of the program, for example vertex or fragment. */
    bool requiresAdjacency = false; /**< If true then adjacency information will be provided when rendering. */

                                    /**
                                    * Optional intermediate version of the GPU program. Can significantly speed up GPU program compilation/creation.
                                    * when supported by the render backend. Call geCoreThread::GPUProgram::compileBytecode to generate it.
                                    */
    SPtr<GPUProgramBytecode> bytecode;
  };

  /**
  * Contains a GPU program such as vertex or fragment program which gets compiled from the provided source code.
  *
  * @note	Sim thread only.
  */
  class GE_CORE_EXPORT GPUProgram : public IReflectable, public CoreObject
  {
  public:
    /** Information returned when compiling a GPU program. */
    struct CompileStatus
    {
      bool success = false;
      String messages;
    };

    virtual ~GPUProgram() { }

    /**
    * Returns true if the program was successfully compiled.
    *
    * @note	Only valid after core thread has initialized the program.
    */
    bool isCompiled() const;

    /**
    * Returns an error message returned by the compiler, if the compilation failed.
    *
    * @note	Only valid after core thread has initialized the program.
    */
    String getCompileErrorMessage() const;

    /**
    * Returns description of all parameters in this GPU program.
    *
    * @note	Only valid after core thread has initialized the program.
    */
    SPtr<GPUParamDesc> getParamDesc() const;

    /** Retrieves a core implementation of a GPU program usable only from the core thread. */
    SPtr<geCoreThread::GPUProgram> getCore() const;

    /**
    * Creates a new GPU program using the provided source code. If compilation fails or program is not supported
    * isCompiled() with return false, and you will be able to retrieve the error message via getCompileErrorMessage().
    *
    * @param[in]	desc		Description of the program to create.
    */
    static SPtr<GPUProgram> create(const GPU_PROGRAM_DESC& desc);

  protected:
    friend class GPUProgramManager;

    GPUProgram(const GPU_PROGRAM_DESC& desc);

    /** @copydoc CoreObject::createCore */
    SPtr<geCoreThread::CoreObject> createCore() const override;

  protected:
    bool mNeedsAdjacencyInfo;
    String mLanguage;
    GPU_PROGRAM_TYPE::E mType;
    String mEntryPoint;
    String mSource;

    SPtr<GPUProgramBytecode> mBytecode;

    /************************************************************************/
    /* 								SERIALIZATION                      		*/
    /************************************************************************/
  public:
    friend class GPUProgramRTTI;
    static RTTITypeBase* getRTTIStatic();
    RTTITypeBase* getRTTI() const override;
  };

  /**
  * A GPU program compiled to an intermediate bytecode format, as well as any relevant meta-data that could be
  * extracted from that format.
  */
  struct GE_CORE_EXPORT GPUProgramBytecode : IReflectable
  {
    ~GPUProgramBytecode();

    /** Instructions (compiled code) for the GPU program. Contains no data if compilation was not succesful. */
    DataBlob instructions;

    /** Reflected information about GPU program parameters. */
    SPtr<GPUParamDesc> paramDesc;

    /** Input parameters for a vertex GPU program. */
    Vector<VertexElement> vertexInput;

    /** Messages output during the compilation process. Includes errors in case compilation failed. */
    String messages;

    /** Identifier of the compiler that compiled the bytecode. */
    String compilerId;

    /** Version of the compiler that compiled the bytecode. */
    uint32 compilerVersion = 0;

    /************************************************************************/
    /* 								SERIALIZATION                      		*/
    /************************************************************************/
  public:
    friend class GPUProgramBytecodeRTTI;
    static RTTITypeBase* getRTTIStatic();
    RTTITypeBase* getRTTI() const override;
  };

  namespace geCoreThread {
    /**
    * Core thread version of a bs::GPUProgram.
    * @note	Core thread only.
    */
    class GE_CORE_EXPORT GPUProgram : public CoreObject
    {
    public:
      virtual ~GPUProgram() = default;

      /** Returns whether this program can be supported on the current renderer and hardware. */
      virtual bool isSupported() const;

      /** Returns true if program was successfully compiled. */
      virtual bool isCompiled() const { return mIsCompiled; }

      /**	Returns an error message returned by the compiler, if the compilation failed. */
      virtual String getCompileErrorMessage() const { return mCompileMessages; }

      /**
      * Sets whether this geometry program requires adjacency information from the input primitives.
      * @note	Only relevant for geometry programs.
      */
      virtual void setAdjacencyInfoRequired(bool required) { mNeedsAdjacencyInfo = required; }

      /**
      * Returns whether this geometry program requires adjacency information from the input primitives.
      * @note	Only relevant for geometry programs.
      */
      virtual bool isAdjacencyInfoRequired() const { return mNeedsAdjacencyInfo; }

      /**	Type of GPU program (for example fragment, vertex). */
      GPU_PROGRAM_TYPE::E getType() const { return mType; }

      /** @copydoc bs::GPUProgram::getParamDesc */
      SPtr<GPUParamDesc> getParamDesc() const { return mParametersDesc; }

      /**	Returns GPU program input declaration. Only relevant for vertex programs. */
      SPtr<VertexDeclaration> getInputDeclaration() const { return mInputDeclaration; }

      /** Returns the compiled bytecode of this program. */
      SPtr<GPUProgramBytecode> getBytecode() const { return mBytecode; }

      /**
      * @copydoc bs::GPUProgram::create(const GPU_PROGRAM_DESC&)
      * @param[in]	deviceMask		Mask that determines on which GPU devices should the object be created on.
      */
      static SPtr<GPUProgram> create(const GPU_PROGRAM_DESC& desc, GPU_DEVICE_FLAGS::E deviceMask = GPU_DEVICE_FLAGS::kDEFAULT);

      /**
      * Compiles the GPU program to an intermediate bytecode format. The bytecode can be cached and used for
      * quicker compilation/creation of GPU programs.
      */
      static SPtr<GPUProgramBytecode> compileBytecode(const GPU_PROGRAM_DESC& desc);

    protected:
      friend class GPUProgramRTTI;

      GPUProgram(const GPU_PROGRAM_DESC& desc, GPU_DEVICE_FLAGS::E deviceMask);

      bool mNeedsAdjacencyInfo;

      bool mIsCompiled = false;
      String mCompileMessages;

      SPtr<GPUParamDesc> mParametersDesc;
      SPtr<VertexDeclaration> mInputDeclaration;

      GPU_PROGRAM_TYPE::E mType;
      String mEntryPoint;
      String mSource;

      SPtr<GPUProgramBytecode> mBytecode;
    };
  }
}
