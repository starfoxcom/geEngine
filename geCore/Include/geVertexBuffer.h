/*****************************************************************************/
/**
 * @file    geVertexBuffer.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2018/06/08
 * @brief   Specialization of a hardware buffer used for holding vertex data.
 *
 * Specialization of a hardware buffer used for holding vertex data.
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
   * @brief Descriptor structure used for initialization of a VertexBuffer.
   */
  struct VERTEX_BUFFER_DESC
  {
    /**
     * Size of a single vertex in the buffer, in bytes.
     */
    uint32 vertexSize;

    /**
     * Number of vertices the buffer can hold.
     */
    uint32 numVerts;

    /**
     * Usage that tells the hardware how will the buffer be used.
     */
    GPU_BUFFER_USAGE::E usage = GPU_BUFFER_USAGE::kSTATIC;

    /**
     * If true the buffer will be usable for streaming out data from the GPU.
     */
    bool streamOut = false;
  };

  /**
   * @brief Contains information about a vertex buffer.
   */
  class GE_CORE_EXPORT VertexBufferProperties
  {
   public:
    VertexBufferProperties(uint32 numVertices, uint32 vertexSize);

    /**
     * @brief Gets the size in bytes of a single vertex in this buffer.
     */
    uint32
    getVertexSize() const {
      return m_vertexSize;
    }

    /**
     * @brief Get the number of vertices in this buffer.
     */
    uint32
    getNumVertices() const {
      return m_numVertices;
    }

   protected:
    friend class VertexBuffer;
    friend class geCoreThread::VertexBuffer;

    uint32 m_numVertices;
    uint32 m_vertexSize;
  };

  /**
   * @brief Specialization of a hardware buffer used for holding vertex data.
   */
  class GE_CORE_EXPORT VertexBuffer : public CoreObject
  {
   public:
    virtual ~VertexBuffer() = default;

    /**
     * @brief Retrieves a core implementation of a vertex buffer usable only
     *        from the core thread.
     * @note  Core thread only.
     */
    SPtr<geCoreThread::VertexBuffer>
    getCore() const;

    /**
     * @copydoc HardwareBufferManager::createVertexBuffer
     */
    static SPtr<VertexBuffer>
    create(const VERTEX_BUFFER_DESC& desc);

    static constexpr int32 MAX_SEMANTIC_IDX = 8;

   protected:
    friend class HardwareBufferManager;

    VertexBuffer(const VERTEX_BUFFER_DESC& desc);

    /**
     * @copydoc CoreObject::createCore
     */
    virtual SPtr<geCoreThread::CoreObject>
    createCore() const;

    VertexBufferProperties m_properties;
    GPU_BUFFER_USAGE::E m_usage;
    bool m_streamOut;
  };

  namespace geCoreThread {
    /**
     * @brief Core thread specific implementation of a
     *        geEngineSDK::VertexBuffer.
     */
    class GE_CORE_EXPORT VertexBuffer
      : public CoreObject, public HardwareBuffer {
     public:
      VertexBuffer(const VERTEX_BUFFER_DESC& desc,
                   GPU_DEVICE_FLAGS::E deviceMask =
                     GPU_DEVICE_FLAGS::kDEFAULT);

      virtual ~VertexBuffer() = default;

      /**
       * @brief Returns information about the vertex buffer.
       */
      const VertexBufferProperties&
      getProperties() const {
        return m_properties;
      }

      /**
       * @copydoc HardwareBufferManager::createVertexBuffer
       */
      static SPtr<VertexBuffer>
      create(const VERTEX_BUFFER_DESC& desc,
             GPU_DEVICE_FLAGS::E deviceMask = GPU_DEVICE_FLAGS::kDEFAULT);

     protected:
      VertexBufferProperties m_properties;
    };
  }
}
