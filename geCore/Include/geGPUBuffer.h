/*****************************************************************************/
/**
 * @file    geGPUBuffer.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2018/06/08
 * @brief   Handles a generic GPU buffer.
 *
 * Handles a generic GPU buffer that you may use for storing any kind of data.
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
#include "geHardwareBuffer.h"

namespace geEngineSDK {
  /**
   * @brief Descriptor structure used for initialization of a GPUBuffer.
   */
  struct GPU_BUFFER_DESC
  {
    /**
     * @brief Number of elements in the buffer.
     */
    uint32 elementCount;

    /**
     * @brief Size of each individual element in the buffer, in bytes. Only needed if using non-standard buffer. If using standard buffers element size is calculated from format and this must be zero.
     */
    uint32 elementSize;

    /**
     * @brief Type of the buffer. Determines how is buffer seen by the GPU program and in what ways can it be used.
     */
    GPU_BUFFER_TYPE::E type;

    /**
     * @brief Format if the data in the buffer. Only relevant for standard buffers, must be BF_UNKNOWN otherwise.
     */
    GPU_BUFFER_FORMAT::E format;

    /**
     * @brief Usage that tells the hardware how will be buffer be used.
     */
    GPU_BUFFER_USAGE::E usage = GPU_BUFFER_USAGE::kSTATIC;

    /**
     * @brief When true allows the GPU to write to the resource. Must be enabled if buffer type is GBT_APPENDCONSUME.
     */
    bool randomGPUWrite = false;

    /**
     * @brief When true binds a counter that can be used from a GPU program on the buffer. Can only be used in combination with GBT_STRUCTURED and randomGPUWrite must be enabled.
     */
    bool useCounter = false;
  };

  /**
  * @brief Information about a GPUBuffer. Allows core and non-core versions of GPUBuffer to share the same structure for properties.
  */
  class GE_CORE_EXPORT GPUBufferProperties
  {
  public:
    GPUBufferProperties(const GPU_BUFFER_DESC& desc);

    /**
    * @brief Returns the type of the GPU buffer. Type determines which kind of views (if any) can be created for the buffer, and how is data read or modified in it.
    */
    GPU_BUFFER_TYPE::E getType() const { return mDesc.type; }

    /**
     * @brief Returns format used by the buffer. Only relevant for standard buffers.
     */
    GPU_BUFFER_FORMAT::E getFormat() const { return mDesc.format; }

    /**
     * @brief Returns buffer usage which determines how are planning on updating the buffer contents.
     */
    GPU_BUFFER_USAGE::E getUsage() const { return mDesc.usage; }

    /**
     * @brief Return whether the buffer supports random reads and writes within the GPU programs.
     */
    bool getRandomGPUWrite() const { return mDesc.randomGPUWrite; }

    /**
     * @brief Returns whether the buffer supports counter use within GPU programs.
     */
    bool getUseCounter() const { return mDesc.useCounter; }

    /**
     * @brief Returns number of elements in the buffer.
     */
    uint32 getElementCount() const { return mDesc.elementCount; }

    /**
     * @brief Returns size of a single element in the buffer in bytes.
     */
    uint32 getElementSize() const { return mDesc.elementSize; }

   protected:
    friend class GPUBuffer;

    GPU_BUFFER_DESC mDesc;
  };

  /**
  * @brief Handles a generic GPU buffer that you may use for storing any kind of data you wish to be accessible to the GPU.
  * These buffers may be bounds to GPU program binding slots and accessed from a GPU program, or may be used by fixed pipeline in some way.
  *
  * Buffer types:
  *  - Raw buffers containing a block of bytes that are up to the GPU program to interpret.
  *	- Structured buffer containing an array of structures compliant to a certain layout. Similar to raw buffer but
  *    easier to interpret the data.
  *	- Random read/write buffers that allow you to write to random parts of the buffer from within the GPU program, and
  *    then read it later. These can only be bound to pixel and compute stages.
  *	- Append/Consume buffers also allow you to write to them, but in a stack-like fashion, usually where one set of
  *    programs produces data while other set consumes it from the same buffer. Append/Consume buffers are structured
  *	  by default.
  *
  * @note	Sim thread only.
  */
  class GE_CORE_EXPORT GPUBuffer : public CoreObject
  {
  public:
    virtual ~GPUBuffer() { }

    /**
     * @brief Returns properties describing the buffer.
     */
    const GPUBufferProperties& getProperties() const { return mProperties; }

    /**
     * @brief Retrieves a core implementation of a GPU buffer usable only from the core thread.
     */
    SPtr<geCoreThread::GPUBuffer> getCore() const;

    /**
     * @brief Returns the size of a single element in the buffer, of the provided format, in bytes.
     */
    static uint32 getFormatSize(GPU_BUFFER_FORMAT::E format);

    /**
     * @copydoc HardwareBufferManager::createGPUBuffer
     */
    static SPtr<GPUBuffer> create(const GPU_BUFFER_DESC& desc);

   protected:
    friend class HardwareBufferManager;

    GPUBuffer(const GPU_BUFFER_DESC& desc);

    /**
     * @copydoc CoreObject::createCore
     */
    SPtr<geCoreThread::CoreObject> createCore() const override;

    GPUBufferProperties mProperties;
  };

  namespace geCoreThread {
    /**
     * @brief Core thread version of a bs::GPUBuffer.
     * @note  Core thread only.
     */
    class GE_CORE_EXPORT GPUBuffer : public CoreObject, public HardwareBuffer
    {
     public:
      virtual ~GPUBuffer();

      /**
       * @brief Returns properties describing the buffer.
       */
      const GPUBufferProperties& getProperties() const { return mProperties; }

      /**
       * @copydoc HardwareBufferManager::createGPUBuffer
       */
      static SPtr<GPUBuffer> create(const GPU_BUFFER_DESC& desc, GPU_DEVICE_FLAGS::E deviceMask = GPU_DEVICE_FLAGS::kDEFAULT);

     protected:
      GPUBuffer(const GPU_BUFFER_DESC& desc, uint32 deviceMask);

      GPUBufferProperties mProperties;
    };
  }
}
