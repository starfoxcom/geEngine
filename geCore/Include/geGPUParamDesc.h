/*****************************************************************************/
/**
 * @file    geGPUParamDesc.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2018/06/10
 * @brief   Describes a single GPU program data parameter.
 *
 * Describes a single GPU program data (i.e. int, float, Vector2) parameter.
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

namespace geEngineSDK {
  /**
   * @brief Describes a single GPU program data (for example int, float,
   *        Vector2) parameter.
   */
  struct GPUParamDataDesc
  {
    String name;
    
    /**
     * In multiples of 4 bytes.
     */
    uint32 elementSize;

    uint32 arraySize;
    
    /**
     * In multiples of 4 bytes.
     */
    uint32 arrayElementStride;

    GPU_PARAM_DATA_TYPE::E type;

    uint32 paramBlockSlot;
    uint32 paramBlockSet;

    /**
     * In multiples of 4 bytes, or index for parameters not in a buffer.
     */
    uint32 gpuMemOffset;

    /**
     * In multiples of 4 bytes.
     */
    uint32 cpuMemOffset;
  };

  /**
   * @brief Describes a single GPU program object (for example texture,
   *        sampler state) parameter.
   */
  struct GPUParamObjectDesc
  {
    String name;
    GPU_PARAM_OBJECT_TYPE::E type;

    /**
     * Slot within a set. Uniquely identifies bind location in the GPU
     * pipeline, together with the set.
     */
    uint32 slot;

    /**
     * Uniquely identifies the bind location in the GPU pipeline, together
     * with the slot.
     */
    uint32 set;
  };

  /**
   * @brief Describes a GPU program parameter block (collection of GPU program
   *        data parameters).
   */
  struct GPUParamBlockDesc
  {
    String name;

    /**
     * Slot within a set. Uniquely identifies bind location in the GPU
     * pipeline, together with the set.
     */
    uint32 slot;

    /**
     * Uniquely identifies the bind location in the GPU pipeline, together
     * with the slot.
     */
    uint32 set;

    /**
     * In multiples of 4 bytes.
     */
    uint32 blockSize;

    /**
     * True for blocks that can be shared between different GPU pipeline
     * stages.
     */
    bool isShareable;
  };

  /**
   * @brief Contains all parameter information for a GPU program, including
   *        data and object parameters, plus parameter blocks.
   */
  struct GE_CORE_EXPORT GPUParamDesc : IReflectable
  {
    Map<String, GPUParamBlockDesc> paramBlocks;
    Map<String, GPUParamDataDesc> params;

    Map<String, GPUParamObjectDesc> samplers;
    Map<String, GPUParamObjectDesc> textures;
    Map<String, GPUParamObjectDesc> loadStoreTextures;
    Map<String, GPUParamObjectDesc> buffers;

    /*************************************************************************/
    /**
     * RTTI
     */
    /*************************************************************************/

   public:
    friend class GPUParamDescRTTI;

    static RTTITypeBase*
    getRTTIStatic();

    RTTITypeBase*
    getRTTI() const override;
  };

  template<>
  struct RTTIPlainType<GPUParamDataDesc>
  {
    enum { kID = TYPEID_CORE::kID_GPUParamDataDesc };
    enum { kHasDynamicSize = 1 };
    static constexpr uint32 VERSION = 1;

    static void
    toMemory(const GPUParamDataDesc& data, char* memory) {
      uint32 size = getDynamicSize(data);

      uint32 curSize = sizeof(uint32);
      memcpy(memory, &size, curSize);
      memory += curSize;

      memory = rttiWriteElement(VERSION, memory);

      memory = rttiWriteElement(data.name, memory);
      memory = rttiWriteElement(data.elementSize, memory);
      memory = rttiWriteElement(data.arraySize, memory);
      memory = rttiWriteElement(data.arrayElementStride, memory);
      memory = rttiWriteElement(data.type, memory);

      memory = rttiWriteElement(data.paramBlockSlot, memory);
      memory = rttiWriteElement(data.paramBlockSet, memory);
      memory = rttiWriteElement(data.gpuMemOffset, memory);
      rttiWriteElement(data.cpuMemOffset, memory);
    }

    static uint32
    fromMemory(GPUParamDataDesc& data, char* memory) {
      uint32 size;
      memcpy(&size, memory, sizeof(uint32));
      memory += sizeof(uint32);

      uint32 version = 0;
      memory = rttiReadElement(version, memory);
      GE_ASSERT(VERSION == version);

      memory = rttiReadElement(data.name, memory);
      memory = rttiReadElement(data.elementSize, memory);
      memory = rttiReadElement(data.arraySize, memory);
      memory = rttiReadElement(data.arrayElementStride, memory);
      memory = rttiReadElement(data.type, memory);

      memory = rttiReadElement(data.paramBlockSlot, memory);
      memory = rttiReadElement(data.paramBlockSet, memory);
      memory = rttiReadElement(data.gpuMemOffset, memory);
      rttiReadElement(data.cpuMemOffset, memory);

      return size;
    }

    static uint32
    getDynamicSize(const GPUParamDataDesc& data) {
      uint32 dataSize = rttiGetElementSize(VERSION) +
                        rttiGetElementSize(data.name) +
                        rttiGetElementSize(data.elementSize) +
                        rttiGetElementSize(data.arraySize) +
                        rttiGetElementSize(data.arrayElementStride) +
                        rttiGetElementSize(data.type) +
                        rttiGetElementSize(data.paramBlockSlot) +
                        rttiGetElementSize(data.paramBlockSet) +
                        rttiGetElementSize(data.gpuMemOffset) +
                        rttiGetElementSize(data.cpuMemOffset) +
                        sizeof(uint32);

      return dataSize;
    }
  };

  template<>
  struct RTTIPlainType<GPUParamObjectDesc>
  {
    enum { kID = TYPEID_CORE::kID_GPUParamObjectDesc };
    enum { kHasDynamicSize = 1 };
    static constexpr uint32 VERSION = 1;

    static void
    toMemory(const GPUParamObjectDesc& data, char* memory) {
      uint32 size = getDynamicSize(data);

      uint32 curSize = sizeof(uint32);
      memcpy(memory, &size, curSize);
      memory += curSize;

      memory = rttiWriteElement(VERSION, memory);

      memory = rttiWriteElement(data.name, memory);
      memory = rttiWriteElement(data.type, memory);
      memory = rttiWriteElement(data.slot, memory);
      rttiWriteElement(data.set, memory);
    }

    static uint32
    fromMemory(GPUParamObjectDesc& data, char* memory) {
      uint32 size;
      memcpy(&size, memory, sizeof(uint32));
      memory += sizeof(uint32);

      uint32 version = 0;
      memory = rttiReadElement(version, memory);
      GE_ASSERT(VERSION == version);

      memory = rttiReadElement(data.name, memory);
      memory = rttiReadElement(data.type, memory);
      memory = rttiReadElement(data.slot, memory);
      rttiReadElement(data.set, memory);

      return size;
    }

    static uint32
    getDynamicSize(const GPUParamObjectDesc& data) {
      uint32 dataSize = rttiGetElementSize(VERSION) +
                        rttiGetElementSize(data.name) +
                        rttiGetElementSize(data.type) +
                        rttiGetElementSize(data.slot) +
                        rttiGetElementSize(data.set) +
                        sizeof(uint32);

      return dataSize;
    }
  };

  template<>
  struct RTTIPlainType<GPUParamBlockDesc>
  {
    enum { kID = TYPEID_CORE::kID_GPUParamBlockDesc };
    enum { kHasDynamicSize = 1 };
    static constexpr uint32 VERSION = 1;

    static void
    toMemory(const GPUParamBlockDesc& data, char* memory) {
      uint32 size = getDynamicSize(data);

      uint32 curSize = sizeof(uint32);
      memcpy(memory, &size, curSize);
      memory += curSize;

      memory = rttiWriteElement(VERSION, memory);

      memory = rttiWriteElement(data.name, memory);
      memory = rttiWriteElement(data.set, memory);
      memory = rttiWriteElement(data.slot, memory);
      memory = rttiWriteElement(data.blockSize, memory);
      rttiWriteElement(data.isShareable, memory);
    }

    static uint32
    fromMemory(GPUParamBlockDesc& data, char* memory) {
      uint32 size;
      memcpy(&size, memory, sizeof(uint32));
      memory += sizeof(uint32);

      uint32 version = 0;
      memory = rttiReadElement(version, memory);
      GE_ASSERT(VERSION == version);

      memory = rttiReadElement(data.name, memory);
      memory = rttiReadElement(data.set, memory);
      memory = rttiReadElement(data.slot, memory);
      memory = rttiReadElement(data.blockSize, memory);
      rttiReadElement(data.isShareable, memory);

      return size;
    }

    static uint32
    getDynamicSize(const GPUParamBlockDesc& data) {
      uint32 dataSize = rttiGetElementSize(VERSION) +
                        rttiGetElementSize(data.name) +
                        rttiGetElementSize(data.set) +
                        rttiGetElementSize(data.slot) +
                        rttiGetElementSize(data.blockSize) +
                        rttiGetElementSize(data.isShareable) +
                        sizeof(uint32);

      return dataSize;
    }
  };
}
