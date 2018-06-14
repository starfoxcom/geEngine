/*****************************************************************************/
/**
 * @file    geGPUPipelineParamInfo.cpp
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2018/06/11
 * @brief   Meta-data about a set of GPU parameters used by a pipeline state.
 *
 * Holds meta-data about a set of GPU parameters used by a single pipeline
 * state.
 *
 * @bug     No known bugs.
 */
/*****************************************************************************/

/*****************************************************************************/
/**
 * Includes
 */
/*****************************************************************************/
#include "geGPUPipelineParamInfo.h"
#include "geGPUParamDesc.h"
#include "geRenderStateManager.h"

#include <geNumericLimits.h>

namespace geEngineSDK {
  using std::max;
  using std::static_pointer_cast;

  GPUPipelineParamInfoBase::GPUPipelineParamInfoBase(const GPU_PIPELINE_PARAMS_DESC& desc)
    : m_numSets(0),
      m_numElements(0),
      m_setInfos(nullptr),
      m_resourceInfos() {
    ge_zero_out(m_numElementsPerType);

    m_paramDescs[GPU_PROGRAM_TYPE::kFRAGMENT_PROGRAM] = desc.fragmentParams;
    m_paramDescs[GPU_PROGRAM_TYPE::kVERTEX_PROGRAM] = desc.vertexParams;
    m_paramDescs[GPU_PROGRAM_TYPE::kGEOMETRY_PROGRAM] = desc.geometryParams;
    m_paramDescs[GPU_PROGRAM_TYPE::kHULL_PROGRAM] = desc.hullParams;
    m_paramDescs[GPU_PROGRAM_TYPE::kDOMAIN_PROGRAM] = desc.domainParams;
    m_paramDescs[GPU_PROGRAM_TYPE::kCOMPUTE_PROGRAM] = desc.computeParams;

    auto countElements = [&](auto& entry, PARAM_TYPE::E type)
    {
      int32 typeIdx = static_cast<int32>(type);

      if ((entry.set + 1) > m_numSets) {
        m_numSets = entry.set + 1;
      }

      ++m_numElementsPerType[typeIdx];
      ++m_numElements;
    };

    uint32 numParamDescs = sizeof(m_paramDescs) / sizeof(m_paramDescs[0]);
    for (uint32 i = 0; i < numParamDescs; ++i) {
      const SPtr<GPUParamDesc>& paramDesc = m_paramDescs[i];
      if (nullptr == paramDesc) {
        continue;
      }

      for (auto& paramBlock : paramDesc->paramBlocks) {
        countElements(paramBlock.second, PARAM_TYPE::kParamBlock);
      }

      for (auto& texture : paramDesc->textures) {
        countElements(texture.second, PARAM_TYPE::kTexture);
      }

      for (auto& texture : paramDesc->loadStoreTextures) {
        countElements(texture.second, PARAM_TYPE::kLoadStoreTexture);
      }

      for (auto& buffer : paramDesc->buffers) {
        countElements(buffer.second, PARAM_TYPE::kBuffer);
      }

      for (auto& sampler : paramDesc->samplers) {
        countElements(sampler.second, PARAM_TYPE::kSamplerState);
      }
    }

    uint32* numSlotsPerSet = reinterpret_cast<uint32*>(
                               ge_stack_alloc(m_numSets * sizeof(uint32)));
    ge_zero_out(numSlotsPerSet, m_numSets);

    for (uint32 i = 0; i < numParamDescs; ++i) {
      const SPtr<GPUParamDesc>& paramDesc = m_paramDescs[i];
      if (nullptr == paramDesc) {
        continue;
      }

      for (auto& paramBlock : paramDesc->paramBlocks) {
        numSlotsPerSet[paramBlock.second.set] = max(numSlotsPerSet[paramBlock.second.set],
                                                    paramBlock.second.slot + 1);
      }

      for (auto& texture : paramDesc->textures) {
        numSlotsPerSet[texture.second.set] = max(numSlotsPerSet[texture.second.set],
                                                 texture.second.slot + 1);
      }

      for (auto& texture : paramDesc->loadStoreTextures) {
        numSlotsPerSet[texture.second.set] = max(numSlotsPerSet[texture.second.set],
                                                 texture.second.slot + 1);
      }

      for (auto& buffer : paramDesc->buffers) {
        numSlotsPerSet[buffer.second.set] = max(numSlotsPerSet[buffer.second.set],
                                                buffer.second.slot + 1);
      }

      for (auto& sampler : paramDesc->samplers) {
        numSlotsPerSet[sampler.second.set] = max(numSlotsPerSet[sampler.second.set],
                                                 sampler.second.slot + 1);
      }
    }

    uint32 totalNumSlots = 0;
    for (uint32 i = 0; i < m_numSets; ++i) {
      totalNumSlots += numSlotsPerSet[i];
    }

    m_alloc.reserve<SetInfo>(m_numSets)
           .reserve<uint32>(totalNumSlots)
           .reserve<PARAM_TYPE::E>(totalNumSlots)
           .reserve<uint32>(totalNumSlots);

    for (uint32 i : m_numElementsPerType) {
      m_alloc.reserve<ResourceInfo>(i);
    }

    m_alloc.init();
    m_setInfos = m_alloc.alloc<SetInfo>(m_numSets);

    if (nullptr != m_setInfos) {
      ge_zero_out(m_setInfos, m_numSets);
    }

    for (uint32 i = 0; i < m_numSets; ++i) {
      m_setInfos[i].numSlots = numSlotsPerSet[i];
    }

    ge_stack_free(numSlotsPerSet);

    for (uint32 i = 0; i < m_numSets; ++i) {
      m_setInfos[i].slotIndices = m_alloc.alloc<uint32>(m_setInfos[i].numSlots);
      memset(m_setInfos[i].slotIndices,
             NumLimit::MAX_UINT32,
             sizeof(uint32) * m_setInfos[i].numSlots);

      m_setInfos[i].slotTypes = m_alloc.alloc<PARAM_TYPE::E>(m_setInfos[i].numSlots);

      m_setInfos[i].slotSamplers = m_alloc.alloc<uint32>(m_setInfos[i].numSlots);
      memset(m_setInfos[i].slotSamplers,
        NumLimit::MAX_UINT32,
        sizeof(uint32) * m_setInfos[i].numSlots);
    }

    for (uint32 i = 0; i < static_cast<uint32>(PARAM_TYPE::kCount); ++i) {
      m_resourceInfos[i] = m_alloc.alloc<ResourceInfo>(m_numElementsPerType[i]);
      m_numElementsPerType[i] = 0;
    }

    auto populateSetInfo = [&](auto& entry, PARAM_TYPE::E type)
    {
      int32 typeIdx = static_cast<int32>(type);
      uint32 sequentialIdx = m_numElementsPerType[typeIdx];

      SetInfo& setInfo = m_setInfos[entry.set];
      setInfo.slotIndices[entry.slot] = sequentialIdx;
      setInfo.slotTypes[entry.slot] = type;

      m_resourceInfos[typeIdx][sequentialIdx].set = entry.set;
      m_resourceInfos[typeIdx][sequentialIdx].slot = entry.slot;

      ++m_numElementsPerType[typeIdx];
    };

    for (uint32 i = 0; i < numParamDescs; ++i) {
      const SPtr<GPUParamDesc>& paramDesc = m_paramDescs[i];
      if (nullptr == paramDesc) {
        continue;
      }

      for (auto& paramBlock : paramDesc->paramBlocks) {
        populateSetInfo(paramBlock.second, PARAM_TYPE::kParamBlock);
      }

      for (auto& texture : paramDesc->textures) {
        populateSetInfo(texture.second, PARAM_TYPE::kTexture);
      }

      for (auto& texture : paramDesc->loadStoreTextures) {
        populateSetInfo(texture.second, PARAM_TYPE::kLoadStoreTexture);
      }

      for (auto& buffer : paramDesc->buffers) {
        populateSetInfo(buffer.second, PARAM_TYPE::kBuffer);
      }

      //Samplers need to be handled specially because certain slots could be
      //texture/buffer + sampler combinations
      {
        int32 typeIdx = static_cast<int32>(PARAM_TYPE::kSamplerState);
        for (auto& entry : paramDesc->samplers) {
          const GPUParamObjectDesc& samplerDesc = entry.second;
          uint32 sequentialIdx = m_numElementsPerType[typeIdx];

          SetInfo& setInfo = m_setInfos[samplerDesc.set];

          //Slot is sampler only
          if (setInfo.slotIndices[samplerDesc.slot] == NumLimit::MAX_UINT32) {
            setInfo.slotIndices[samplerDesc.slot] = sequentialIdx;
            setInfo.slotTypes[samplerDesc.slot] = PARAM_TYPE::kSamplerState;
          }
          else {  //Slot is a combination
            setInfo.slotSamplers[samplerDesc.slot] = sequentialIdx;
          }

          m_resourceInfos[typeIdx][sequentialIdx].set = samplerDesc.set;
          m_resourceInfos[typeIdx][sequentialIdx].slot = samplerDesc.slot;

          ++m_numElementsPerType[typeIdx];
        }
      }
    }
  }

  uint32
  GPUPipelineParamInfoBase::getSequentialSlot(PARAM_TYPE::E type,
                                              uint32 set,
                                              uint32 slot) const {
#if GE_DEBUG_MODE
    if (set >= m_numSets) {
      LOGERR("Set index out of range: Valid range: [0, " +
             toString(m_numSets) +
             "). Requested: " +
             toString(set) + ".");
      return NumLimit::MAX_UINT32;
    }

    if (slot >= m_setInfos[set].numSlots) {
      LOGERR("Slot index out of range: Valid range: [0, " +
             toString(m_setInfos[set].numSlots) +
             "). Requested: " +
             toString(slot) + ".");
      return NumLimit::MAX_UINT32;
    }

    PARAM_TYPE::E slotType = m_setInfos[set].slotTypes[slot];
    if (slotType != type) {
      //Allow sampler states & textures/buffers to share the same slot,
      //as some APIs combine them
      if (PARAM_TYPE::kSamplerState == type) {
        if (m_setInfos[set].slotSamplers[slot] != NumLimit::MAX_UINT32) {
          return m_setInfos[set].slotSamplers[slot];
        }
      }

      LOGERR("Requested parameter is not of the valid type. Requested: " +
             toString(static_cast<uint32>(type)) + ". Actual: " +
             toString(static_cast<uint32>(m_setInfos[set].slotTypes[slot])) +
             ".");
      return NumLimit::MAX_UINT32;
    }
#endif
    return m_setInfos[set].slotIndices[slot];
  }

  void
  GPUPipelineParamInfoBase::getBinding(PARAM_TYPE::E type,
                                       uint32 sequentialSlot,
                                       uint32& set,
                                       uint32& slot) const {
#if GE_DEBUG_MODE
    if (sequentialSlot >= m_numElementsPerType[static_cast<int>(type)]) {
      LOGERR("Sequential slot index out of range: Valid range: [0, " +
             toString(m_numElementsPerType[(int)type]) +
             "). Requested: " + toString(sequentialSlot) + ".");
      set = 0;
      slot = 0;
      return;
    }
#endif
    set = m_resourceInfos[static_cast<int32>(type)][sequentialSlot].set;
    slot = m_resourceInfos[static_cast<int32>(type)][sequentialSlot].slot;
  }

  void
  GPUPipelineParamInfoBase::getBindings(PARAM_TYPE::E type,
                                        const String& name,
                                       GPUParamBinding(&bindings)[GPU_PROGRAM_TYPE::kCOUNT]) {
    constexpr uint32 numParamDescs = sizeof(m_paramDescs) / sizeof(m_paramDescs[0]);
    static_assert(numParamDescs == GPU_PROGRAM_TYPE::kCOUNT,
                  "Number of param descriptor structures must match the "
                  "number of GPU program stages.");

    for (uint32 i = 0; i < numParamDescs; ++i) {
      getBinding(static_cast<GPU_PROGRAM_TYPE::E>(i), type, name, bindings[i]);
    }
  }

  void
  GPUPipelineParamInfoBase::getBinding(GPU_PROGRAM_TYPE::E progType,
                                       PARAM_TYPE::E type,
                                       const String& name,
                                       GPUParamBinding &binding) {
    auto findBinding = [](auto& paramMap, const String& name, GPUParamBinding& binding)
    {
      auto iterFind = paramMap.find(name);
      if (paramMap.end() != iterFind) {
        binding.set = iterFind->second.set;
        binding.slot = iterFind->second.slot;
      }
      else {
        binding.set = binding.slot = NumLimit::MAX_UINT32;
      }
    };

    const auto& paramDesc = m_paramDescs[static_cast<uint32>(progType)];
    if (nullptr == paramDesc) {
      binding.set = binding.slot = NumLimit::MAX_UINT32;
      return;
    }

    switch (type)
    {
      case PARAM_TYPE::kParamBlock:
        findBinding(paramDesc->paramBlocks, name, binding);
        break;
      case PARAM_TYPE::kTexture:
        findBinding(paramDesc->textures, name, binding);
        break;
      case PARAM_TYPE::kLoadStoreTexture:
        findBinding(paramDesc->loadStoreTextures, name, binding);
        break;
      case PARAM_TYPE::kBuffer:
        findBinding(paramDesc->buffers, name, binding);
        break;
      case PARAM_TYPE::kSamplerState:
        findBinding(paramDesc->samplers, name, binding);
        break;
      default:
        break;
    }
  }

  GPUPipelineParamInfo::GPUPipelineParamInfo(const GPU_PIPELINE_PARAMS_DESC& desc)
    : GPUPipelineParamInfoBase(desc)
  {}

  SPtr<GPUPipelineParamInfo>
  GPUPipelineParamInfo::create(const GPU_PIPELINE_PARAMS_DESC& desc) {
    auto paramInfo = ge_core_ptr<GPUPipelineParamInfo>
                       (GE_PVT_NEW(GPUPipelineParamInfo, desc));
    paramInfo->_setThisPtr(paramInfo);
    paramInfo->initialize();

    return paramInfo;
  }

  SPtr<geCoreThread::GPUPipelineParamInfo>
  GPUPipelineParamInfo::getCore() const {
    return static_pointer_cast<geCoreThread::GPUPipelineParamInfo>(m_coreSpecific);
  }

  SPtr<geCoreThread::CoreObject>
  GPUPipelineParamInfo::createCore() const {
    GPU_PIPELINE_PARAMS_DESC desc;
    desc.fragmentParams = m_paramDescs[GPU_PROGRAM_TYPE::kFRAGMENT_PROGRAM];
    desc.vertexParams = m_paramDescs[GPU_PROGRAM_TYPE::kVERTEX_PROGRAM];
    desc.geometryParams = m_paramDescs[GPU_PROGRAM_TYPE::kGEOMETRY_PROGRAM];
    desc.hullParams = m_paramDescs[GPU_PROGRAM_TYPE::kHULL_PROGRAM];
    desc.domainParams = m_paramDescs[GPU_PROGRAM_TYPE::kDOMAIN_PROGRAM];
    desc.computeParams = m_paramDescs[GPU_PROGRAM_TYPE::kCOMPUTE_PROGRAM];

    return geCoreThread::RenderStateManager::instance()._createPipelineParamInfo(desc);
  }

  namespace geCoreThread {
    GPUPipelineParamInfo::GPUPipelineParamInfo(const GPU_PIPELINE_PARAMS_DESC& desc,
                                               GPU_DEVICE_FLAGS::E /*deviceMask*/)
      : GPUPipelineParamInfoBase(desc)
    {}

    SPtr<GPUPipelineParamInfo>
    GPUPipelineParamInfo::create(const GPU_PIPELINE_PARAMS_DESC& desc,
                                 GPU_DEVICE_FLAGS::E deviceMask) {
      return RenderStateManager::instance().createPipelineParamInfo(desc, deviceMask);
    }
  }
}
