/*****************************************************************************/
/**
 * @file    geGPUResourceData.cpp
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

/*****************************************************************************/
/**
 * Includes
 */
/*****************************************************************************/
#include "geGPUResourceData.h"
#include "geGPUResourceDataRTTI.h"
#include "geCoreThread.h"

#include <geException.h>

namespace geEngineSDK {
  GPUResourceData::GPUResourceData(const GPUResourceData& copy) {
    m_data = copy.m_data;
    
    //TODO: This should be shared by all copies pointing to the same data?
    m_locked = copy.m_locked;
    m_ownsData = false;
  }

  GPUResourceData::~GPUResourceData() {
    freeInternalBuffer();
  }

  GPUResourceData&
  GPUResourceData::operator=(const GPUResourceData& rhs) {
    m_data = rhs.m_data;
    
    //TODO: This should be shared by all copies pointing to the same data?
    m_locked = rhs.m_locked;
    m_ownsData = false;

    return *this;
  }

  uint8*
  GPUResourceData::getData() const {
# if !GE_FORCE_SINGLETHREADED_RENDERING
    if (m_locked) {
      if (GE_THREAD_CURRENT_ID != CoreThread::instance().getCoreThreadId()) {
        GE_EXCEPT(InternalErrorException,
                  "You are not allowed to access buffer data from non-core "
                  "thread when the buffer is locked.");
      }
    }
# endif
    return m_data;
  }

  void
  GPUResourceData::allocateInternalBuffer() {
    allocateInternalBuffer(getInternalBufferSize());
  }

  void
  GPUResourceData::allocateInternalBuffer(uint32 size) {
# if !GE_FORCE_SINGLETHREADED_RENDERING
    if (m_locked) {
      if (GE_THREAD_CURRENT_ID != CoreThread::instance().getCoreThreadId()) {
        GE_EXCEPT(InternalErrorException,
                  "You are not allowed to access buffer data from non-core "
                  "thread when the buffer is locked.");
      }
    }
# endif
    freeInternalBuffer();

    m_data = reinterpret_cast<uint8*>(ge_alloc(size));
    m_ownsData = true;
  }

  void
  GPUResourceData::freeInternalBuffer() {
    if (nullptr == m_data || !m_ownsData) {
      return;
    }

# if !GE_FORCE_SINGLETHREADED_RENDERING
    if (m_locked) {
      if (GE_THREAD_CURRENT_ID != CoreThread::instance().getCoreThreadId())
        GE_EXCEPT(InternalErrorException,
                  "You are not allowed to access buffer data from non-core "
                  "thread when the buffer is locked.");
    }
# endif
    ge_free(m_data);
    m_data = nullptr;
  }

  void
  GPUResourceData::setExternalBuffer(uint8* data) {
# if !GE_FORCE_SINGLETHREADED_RENDERING
    if (m_locked) {
      if (GE_THREAD_CURRENT_ID != CoreThread::instance().getCoreThreadId()) {
        GE_EXCEPT(InternalErrorException,
                  "You are not allowed to access buffer data from non-core "
                  "thread when the buffer is locked.");
      }
    }
# endif
    freeInternalBuffer();

    m_data = data;
    m_ownsData = false;
  }

  void
  GPUResourceData::_lock() const {
    m_locked = true;
  }

  void
  GPUResourceData::_unlock() const {
    m_locked = false;
  }

  /***************************************************************************/
  /**
   * Seriualization
   */
  /***************************************************************************/

  RTTITypeBase*
  GPUResourceData::getRTTIStatic() {
    return GPUResourceDataRTTI::instance();
  }

  RTTITypeBase*
  GPUResourceData::getRTTI() const {
    return GPUResourceData::getRTTIStatic();
  }
}
