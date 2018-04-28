/*****************************************************************************/
/**
 * @file    geGPUResourceData.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2017/11/23
 * @brief   Storage class for reading/writing from/to various GPU resources.
 *
 * You can use this class as a storage for reading and writing from/to various
 * GPU resources. It is meant to be created on sim thread and used on the core
 * thread. This class is abstract and specific resource types need to implement
 * their own type of GPUResourceData.
 *
 * @note Normal use of this class involves requesting an instance of
 *       GPUResourceData from a Resource, then scheduling a read or write on
 *       that resource using the provided instance. Instance will be locked
 *       while it is used by the core thread and sim thread will be allowed to
 *       access it when the operation ends. Caller can track AsyncOp's
 *       regarding the read/write operation to be notified when it is complete.
 * @note If you allocate an internal buffer to store the resource data, the
 *       ownership of the buffer will always remain with the initial instance
 *       of the class. If that initial instance is deleted, any potential
 *       copies will point to garbage data.
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
  class GE_CORE_EXPORT GPUResourceData : public IReflectable
  {
   public:
    GPUResourceData() = default;
    GPUResourceData(const GPUResourceData& copy);
    virtual ~GPUResourceData();

    GPUResourceData&
    operator=(const GPUResourceData& rhs);

    /**
     * @brief Returns pointer to the internal buffer.
     */
    uint8*
    getData() const;

    /**
     * @brief Allocates an internal buffer of a certain size. If there is
     *        another buffer already allocated, it will be freed and new one
     *        will be allocated. Buffer size is determined based on parameters
     *        used for initializing the class.
     */
    void
    allocateInternalBuffer();

    /**
     * @brief Allocates an internal buffer of a certain size. If there is
     *        another buffer already allocated, it will be freed and new one
     *        will be allocated.
     * @param[in] size  The size of the buffer in bytes.
     */
    void
    allocateInternalBuffer(uint32 size);

    /**
     * @brief Frees the internal buffer that was allocated using
     *        allocateInternalBuffer(). Called automatically when the instance
     *        of the class is destroyed.
     */
    void
    freeInternalBuffer();

    /**
     * @brief Makes the internal data pointer point to some external data. No
     *        copying is done, so you must ensure that external data exists as
     *        long as this class uses it. You are also responsible for deleting
     *        the data when you are done with it.
     * @note  If any internal data is allocated, it is freed.
     */
    void
    setExternalBuffer(uint8* data);

    /**
     * @brief Checks if the internal buffer is locked due to some other thread
     *        using it.
     */
    bool
    isLocked() const {
      return m_locked;
    }

    /**
     * @brief Locks the data and makes it available only to the core thread.
     */
    void
    _lock() const;

    /**
     * @brief Unlocks the data and makes it available to all threads.
     */
    void
    _unlock() const;

   protected:
    /**
     * @brief Returns the size of the internal buffer in bytes. This is
     *        calculated based on parameters provided upon construction and
     *        specific implementation details.
     */
    virtual uint32
    getInternalBufferSize() const = 0;

   private:
    uint8* m_data = nullptr;
    bool m_ownsData = false;
    mutable bool m_locked = false;

    /*************************************************************************/
    /**
     * Serialization
     */
    /*************************************************************************/
  public:
    friend class GPUResourceDataRTTI;

    static RTTITypeBase*
    getRTTIStatic();

    virtual RTTITypeBase*
    getRTTI() const override;
  };
}
