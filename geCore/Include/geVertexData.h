/*****************************************************************************/
/**
 * @file    geVertexData.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2018/06/09
 * @brief   Container consisting of a set of v-buffers and their declaration.
 *
 * Container class consisting of a set of vertex buffers and their declaration.
 *
 * @note    Used just for more easily passing around vertex information..
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
#include "geVertexDeclaration.h"
#include "geVertexBuffer.h"

namespace geEngineSDK {
  namespace geCoreThread {
    /**
     * @brief Container class consisting of a set of vertex buffers and their
     *        declaration.
     * @note  Used just for more easily passing around vertex information.
     */
    class GE_CORE_EXPORT VertexData final
    {
     public:
      VertexData() = default;
      ~VertexData() = default;

      /**
       * @brief Assigns a new vertex buffer to the specified index.
       */
      void
      setBuffer(uint32 index, SPtr<VertexBuffer> buffer);

      /**
       * @brief Retrieves a vertex buffer from the specified index.
       */
      SPtr<VertexBuffer>
      getBuffer(uint32 index) const;

      /**
       * @brief Returns a list of all bound vertex buffers.
       */
      const UnorderedMap<uint32, SPtr<VertexBuffer>>&
      getBuffers() const {
        return m_vertexBuffers;
      }

      /**
       * @brief Checks if there is a buffer at the specified index.
       */
      bool
      isBufferBound(uint32 index) const;

      /**
       * @brief Gets total number of bound buffers.
       */
      uint32
      getBufferCount() const {
        return static_cast<uint32>(m_vertexBuffers.size());
      }

      /**
       * @brief Returns the maximum index of all bound buffers.
       */
      uint32
      getMaxBufferIndex() const {
        return m_maxBufferIdx;
      }

      /**
       * @brief Declaration used for the contained vertex buffers.
       */
      SPtr<VertexDeclaration> m_vertexDeclaration;

      /**
      * @brief Number of vertices to use.
      */
      uint32 m_vertexCount = 0;

     private:
      void recalculateMaxIndex();

      UnorderedMap<uint32, SPtr<VertexBuffer>> m_vertexBuffers;
      uint32 m_maxBufferIdx = 0;
    };
  }
}
