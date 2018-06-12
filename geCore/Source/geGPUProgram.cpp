/*****************************************************************************/
/**
 * @file    geGPUProgram.cpp
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
#include "geGPUProgram.h"
#include "geGPUProgramRTTI.h"
#include "geRenderAPICapabilities.h"
#include "geRenderAPI.h"
#include "geGPUParams.h"
#include "geGPUParamDesc.h"
#include "geGPUProgramManager.h"

namespace geEngineSDK {
  using std::static_pointer_cast;

  GPUProgramBytecode::~GPUProgramBytecode()
  {
    if (instructions.data)
      ge_free(instructions.data);
  }

  RTTITypeBase* GPUProgramBytecode::getRTTIStatic()
  {
    return GPUProgramBytecodeRTTI::instance();
  }

  RTTITypeBase* GPUProgramBytecode::getRTTI() const
  {
    return GPUProgramBytecode::getRTTIStatic();
  }

  GPUProgram::GPUProgram(const GPU_PROGRAM_DESC& desc)
    : mNeedsAdjacencyInfo(desc.requiresAdjacency), mLanguage(desc.language), mType(desc.type)
    , mEntryPoint(desc.entryPoint), mSource(desc.source)
  {

  }

  bool GPUProgram::isCompiled() const
  {
    return getCore()->isCompiled();
  }

  String GPUProgram::getCompileErrorMessage() const
  {
    return getCore()->getCompileErrorMessage();
  }

  SPtr<GPUParamDesc> GPUProgram::getParamDesc() const
  {
    return getCore()->getParamDesc();
  }

  SPtr<geCoreThread::GPUProgram> GPUProgram::getCore() const
  {
    return static_pointer_cast<geCoreThread::GPUProgram>(m_coreSpecific);
  }

  SPtr<geCoreThread::CoreObject> GPUProgram::createCore() const
  {
    GPU_PROGRAM_DESC desc;
    desc.source = mSource;
    desc.entryPoint = mEntryPoint;
    desc.language = mLanguage;
    desc.type = mType;
    desc.requiresAdjacency = mNeedsAdjacencyInfo;
    desc.bytecode = mBytecode;

    return geCoreThread::GPUProgramManager::instance().createInternal(desc);
  }

  SPtr<GPUProgram> GPUProgram::create(const GPU_PROGRAM_DESC& desc)
  {
    return GPUProgramManager::instance().create(desc);
  }

  /************************************************************************/
  /* 								SERIALIZATION                      		*/
  /************************************************************************/
  RTTITypeBase* GPUProgram::getRTTIStatic()
  {
    return GPUProgramRTTI::instance();
  }

  RTTITypeBase* GPUProgram::getRTTI() const
  {
    return GPUProgram::getRTTIStatic();
  }

  namespace geCoreThread
  {
    GPUProgram::GPUProgram(const GPU_PROGRAM_DESC& desc, GPU_DEVICE_FLAGS::E deviceMask)
      : mNeedsAdjacencyInfo(desc.requiresAdjacency), mType(desc.type), mEntryPoint(desc.entryPoint), mSource(desc.source)
      , mBytecode(desc.bytecode)
    {
      mParametersDesc = ge_shared_ptr_new<GPUParamDesc>();
    }

    bool GPUProgram::isSupported() const
    {
      return true;
    }

    SPtr<GPUProgram> GPUProgram::create(const GPU_PROGRAM_DESC& desc, GPU_DEVICE_FLAGS::E deviceMask)
    {
      return GPUProgramManager::instance().create(desc, deviceMask);
    }

    SPtr<GPUProgramBytecode> GPUProgram::compileBytecode(const GPU_PROGRAM_DESC& desc)
    {
      return GPUProgramManager::instance().compileBytecode(desc);
    }
  }
}