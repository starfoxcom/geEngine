/*****************************************************************************/
/**
 * @file    geIndexBuffer.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2018/06/08
 * @brief   Hardware buffer that hold indices of vertices in a vertex buffer.
 *
 * Hardware buffer that hold indices of vertices in a vertex buffer.
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
#include "geHardwareBuffer.h"
#include "geCoreObject.h"

namespace geEngineSDK {
  /**
   * @brief Descriptor structure used for initialization of an IndexBuffer.
   */
  struct INDEX_BUFFER_DESC
  {
    /**
     * @brief Index type, determines the size of a single index.
     */
    IndexType indexType;

    /**
     * @brief Number of indices can buffer can hold.
     */
    uint32 numIndices;

    /**
     * @brief Usage that tells the hardware how will be buffer be used.
     */
    GPU_BUFFER_USAGE::E usage = GPU_BUFFER_USAGE::kSTATIC;
  };

  /**
   * @brief Contains information about an index buffer.
   */
  class GE_CORE_EXPORT IndexBufferProperties
  {
   public:
    IndexBufferProperties(IndexType idxType, uint32 numIndexes);

    /**
     * @brief Returns the type of indices stored.
     */
    IndexType
    getType() const {
      return mIndexType;
    }

    /**
     * @brief Returns the number of indices this buffer can hold.
     */
    uint32
    getNumIndices() const {
      return mNumIndices;
    }

    /**
     * @brief Returns the size of a single index in bytes.
     */
    uint32
    getIndexSize() const {
      return mIndexSize;
    }

   protected:
    friend class IndexBuffer;
    friend class geCoreThread::IndexBuffer;

    IndexType mIndexType;
    uint32 mNumIndices;
    uint32 mIndexSize;
  };

  /**
   * @brief Hardware buffer that hold indices that reference vertices in a
   *        vertex buffer.
   */
  class GE_CORE_EXPORT IndexBuffer : public CoreObject
  {
   public:
    virtual ~IndexBuffer() = default;

    /**
     * @brief Returns information about the index buffer.
     */
    const IndexBufferProperties&
    getProperties() const {
      return mProperties;
    }

    /**
     * @brief Retrieves a core implementation of an index buffer usable only
     *        from the core thread.
     * @note  Core thread only.
     */
    SPtr<geCoreThread::IndexBuffer>
    getCore() const;

    /**
     * @copydoc HardwareBufferManager::createIndexBuffer
     */
    static SPtr<IndexBuffer>
    create(const INDEX_BUFFER_DESC& desc);

   protected:
    friend class HardwareBufferManager;

    IndexBuffer(const INDEX_BUFFER_DESC& desc);

    /**
     *@copydoc CoreObject::createCore
     */
    SPtr<geCoreThread::CoreObject>
    createCore() const override;

    IndexBufferProperties mProperties;
    GPU_BUFFER_USAGE::E mUsage;
  };

  namespace geCoreThread {
    /**
     * @brief Core thread specific implementation of an bs::IndexBuffer.
     */
    class GE_CORE_EXPORT IndexBuffer
      : public CoreObject, public HardwareBuffer
    {
     public:
      IndexBuffer(const INDEX_BUFFER_DESC& desc,
        GPU_DEVICE_FLAGS::E deviceMask = GPU_DEVICE_FLAGS::kDEFAULT);
      virtual ~IndexBuffer() = default;

      /**
       * @brief Returns information about the index buffer.
       */
      const IndexBufferProperties&
      getProperties() const {
        return mProperties;
      }

      /**
       * @copydoc HardwareBufferManager::createIndexBuffer
       */
      static SPtr<IndexBuffer>
      create(const INDEX_BUFFER_DESC& desc,
             GPU_DEVICE_FLAGS::E deviceMask = GPU_DEVICE_FLAGS::kDEFAULT);

     protected:
      IndexBufferProperties mProperties;
    };
  }
}
