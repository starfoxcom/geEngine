/*****************************************************************************/
/**
 * @file    geGPUPipelineParamInfo.h
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
#pragma once

/*****************************************************************************/
/**
 * Includes
 */
/*****************************************************************************/
#include "gePrerequisitesCore.h"
#include "geCoreObject.h"

#include <geGroupAlloc.h>
#include <geNumericLimits.h>

namespace geEngineSDK {
  using std::array;

  /**
   * @brief Helper structure used for initializing GPUPipelineParamInfo.
   */
  struct GPU_PIPELINE_PARAMS_DESC
  {
    SPtr<GPUParamDesc> fragmentParams;
    SPtr<GPUParamDesc> vertexParams;
    SPtr<GPUParamDesc> geometryParams;
    SPtr<GPUParamDesc> hullParams;
    SPtr<GPUParamDesc> domainParams;
    SPtr<GPUParamDesc> computeParams;
  };

  /**
   * @brief Binding location for a single GPU program parameter.
   */
  struct GPUParamBinding
  {
    uint32 set = NumLimit::MAX_UINT32;
    uint32 slot = NumLimit::MAX_UINT32;
  };

  /**
  * @brief Types of GPU parameters.
  */
  namespace PARAM_TYPE {
    enum E {
      kParamBlock,
      kTexture,
      kLoadStoreTexture,
      kBuffer,
      kSamplerState,
      kCount
    };
  }

  /**
   * @brief Contains code common to both sim and core thread implementations
   *        of GPUPipelineParamInfo.
   */
  class GE_CORE_EXPORT GPUPipelineParamInfoBase
  {
   public:
    /**
     * @brief Constructs the object using the provided GPU parameter
     *        descriptors.
     */
    GPUPipelineParamInfoBase(const GPU_PIPELINE_PARAMS_DESC& desc);
    virtual ~GPUPipelineParamInfoBase() = default;

    /**
     * @brief Gets the total number of sets.
     */
    uint32
    getNumSets() const {
      return m_numSets;
    }

    /**
     * @brief Returns the total number of elements across all sets.
     */
    uint32
    getNumElements() const {
      return m_numElements;
    }

    /**
     * @brief Returns the number of elements in all sets for the specified
     *        parameter type.
     */
    uint32
    getNumElements(PARAM_TYPE::E type) {
      return m_numElementsPerType[static_cast<int32>(type)];
    }

    /**
     * @brief Converts a set/slot combination into a sequential index that maps
     *        to the parameter in that parameter type's array.
     *        If the set or slot is out of valid range, the method logs an
     *        error and returns MAX_UINT32.
     *        Only performs range checking in debug mode.
     */
    uint32
    getSequentialSlot(PARAM_TYPE::E type,
                      uint32 set,
                      uint32 slot) const;

    /**
     * @brief Converts a sequential slot index into a set/slot combination.
     */
    void
    getBinding(PARAM_TYPE::E type,
               uint32 sequentialSlot,
               uint32& set,
               uint32& slot) const;

    /**
     * @brief Finds set/slot indices of a parameter with the specified name for
     *        the specified GPU program stage.
     *        Set/slot indices are set to -1 if a stage doesn't have a block
     *        with the specified name.
     */
    void
    getBinding(GPU_PROGRAM_TYPE::E progType,
               PARAM_TYPE::E type,
               const String& name,
               GPUParamBinding &binding);

    /**
     * @brief Finds set/slot indices of a parameter with the specified name for
     *        every GPU program stage. Set/slot indices are set to -1 if a
     *        stage doesn't have a block with the specified name.
     */
    void
    getBindings(PARAM_TYPE::E type,
                const String& name,
                GPUParamBinding(&bindings)[GPU_PROGRAM_TYPE::kCOUNT]);

    /**
     * @brief Returns descriptions of individual parameters for the specified
     *        GPU program type.
     */
    const SPtr<GPUParamDesc>&
    getParamDesc(GPU_PROGRAM_TYPE::E type) const {
      return m_paramDescs[static_cast<int32>(type)];
    }

   protected:
    /**
     * @brief Information about a single set in the param info object.
     */
    struct SetInfo
    {
      uint32* slotIndices;
      PARAM_TYPE::E* slotTypes;
      uint32* slotSamplers;
      uint32 numSlots;
    };

    /**
     * @brief Information how a resource maps to a certain set/slot.
     */
    struct ResourceInfo
    {
      uint32 set;
      uint32 slot;
    };

    array<SPtr<GPUParamDesc>, 6> m_paramDescs;

    uint32 m_numSets;
    uint32 m_numElements;
    SetInfo* m_setInfos;
    uint32 m_numElementsPerType[static_cast<int32>(PARAM_TYPE::kCount)];
    ResourceInfo* m_resourceInfos[static_cast<int32>(PARAM_TYPE::kCount)];

    GroupAlloc m_alloc;
  };

  /**
   * @brief Holds meta-data about a set of GPU parameters used by a single
   *        pipeline state.
   */
  class GE_CORE_EXPORT GPUPipelineParamInfo
    : public CoreObject, public GPUPipelineParamInfoBase
  {
   public:
    virtual ~GPUPipelineParamInfo() = default;

    /**
     * @brief Retrieves a core implementation of this object usable only from
     *        the core thread.
     * @note  Core thread only.
     */
    SPtr<geCoreThread::GPUPipelineParamInfo>
    getCore() const;

    /**
     * @brief Constructs the object using the provided GPU parameter
     *        descriptors.
     * @param[in] desc  Object containing parameter descriptions for
     *                  individual GPU program stages.
     */
    static SPtr<GPUPipelineParamInfo>
    create(const GPU_PIPELINE_PARAMS_DESC& desc);

   private:
    GPUPipelineParamInfo(const GPU_PIPELINE_PARAMS_DESC& desc);

    /**
     * @copydoc CoreObjegeCoreThread::createCore
     */
    SPtr<geCoreThread::CoreObject>
    createCore() const override;
  };

  namespace geCoreThread {
    /**
     * @brief Core thread version of a geEngineSDK::GPUPipelineParamInfo.
     */
    class GE_CORE_EXPORT GPUPipelineParamInfo
      : public CoreObject, public GPUPipelineParamInfoBase
    {
     public:
      virtual ~GPUPipelineParamInfo() = default;

      /**
       * @copydoc geEngineSDK::GPUPipelineParamInfo::create
       * @param[in] deviceMask  Mask that determines on which GPU devices
       *                        should the buffer be created on.
       */
      static SPtr<GPUPipelineParamInfo>
      create(const GPU_PIPELINE_PARAMS_DESC& desc,
             GPU_DEVICE_FLAGS::E deviceMask = GPU_DEVICE_FLAGS::kDEFAULT);

     protected:
      friend class RenderStateManager;

      GPUPipelineParamInfo(const GPU_PIPELINE_PARAMS_DESC& desc,
                           GPU_DEVICE_FLAGS::E deviceMask);
    };
  }
}
