/*****************************************************************************/
/**
 * @file    geGPUParams.cpp
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2018/06/11
 * @brief   Contains descriptions for all parameters in a set of programs.
 *
 * Contains descriptions for all parameters in a set of programs (one for each
 * stage) and allows you to write and read those parameters. All parameter
 * values are stored internally on the CPU, and are only submitted to the GPU
 * once the parameters are bound to the pipeline.
 *
 * @bug     No known bugs.
 */
/*****************************************************************************/

/*****************************************************************************/
/**
 * Includes
 */
/*****************************************************************************/
#include "geGPUParams.h"
#include "geGPUParamDesc.h"
#include "geGPUParamBlockBuffer.h"
#include "geGPUPipelineParamInfo.h"
#include "geGPUPipelineState.h"

#include "geTexture.h"
#include "geGPUBuffer.h"
#include "geSamplerState.h"
#include "geHardwareBufferManager.h"

#include <geVector2.h>
#include <geDebug.h>
#include <geException.h>
#include <geVectorNI.h>

namespace geEngineSDK {
  using std::static_pointer_cast;

  const TextureSurface TextureSurface::COMPLETE = TextureSurface(0, 0, 0, 0);

  GPUParamsBase::GPUParamsBase(const SPtr<GPUPipelineParamInfoBase>& paramInfo)
    : m_paramInfo(paramInfo)
  {}

  SPtr<GPUParamDesc>
  GPUParamsBase::getParamDesc(GPU_PROGRAM_TYPE::E type) const {
    return m_paramInfo->getParamDesc(type);
  }

  uint32
  GPUParamsBase::getDataParamSize(GPU_PROGRAM_TYPE::E type,
                                  const String& name) const {
    GPUParamDataDesc* desc = getParamDesc(type, name);
    if (nullptr != desc) {
      return desc->elementSize * 4;
    }

    return 0;
  }

  bool
  GPUParamsBase::hasParam(GPU_PROGRAM_TYPE::E type, const String& name) const {
    return getParamDesc(type, name) != nullptr;
  }

  bool
  GPUParamsBase::hasTexture(GPU_PROGRAM_TYPE::E type,
                            const String& name) const {
    const SPtr<GPUParamDesc>& paramDesc = m_paramInfo->getParamDesc(type);
    if (nullptr == paramDesc) {
      return false;
    }

    auto paramIter = paramDesc->textures.find(name);
    if (paramDesc->textures.end() != paramIter) {
      return true;
    }

    return false;
  }

  bool
  GPUParamsBase::hasBuffer(GPU_PROGRAM_TYPE::E type,
                           const String& name) const {
    const SPtr<GPUParamDesc>& paramDesc = m_paramInfo->getParamDesc(type);
    if (nullptr == paramDesc) {
      return false;
    }

    auto paramIter = paramDesc->buffers.find(name);
    if (paramDesc->buffers.end() != paramIter) {
      return true;
    }

    return false;
  }

  bool
  GPUParamsBase::hasLoadStoreTexture(GPU_PROGRAM_TYPE::E type,
                                     const String& name) const {
    auto& paramDesc = m_paramInfo->getParamDesc(type);
    if (nullptr == paramDesc) {
      return false;
    }

    auto paramIter = paramDesc->loadStoreTextures.find(name);
    if (paramDesc->loadStoreTextures.end() != paramIter) {
      return true;
    }

    return false;
  }

  bool
  GPUParamsBase::hasSamplerState(GPU_PROGRAM_TYPE::E type,
                                 const String& name) const {
    const SPtr<GPUParamDesc>& paramDesc = m_paramInfo->getParamDesc(type);
    if (nullptr == paramDesc) {
      return false;
    }

    auto paramIter = paramDesc->samplers.find(name);
    if (paramDesc->samplers.end() != paramIter) {
      return true;
    }

    return false;
  }

  bool
  GPUParamsBase::hasParamBlock(GPU_PROGRAM_TYPE::E type,
                               const String& name) const {
    const SPtr<GPUParamDesc>& paramDesc = m_paramInfo->getParamDesc(type);
    if (nullptr == paramDesc) {
      return false;
    }

    auto paramBlockIter = paramDesc->paramBlocks.find(name);
    if (paramDesc->paramBlocks.end() != paramBlockIter) {
      return true;
    }

    return false;
  }

  GPUParamDataDesc*
  GPUParamsBase::getParamDesc(GPU_PROGRAM_TYPE::E type,
                              const String& name) const {
    const SPtr<GPUParamDesc>& paramDesc = m_paramInfo->getParamDesc(type);
    if (nullptr == paramDesc) {
      return nullptr;
    }

    auto paramIter = paramDesc->params.find(name);
    if (paramDesc->params.end() != paramIter) {
      return &paramIter->second;
    }

    return nullptr;
  }

  GPUParamBlockDesc*
  GPUParamsBase::getParamBlockDesc(GPU_PROGRAM_TYPE::E type,
    const String& name) const {
    const SPtr<GPUParamDesc>& paramDesc = m_paramInfo->getParamDesc(type);
    if (nullptr == paramDesc) {
      return nullptr;
    }

    auto paramBlockIter = paramDesc->paramBlocks.find(name);
    if (paramDesc->paramBlocks.end() != paramBlockIter) {
      return &paramBlockIter->second;
    }

    return nullptr;
  }

  template<bool Core>
  TGPUParams<Core>::TGPUParams(const SPtr<GPUPipelineParamInfoBase>& paramInfo)
    : GPUParamsBase(paramInfo) {
    uint32 numParamBlocks     = m_paramInfo->getNumElements(PARAM_TYPE::kParamBlock);
    uint32 numTextures        = m_paramInfo->getNumElements(PARAM_TYPE::kTexture);
    uint32 numStorageTextures = m_paramInfo->getNumElements(PARAM_TYPE::kLoadStoreTexture);
    uint32 numBuffers         = m_paramInfo->getNumElements(PARAM_TYPE::kBuffer);
    uint32 numSamplers        = m_paramInfo->getNumElements(PARAM_TYPE::kSamplerState);

    uint32 paramBlocksSize = sizeof(ParamsBufferType) * numParamBlocks;
    uint32 texturesSize = (sizeof(TextureType) + sizeof(TextureSurface)) * numTextures;
    uint32 loadStoreTexturesSize = (sizeof(TextureType) + sizeof(TextureSurface)) *
                                   numStorageTextures;
    uint32 buffersSize = sizeof(BufferType) * numBuffers;
    uint32 samplerStatesSize = sizeof(SamplerType) * numSamplers;

    uint32 totalSize = paramBlocksSize +
                       texturesSize +
                       loadStoreTexturesSize +
                       buffersSize +
                       samplerStatesSize;

    auto data = reinterpret_cast<uint8*>(ge_alloc(totalSize));
    m_paramBlockBuffers = reinterpret_cast<ParamsBufferType*>(data);
    for (uint32 i = 0; i < numParamBlocks; ++i) {
      new (&m_paramBlockBuffers[i]) ParamsBufferType();
    }
    data += paramBlocksSize;

    m_sampledTextureData = reinterpret_cast<TextureData*>(data);
    for (uint32 i = 0; i < numTextures; ++i) {
      new (&m_sampledTextureData[i].texture) TextureType();
      new (&m_sampledTextureData[i].surface) TextureSurface(0, 0, 0, 0);
    }
    data += texturesSize;

    m_loadStoreTextureData = reinterpret_cast<TextureData*>(data);
    for (uint32 i = 0; i < numStorageTextures; ++i) {
      new (&m_loadStoreTextureData[i].texture) TextureType();
      new (&m_loadStoreTextureData[i].surface) TextureSurface(0, 0, 0, 0);
    }
    data += loadStoreTexturesSize;

    m_buffers = reinterpret_cast<BufferType*>(data);
    for (uint32 i = 0; i < numBuffers; ++i) {
      new (&m_buffers[i]) BufferType();
    }

    data += buffersSize;
    m_samplerStates = reinterpret_cast<SamplerType*>(data);
    for (uint32 i = 0; i < numSamplers; ++i) {
      new (&m_samplerStates[i]) SamplerType();
    }
    data += samplerStatesSize;
  }

  template<bool Core>
  TGPUParams<Core>::~TGPUParams() {
    uint32 numParamBlocks     = m_paramInfo->getNumElements(PARAM_TYPE::kParamBlock);
    uint32 numTextures        = m_paramInfo->getNumElements(PARAM_TYPE::kTexture);
    uint32 numStorageTextures = m_paramInfo->getNumElements(PARAM_TYPE::kLoadStoreTexture);
    uint32 numBuffers         = m_paramInfo->getNumElements(PARAM_TYPE::kBuffer);
    uint32 numSamplers        = m_paramInfo->getNumElements(PARAM_TYPE::kSamplerState);

    for (uint32 i = 0; i < numParamBlocks; ++i) {
      m_paramBlockBuffers[i].~ParamsBufferType();
    }

    for (uint32 i = 0; i < numTextures; ++i) {
      m_sampledTextureData[i].texture.~TextureType();
      m_sampledTextureData[i].surface.~TextureSurface();
    }

    for (uint32 i = 0; i < numStorageTextures; ++i) {
      m_loadStoreTextureData[i].texture.~TextureType();
      m_loadStoreTextureData[i].surface.~TextureSurface();
    }

    for (uint32 i = 0; i < numBuffers; ++i) {
      m_buffers[i].~BufferType();
    }

    for (uint32 i = 0; i < numSamplers; ++i) {
      m_samplerStates[i].~SamplerType();
    }

    //Everything is allocated in a single block, so it's enough to free the
    //first element
    ge_free(m_paramBlockBuffers);
  }

  template<bool Core>
  void
  TGPUParams<Core>::setParamBlockBuffer(uint32 set,
                                        uint32 slot,
                                        const ParamsBufferType& paramBlockBuffer) {
    uint32 globalSlot = m_paramInfo->getSequentialSlot(PARAM_TYPE::kParamBlock, set, slot);
    if (NumLimit::MAX_UINT32 == globalSlot) {
      return;
    }

    m_paramBlockBuffers[globalSlot] = paramBlockBuffer;

    _markCoreDirty();
  }

  template<bool Core>
  void
  TGPUParams<Core>::setParamBlockBuffer(GPU_PROGRAM_TYPE::E type,
                                        const String& name,
                                        const ParamsBufferType& paramBlockBuffer) {
    const auto& paramDescs = m_paramInfo->getParamDesc(type);
    if (nullptr == paramDescs) {
      LOGWRN("Cannot find parameter block with the name: '" + name + "'");
      return;
    }

    auto iterFind = paramDescs->paramBlocks.find(name);
    if (paramDescs->paramBlocks.end() == iterFind) {
      LOGWRN("Cannot find parameter block with the name: '" + name + "'");
      return;
    }

    setParamBlockBuffer(iterFind->second.set,
                        iterFind->second.slot,
                        paramBlockBuffer);
  }

  template<bool Core>
  void
  TGPUParams<Core>::setParamBlockBuffer(const String& name,
                                        const ParamsBufferType& paramBlockBuffer) {
    for (uint32 i = 0; i < 6; ++i) {
      const auto& paramDescs = m_paramInfo->getParamDesc(static_cast<GPU_PROGRAM_TYPE::E>(i));
      if (nullptr == paramDescs) {
        continue;
      }

      auto iterFind = paramDescs->paramBlocks.find(name);
      if (paramDescs->paramBlocks.end() == iterFind) {
        continue;
      }

      setParamBlockBuffer(iterFind->second.set,
                          iterFind->second.slot,
                          paramBlockBuffer);
    }
  }

  template<bool Core>
  template<class T>
  void
  TGPUParams<Core>::getParam(GPU_PROGRAM_TYPE::E type,
                             const String& name,
                             TGPUDataParam<T, Core>& output) const {
    const auto& paramDescs = m_paramInfo->getParamDesc(type);
    if (nullptr == paramDescs) {
      output = TGPUDataParam<T, Core>(nullptr, nullptr);
      LOGWRN("Cannot find parameter block with the name: '" + name + "'");
      return;
    }

    auto iterFind = paramDescs->params.find(name);
    if (paramDescs->params.end() == iterFind) {
      output = TGPUDataParam<T, Core>(nullptr, nullptr);
      LOGWRN("Cannot find parameter with the name '" + name + "'");
    }
    else {
      output = TGPUDataParam<T, Core>(&iterFind->second, _getThisPtr());
    }
  }

  template<bool Core>
  void
  TGPUParams<Core>::getStructParam(GPU_PROGRAM_TYPE::E type,
                                   const String& name,
                                   TGPUParamStruct<Core>& output) const {
    const auto& paramDescs = m_paramInfo->getParamDesc(type);
    if (nullptr == paramDescs) {
      output = TGPUParamStruct<Core>(nullptr, nullptr);
      LOGWRN("Cannot find struct parameter with the name: '" + name + "'");
      return;
    }

    auto iterFind = paramDescs->params.find(name);
    if (iterFind == paramDescs->params.end() ||
        iterFind->second.type != GPU_PARAM_DATA_TYPE::kSTRUCT) {
      output = TGPUParamStruct<Core>(nullptr, nullptr);
      LOGWRN("Cannot find struct parameter with the name '" + name + "'");
    }
    else {
      output = TGPUParamStruct<Core>(&iterFind->second, _getThisPtr());
    }
  }

  template<bool Core>
  void
  TGPUParams<Core>::getTextureParam(GPU_PROGRAM_TYPE::E type,
                                    const String& name,
                                    TGPUParamTexture<Core>& output) const {
    const auto& paramDescs = m_paramInfo->getParamDesc(type);
    if (nullptr == paramDescs) {
      output = TGPUParamTexture<Core>(nullptr, nullptr);
      LOGWRN("Cannot find texture parameter with the name: '" + name + "'");
      return;
    }

    auto iterFind = paramDescs->textures.find(name);
    if (paramDescs->textures.end() == iterFind) {
      output = TGPUParamTexture<Core>(nullptr, nullptr);
      LOGWRN("Cannot find texture parameter with the name '" + name + "'");
    }
    else {
      output = TGPUParamTexture<Core>(&iterFind->second, _getThisPtr());
    }
  }

  template<bool Core>
  void
  TGPUParams<Core>::getLoadStoreTextureParam(GPU_PROGRAM_TYPE::E type,
                                             const String& name,
                                             TGPUParamLoadStoreTexture<Core>& output) const {
    const auto& paramDescs = m_paramInfo->getParamDesc(type);
    if (nullptr == paramDescs) {
      output = TGPUParamLoadStoreTexture<Core>(nullptr, nullptr);
      LOGWRN("Cannot find texture parameter with the name: '" + name + "'");
      return;
    }

    auto iterFind = paramDescs->loadStoreTextures.find(name);
    if (paramDescs->loadStoreTextures.end() == iterFind) {
      output = TGPUParamLoadStoreTexture<Core>(nullptr, nullptr);
      LOGWRN("Cannot find texture parameter with the name '" + name + "'");
    }
    else {
      output = TGPUParamLoadStoreTexture<Core>(&iterFind->second, _getThisPtr());
    }
  }

  template<bool Core>
  void
  TGPUParams<Core>::getBufferParam(GPU_PROGRAM_TYPE::E type,
                                   const String& name,
                                   TGPUParamBuffer<Core>& output) const {
    const auto& paramDescs = m_paramInfo->getParamDesc(type);
    if (nullptr == paramDescs) {
      output = TGPUParamBuffer<Core>(nullptr, nullptr);
      LOGWRN("Cannot find buffer parameter with the name: '" + name + "'");
      return;
    }

    auto iterFind = paramDescs->buffers.find(name);
    if (paramDescs->buffers.end() == iterFind) {
      output = TGPUParamBuffer<Core>(nullptr, nullptr);
      LOGWRN("Cannot find buffer parameter with the name '" + name + "'");
    }
    else {
      output = TGPUParamBuffer<Core>(&iterFind->second, _getThisPtr());
    }
  }

  template<bool Core>
  void
  TGPUParams<Core>::getSamplerStateParam(GPU_PROGRAM_TYPE::E type,
                                         const String& name,
                                         TGPUParamSampState<Core>& output) const {
    const auto& paramDescs = m_paramInfo->getParamDesc(type);
    if (nullptr == paramDescs) {
      output = TGPUParamSampState<Core>(nullptr, nullptr);
      LOGWRN("Cannot find sampler state parameter with the name: '" +
             name + "'");
      return;
    }

    auto iterFind = paramDescs->samplers.find(name);
    if (paramDescs->samplers.end() == iterFind) {
      output = TGPUParamSampState<Core>(nullptr, nullptr);
      LOGWRN("Cannot find sampler state parameter with the name '" +
             name + "'");
    }
    else {
      output = TGPUParamSampState<Core>(&iterFind->second, _getThisPtr());
    }
  }

  template<bool Core>
  typename TGPUParams<Core>::ParamsBufferType
  TGPUParams<Core>::getParamBlockBuffer(uint32 set, uint32 slot) const {
    uint32 globalSlot = m_paramInfo->getSequentialSlot(PARAM_TYPE::kParamBlock, set, slot);
    if (NumLimit::MAX_UINT32 == globalSlot) {
      return nullptr;
    }

    return m_paramBlockBuffers[globalSlot];
  }

  template<bool Core>
  typename TGPUParams<Core>::TextureType
  TGPUParams<Core>::getTexture(uint32 set, uint32 slot) const {
    uint32 globalSlot = m_paramInfo->getSequentialSlot(PARAM_TYPE::kTexture, set, slot);
    if (NumLimit::MAX_UINT32 == globalSlot) {
      return TGPUParams<Core>::TextureType();
    }

    return m_sampledTextureData[globalSlot].texture;
  }

  template<bool Core>
  typename TGPUParams<Core>::TextureType
  TGPUParams<Core>::getLoadStoreTexture(uint32 set, uint32 slot) const {
    uint32 globalSlot = m_paramInfo->getSequentialSlot(PARAM_TYPE::kLoadStoreTexture,
                                                       set,
                                                       slot);
    if (NumLimit::MAX_UINT32 == globalSlot) {
      return TGPUParams<Core>::TextureType();
    }

    return m_loadStoreTextureData[globalSlot].texture;
  }

  template<bool Core>
  typename TGPUParams<Core>::BufferType
  TGPUParams<Core>::getBuffer(uint32 set, uint32 slot) const {
    uint32 globalSlot = m_paramInfo->getSequentialSlot(PARAM_TYPE::kBuffer, set, slot);
    if (NumLimit::MAX_UINT32 == globalSlot) {
      return nullptr;
    }

    return m_buffers[globalSlot];
  }

  template<bool Core>
  typename TGPUParams<Core>::SamplerType
  TGPUParams<Core>::getSamplerState(uint32 set, uint32 slot) const {
    uint32 globalSlot = m_paramInfo->getSequentialSlot(PARAM_TYPE::kSamplerState, set, slot);
    if (NumLimit::MAX_UINT32 == globalSlot) {
      return nullptr;
    }

    return m_samplerStates[globalSlot];
  }

  template<bool Core>
  const TextureSurface&
  TGPUParams<Core>::getTextureSurface(uint32 set, uint32 slot) const {
    static TextureSurface emptySurface;

    uint32 globalSlot = m_paramInfo->getSequentialSlot(PARAM_TYPE::kTexture, set, slot);
    if (NumLimit::MAX_UINT32 == globalSlot) {
      return emptySurface;
    }

    return m_sampledTextureData[globalSlot].surface;
  }

  template<bool Core>
  const TextureSurface&
  TGPUParams<Core>::getLoadStoreSurface(uint32 set, uint32 slot) const {
    static TextureSurface emptySurface;

    uint32 globalSlot = m_paramInfo->getSequentialSlot(PARAM_TYPE::kLoadStoreTexture,
                                                       set,
                                                       slot);
    if (NumLimit::MAX_UINT32 == globalSlot) {
      return emptySurface;
    }

    return m_loadStoreTextureData[globalSlot].surface;
  }


  template<bool Core>
  void
  TGPUParams<Core>::setTexture(uint32 set,
                               uint32 slot,
                               const TextureType& texture,
                               const TextureSurface& surface) {
    uint32 globalSlot = m_paramInfo->getSequentialSlot(PARAM_TYPE::kTexture, set, slot);
    if (NumLimit::MAX_UINT32 == globalSlot) {
      return;
    }

    m_sampledTextureData[globalSlot].texture = texture;
    m_sampledTextureData[globalSlot].surface = surface;

    _markResourcesDirty();
    _markCoreDirty();
  }

  template<bool Core>
  void
  TGPUParams<Core>::setLoadStoreTexture(uint32 set,
                                        uint32 slot,
                                        const TextureType& texture,
                                        const TextureSurface& surface) {
    uint32 globalSlot = m_paramInfo->getSequentialSlot(PARAM_TYPE::kLoadStoreTexture,
                                                       set,
                                                       slot);
    if (NumLimit::MAX_UINT32 == globalSlot) {
      return;
    }

    m_loadStoreTextureData[globalSlot].texture = texture;
    m_loadStoreTextureData[globalSlot].surface = surface;

    _markResourcesDirty();
    _markCoreDirty();
  }

  template<bool Core>
  void
  TGPUParams<Core>::setBuffer(uint32 set, uint32 slot, const BufferType& buffer) {
    uint32 globalSlot = m_paramInfo->getSequentialSlot(PARAM_TYPE::kBuffer, set, slot);
    if (NumLimit::MAX_UINT32 == globalSlot) {
      return;
    }

    m_buffers[globalSlot] = buffer;

    _markResourcesDirty();
    _markCoreDirty();
  }

  template<bool Core>
  void
  TGPUParams<Core>::setSamplerState(uint32 set, uint32 slot, const SamplerType& sampler) {
    uint32 globalSlot = m_paramInfo->getSequentialSlot(PARAM_TYPE::kSamplerState, set, slot);
    if (NumLimit::MAX_UINT32 == globalSlot) {
      return;
    }

    m_samplerStates[globalSlot] = sampler;

    _markResourcesDirty();
    _markCoreDirty();
  }

  template class TGPUParams<false>;
  template class TGPUParams<true>;

  template
  GE_CORE_EXPORT void
  TGPUParams<false>::getParam<float>(GPU_PROGRAM_TYPE::E type,
                                     const String&,
                                     TGPUDataParam<float, false>&) const;

  template
  GE_CORE_EXPORT void
  TGPUParams<false>::getParam<int>(GPU_PROGRAM_TYPE::E type,
                                   const String&,
                                   TGPUDataParam<int, false>&) const;

  template
  GE_CORE_EXPORT void
  TGPUParams<false>::getParam<LinearColor>(GPU_PROGRAM_TYPE::E type,
                                           const String&,
                                           TGPUDataParam<LinearColor, false>&) const;

  template
  GE_CORE_EXPORT void
  TGPUParams<false>::getParam<Vector2>(GPU_PROGRAM_TYPE::E type,
                                       const String&,
                                       TGPUDataParam<Vector2, false>&) const;

  template
  GE_CORE_EXPORT void
  TGPUParams<false>::getParam<Vector3>(GPU_PROGRAM_TYPE::E type,
                                       const String&,
                                       TGPUDataParam<Vector3, false>&) const;

  template
  GE_CORE_EXPORT void
  TGPUParams<false>::getParam<Vector4>(GPU_PROGRAM_TYPE::E type,
                                       const String&,
                                       TGPUDataParam<Vector4, false>&) const;

  template
  GE_CORE_EXPORT void
  TGPUParams<false>::getParam<Vector2I>(GPU_PROGRAM_TYPE::E type,
                                        const String&,
                                        TGPUDataParam<Vector2I, false>&) const;

  template
  GE_CORE_EXPORT void
  TGPUParams<false>::getParam<Vector3I>(GPU_PROGRAM_TYPE::E type,
                                        const String&,
                                        TGPUDataParam<Vector3I, false>&) const;

  template
  GE_CORE_EXPORT void
  TGPUParams<false>::getParam<Vector4I>(GPU_PROGRAM_TYPE::E type,
                                        const String&,
                                        TGPUDataParam<Vector4I, false>&) const;

  template
  GE_CORE_EXPORT void
  TGPUParams<false>::getParam<Matrix4>(GPU_PROGRAM_TYPE::E type,
                                       const String&,
                                       TGPUDataParam<Matrix4, false>&) const;


  template
  GE_CORE_EXPORT void
  TGPUParams<true>::getParam<float>(GPU_PROGRAM_TYPE::E type,
                                    const String&,
                                    TGPUDataParam<float, true>&) const;

  template
  GE_CORE_EXPORT void
  TGPUParams<true>::getParam<int>(GPU_PROGRAM_TYPE::E type,
                                  const String&,
                                  TGPUDataParam<int, true>&) const;

  template
  GE_CORE_EXPORT void
  TGPUParams<true>::getParam<LinearColor>(GPU_PROGRAM_TYPE::E type,
                                          const String&,
                                          TGPUDataParam<LinearColor, true>&) const;

  template
  GE_CORE_EXPORT void
  TGPUParams<true>::getParam<Vector2>(GPU_PROGRAM_TYPE::E type,
                                      const String&,
                                      TGPUDataParam<Vector2, true>&) const;

  template
  GE_CORE_EXPORT void
  TGPUParams<true>::getParam<Vector3>(GPU_PROGRAM_TYPE::E type,
                                      const String&,
                                      TGPUDataParam<Vector3, true>&) const;

  template
  GE_CORE_EXPORT void
  TGPUParams<true>::getParam<Vector4>(GPU_PROGRAM_TYPE::E type,
                                      const String&,
                                      TGPUDataParam<Vector4, true>&) const;

  template
  GE_CORE_EXPORT void
  TGPUParams<true>::getParam<Vector2I>(GPU_PROGRAM_TYPE::E type,
                                       const String&,
                                       TGPUDataParam<Vector2I, true>&) const;

  template
  GE_CORE_EXPORT void
  TGPUParams<true>::getParam<Vector3I>(GPU_PROGRAM_TYPE::E type,
                                       const String&,
                                       TGPUDataParam<Vector3I, true>&) const;

  template
  GE_CORE_EXPORT void
  TGPUParams<true>::getParam<Vector4I>(GPU_PROGRAM_TYPE::E type,
                                       const String&,
                                       TGPUDataParam<Vector4I, true>&) const;

  template
  GE_CORE_EXPORT void
  TGPUParams<true>::getParam<Matrix4>(GPU_PROGRAM_TYPE::E type,
                                      const String&,
                                      TGPUDataParam<Matrix4, true>&) const;

  const GPUDataParamInfos GPUParams::PARAM_SIZES;

  GPUParams::GPUParams(const SPtr<GPUPipelineParamInfo>& paramInfo)
    : TGPUParams(paramInfo)
  {}

  SPtr<GPUParams>
  GPUParams::_getThisPtr() const {
    return static_pointer_cast<GPUParams>(getThisPtr());
  }

  SPtr<geCoreThread::GPUParams>
  GPUParams::getCore() const {
    return static_pointer_cast<geCoreThread::GPUParams>(m_coreSpecific);
  }

  SPtr<geCoreThread::CoreObject>
  GPUParams::createCore() const {
    auto paramInfo = static_pointer_cast<GPUPipelineParamInfo>(m_paramInfo);
    return geCoreThread::HardwareBufferManager::instance().
      createGPUParams(paramInfo->getCore());
  }

  void
  GPUParams::_markCoreDirty() {
    markCoreDirty();
  }

  void
  GPUParams::_markResourcesDirty() {
    markListenerResourcesDirty();
  }

  SPtr<GPUParams>
  GPUParams::create(const SPtr<GraphicsPipelineState>& pipelineState) {
    return HardwareBufferManager::instance().createGPUParams(pipelineState->getParamInfo());
  }

  SPtr<GPUParams>
  GPUParams::create(const SPtr<ComputePipelineState>& pipelineState) {
    return HardwareBufferManager::instance().createGPUParams(pipelineState->getParamInfo());
  }

  SPtr<GPUParams>
  GPUParams::create(const SPtr<GPUPipelineParamInfo>& paramInfo) {
    return HardwareBufferManager::instance().createGPUParams(paramInfo);
  }

  CoreSyncData
  GPUParams::syncToCore(FrameAlloc* allocator) {
    uint32 numParamBlocks     = m_paramInfo->getNumElements(PARAM_TYPE::kParamBlock);
    uint32 numTextures        = m_paramInfo->getNumElements(PARAM_TYPE::kTexture);
    uint32 numStorageTextures = m_paramInfo->getNumElements(PARAM_TYPE::kLoadStoreTexture);
    uint32 numBuffers         = m_paramInfo->getNumElements(PARAM_TYPE::kBuffer);
    uint32 numSamplers        = m_paramInfo->getNumElements(PARAM_TYPE::kSamplerState);

    uint32 sampledSurfacesSize = numTextures * sizeof(TextureSurface);
    uint32 loadStoreSurfacesSize = numStorageTextures * sizeof(TextureSurface);
    uint32 paramBufferSize = numParamBlocks * sizeof(SPtr<geCoreThread::GPUParamBlockBuffer>);
    uint32 textureArraySize = numTextures * sizeof(SPtr<geCoreThread::Texture>);
    uint32 loadStoreTextureArraySize = numStorageTextures *
                                       sizeof(SPtr<geCoreThread::Texture>);
    uint32 bufferArraySize = numBuffers * sizeof(SPtr<geCoreThread::GPUBuffer>);
    uint32 samplerArraySize = numSamplers * sizeof(SPtr<geCoreThread::SamplerState>);

    uint32 totalSize = sampledSurfacesSize +
                       loadStoreSurfacesSize +
                       paramBufferSize +
                       textureArraySize +
                       loadStoreTextureArraySize +
                       bufferArraySize +
                       samplerArraySize;

    uint32 sampledSurfaceOffset = 0;
    uint32 loadStoreSurfaceOffset = sampledSurfaceOffset + sampledSurfacesSize;
    uint32 paramBufferOffset = loadStoreSurfaceOffset + loadStoreSurfacesSize;
    uint32 textureArrayOffset = paramBufferOffset + paramBufferSize;
    uint32 loadStoreTextureArrayOffset = textureArrayOffset + textureArraySize;
    uint32 bufferArrayOffset = loadStoreTextureArrayOffset + loadStoreTextureArraySize;
    uint32 samplerArrayOffset = bufferArrayOffset + bufferArraySize;

    uint8* data = allocator->alloc(totalSize);

    auto sampledSurfaces = reinterpret_cast<TextureSurface*>(data + sampledSurfaceOffset);
    auto loadStoreSurfaces = reinterpret_cast<TextureSurface*>(data + loadStoreSurfaceOffset);
    auto paramBuffers = reinterpret_cast<SPtr<geCoreThread::GPUParamBlockBuffer>*>(data + paramBufferOffset);
    auto textures = reinterpret_cast<<SPtr<geCoreThread::Texture>*>(data + textureArrayOffset);
    auto loadStoreTextures = (SPtr<geCoreThread::Texture>*)(data + loadStoreTextureArrayOffset);
    auto buffers = (SPtr<geCoreThread::GPUBuffer>*)(data + bufferArrayOffset);
    auto samplers = (SPtr<geCoreThread::SamplerState>*)(data + samplerArrayOffset);

    // Construct & copy
    for (uint32 i = 0; i < numParamBlocks; i++)
    {
      new (&paramBuffers[i]) SPtr<geCoreThread::GPUParamBlockBuffer>();

      if (m_paramBlockBuffers[i] != nullptr)
        paramBuffers[i] = m_paramBlockBuffers[i]->getCore();
    }

    for (uint32 i = 0; i < numTextures; i++)
    {
      new (&sampledSurfaces[i]) TextureSurface();
      sampledSurfaces[i] = m_sampledTextureData[i].surface;

      new (&textures[i]) SPtr<geCoreThread::Texture>();

      if (m_sampledTextureData[i].texture.isLoaded())
        textures[i] = m_sampledTextureData[i].texture->getCore();
      else
        textures[i] = nullptr;
    }

    for (uint32 i = 0; i < numStorageTextures; i++)
    {
      new (&loadStoreSurfaces[i]) TextureSurface();
      loadStoreSurfaces[i] = m_loadStoreTextureData[i].surface;

      new (&loadStoreTextures[i]) SPtr<geCoreThread::Texture>();

      if (m_loadStoreTextureData[i].texture.isLoaded())
        loadStoreTextures[i] = m_loadStoreTextureData[i].texture->getCore();
      else
        loadStoreTextures[i] = nullptr;
    }

    for (uint32 i = 0; i < numBuffers; i++)
    {
      new (&buffers[i]) SPtr<geCoreThread::GPUBuffer>();

      if (m_buffers[i] != nullptr)
        buffers[i] = m_buffers[i]->getCore();
      else
        buffers[i] = nullptr;
    }

    for (uint32 i = 0; i < numSamplers; i++)
    {
      new (&samplers[i]) SPtr<geCoreThread::SamplerState>();

      if (m_samplerStates[i] != nullptr)
        samplers[i] = m_samplerStates[i]->getCore();
      else
        samplers[i] = nullptr;
    }

    return CoreSyncData(data, totalSize);
  }

  void GPUParams::getListenerResources(Vector<HResource>& resources)
  {
    uint32 numTextures = m_paramInfo->getNumElements(PARAM_TYPE::kTexture);
    uint32 numStorageTextures = m_paramInfo->getNumElements(PARAM_TYPE::kLoadStoreTexture);

    for (uint32 i = 0; i < numTextures; i++)
    {
      if (m_sampledTextureData[i].texture != nullptr)
        resources.push_back(m_sampledTextureData[i].texture);
    }

    for (uint32 i = 0; i < numStorageTextures; i++)
    {
      if (m_loadStoreTextureData[i].texture != nullptr)
        resources.push_back(m_loadStoreTextureData[i].texture);
    }
  }

  namespace geCoreThread
  {
    GPUParams::GPUParams(const SPtr<GPUPipelineParamInfo>& paramInfo, GPU_DEVICE_FLAGS::E /*deviceMask*/)
      : TGPUParams(paramInfo)
    {

    }

    SPtr<GPUParams> GPUParams::_getThisPtr() const
    {
      return std::static_pointer_cast<GPUParams>(getThisPtr());
    }

    void GPUParams::syncToCore(const CoreSyncData& data)
    {
      uint32 numParamBlocks = m_paramInfo->getNumElements(PARAM_TYPE::kParamBlock);
      uint32 numTextures = m_paramInfo->getNumElements(PARAM_TYPE::kTexture);
      uint32 numStorageTextures = m_paramInfo->getNumElements(PARAM_TYPE::kLoadStoreTexture);
      uint32 numBuffers = m_paramInfo->getNumElements(PARAM_TYPE::kBuffer);
      uint32 numSamplers = m_paramInfo->getNumElements(PARAM_TYPE::kSamplerState);

      uint32 sampledSurfacesSize = numTextures * sizeof(TextureSurface);
      uint32 loadStoreSurfacesSize = numStorageTextures * sizeof(TextureSurface);
      uint32 paramBufferSize = numParamBlocks * sizeof(SPtr<GPUParamBlockBuffer>);
      uint32 textureArraySize = numTextures * sizeof(SPtr<Texture>);
      uint32 loadStoreTextureArraySize = numStorageTextures * sizeof(SPtr<Texture>);
      uint32 bufferArraySize = numBuffers * sizeof(SPtr<GPUBuffer>);
      uint32 samplerArraySize = numSamplers * sizeof(SPtr<SamplerState>);

      uint32 totalSize = sampledSurfacesSize + loadStoreSurfacesSize + paramBufferSize + textureArraySize
        + loadStoreTextureArraySize + bufferArraySize + samplerArraySize;

      uint32 sampledSurfacesOffset = 0;
      uint32 loadStoreSurfaceOffset = sampledSurfacesOffset + sampledSurfacesSize;
      uint32 paramBufferOffset = loadStoreSurfaceOffset + loadStoreSurfacesSize;
      uint32 textureArrayOffset = paramBufferOffset + paramBufferSize;
      uint32 loadStoreTextureArrayOffset = textureArrayOffset + textureArraySize;
      uint32 bufferArrayOffset = loadStoreTextureArrayOffset + loadStoreTextureArraySize;
      uint32 samplerArrayOffset = bufferArrayOffset + bufferArraySize;

      assert(data.getBufferSize() == totalSize);

      uint8* dataPtr = data.getBuffer();

      TextureSurface* sampledSurfaces = (TextureSurface*)(dataPtr + sampledSurfacesOffset);
      TextureSurface* loadStoreSurfaces = (TextureSurface*)(dataPtr + loadStoreSurfaceOffset);
      SPtr<GPUParamBlockBuffer>* paramBuffers = (SPtr<GPUParamBlockBuffer>*)(dataPtr + paramBufferOffset);
      SPtr<Texture>* textures = (SPtr<Texture>*)(dataPtr + textureArrayOffset);
      SPtr<Texture>* loadStoreTextures = (SPtr<Texture>*)(dataPtr + loadStoreTextureArrayOffset);
      SPtr<GPUBuffer>* buffers = (SPtr<GPUBuffer>*)(dataPtr + bufferArrayOffset);
      SPtr<SamplerState>* samplers = (SPtr<SamplerState>*)(dataPtr + samplerArrayOffset);

      // Copy & destruct
      for (uint32 i = 0; i < numParamBlocks; i++)
      {
        m_paramBlockBuffers[i] = paramBuffers[i];
        paramBuffers[i].~SPtr<GPUParamBlockBuffer>();
      }

      for (uint32 i = 0; i < numTextures; i++)
      {
        m_sampledTextureData[i].surface = sampledSurfaces[i];
        loadStoreSurfaces[i].~TextureSurface();

        m_sampledTextureData[i].texture = textures[i];
        textures[i].~SPtr<Texture>();
      }

      for (uint32 i = 0; i < numStorageTextures; i++)
      {
        m_loadStoreTextureData[i].surface = loadStoreSurfaces[i];
        loadStoreSurfaces[i].~TextureSurface();

        m_loadStoreTextureData[i].texture = loadStoreTextures[i];
        loadStoreTextures[i].~SPtr<Texture>();
      }

      for (uint32 i = 0; i < numBuffers; i++)
      {
        m_buffers[i] = buffers[i];
        buffers[i].~SPtr<GPUBuffer>();
      }

      for (uint32 i = 0; i < numSamplers; i++)
      {
        m_samplerStates[i] = samplers[i];
        samplers[i].~SPtr<SamplerState>();
      }
    }

    SPtr<GPUParams> GPUParams::create(const SPtr<GraphicsPipelineState>& pipelineState, GPU_DEVICE_FLAGS::E deviceMask)
    {
      return HardwareBufferManager::instance().createGPUParams(pipelineState->getParamInfo(), deviceMask);
    }

    SPtr<GPUParams> GPUParams::create(const SPtr<ComputePipelineState>& pipelineState, GPU_DEVICE_FLAGS::E deviceMask)
    {
      return HardwareBufferManager::instance().createGPUParams(pipelineState->getParamInfo(), deviceMask);
    }

    SPtr<GPUParams> GPUParams::create(const SPtr<GPUPipelineParamInfo>& paramInfo, GPU_DEVICE_FLAGS::E deviceMask)
    {
      return HardwareBufferManager::instance().createGPUParams(paramInfo, deviceMask);
    }
  }
}
