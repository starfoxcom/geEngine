/*****************************************************************************/
/**
 * @file    geVertexDataDesc.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2018/06/10
 * @brief   Contains information about layout of vertices in a buffer.
 *
 * Contains information about layout of vertices in a buffer. This is very
 * similar to VertexDeclaration but unlike VertexDeclaration it has no render
 * API object to back it up and is very lightweight.
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

namespace geEngineSDK {
  class GE_CORE_EXPORT VertexDataDesc : public IReflectable
  {
   public:
    VertexDataDesc() = default;

    /**
     * @brief Informs the internal buffer that it needs to make room for the
     *       specified vertex element. If a vertex with same stream and
     *       semantics already exists it will just be updated.
     * @param[in] type            Type of the vertex element. Determines size.
     * @param[in] semantic        Semantic that allows the engine to connect
     *                            the data to a shader input slot.
     * @param[in] semanticIdx     (optional) If there are multiple semantics
     *                            with the same name, use different index to
     *                            differentiate between them.
     * @param[in] streamIdx       (optional) Zero-based index of the stream.
     *                            Each stream will internally be represented
     *                            as a single vertex buffer.
     * @param[in] instanceStepRate  Determines at what rate does vertex element
     *                              data advance. Zero means each vertex will
     *                              advance the data pointer and receive new
     *                              data (standard behavior). Values larger
     *                              than one are relevant for instanced
     *                              rendering and determine how often do
     *                              instances advance the vertex element
     *                              (for example a value of 1 means each
     *                              instance will retrieve a new value for this
     *                              vertex element, a value of 2 means each
     *                              second instance will, etc.).
     */
    void
    addVertElem(VERTEX_ELEMENT_TYPE::E type,
                VERTEX_ELEMENT_SEMANTIC::E semantic,
                uint32 semanticIdx = 0,
                uint32 streamIdx = 0,
                uint32 instanceStepRate = 0);

    /**
     * @brief Query if we have vertex data for the specified semantic.
     */
    bool
    hasElement(VERTEX_ELEMENT_SEMANTIC::E semantic,
               uint32 semanticIdx = 0,
               uint32 streamIdx = 0) const;

    /**
     * @brief Returns the size in bytes of the vertex element with the
     *        specified semantic.
     */
    uint32
    getElementSize(VERTEX_ELEMENT_SEMANTIC::E semantic,
                   uint32 semanticIdx = 0,
                   uint32 streamIdx = 0) const;

    /**
     * @brief Returns offset of the vertex from start of the stream in bytes.
     */
    uint32
    getElementOffsetFromStream(VERTEX_ELEMENT_SEMANTIC::E semantic,
                               uint32 semanticIdx = 0,
                               uint32 streamIdx = 0) const;

    /**
     * @brief Gets vertex stride in bytes (offset from one vertex to another)
     *        in the specified stream.
     */
    uint32
    getVertexStride(uint32 streamIdx) const;

    /**
     * @brief Gets vertex stride in bytes (offset from one vertex to another)
     *        in all the streams.
     */
    uint32
    getVertexStride() const;

    /**
     * @brief Gets offset in bytes from the start of the internal buffer to the
     *        start of the specified stream.
     */
    uint32
    getStreamOffset(uint32 streamIdx) const;

    /**
     * @brief Returns the number of vertex elements.
     */
    uint32
    getNumElements() const {
      return static_cast<uint32>(m_vertexElements.size());
    }

    /**
     * @brief Returns the vertex element at the specified index.
     */
    const VertexElement&
    getElement(uint32 idx) const {
      return m_vertexElements[idx];
    }

    /**
     * @brief Returns the vertex element with the specified semantic.
     */
    const VertexElement*
    getElement(VERTEX_ELEMENT_SEMANTIC::E semantic,
               uint32 semanticIdx = 0,
               uint32 streamIdx = 0) const;

    /**
     * @brief Creates a list of vertex elements from internal data.
     */
    Vector<VertexElement>
    createElements() const;

    /**
     * @brief Creates a new empty vertex data descriptor.
     */
    static SPtr<VertexDataDesc>
    create();

   private:
    friend class Mesh;
    friend class geCoreThread::Mesh;
    friend class MeshHeap;
    friend class geCoreThread::MeshHeap;

    /**
     * @brief Returns the largest stream index of all the stored vertex
     *        elements.
     */
    uint32
    getMaxStreamIdx() const;

    /**
     * @brief Checks if any of the vertex elements use the specified stream
     *        index.
     */
    bool
    hasStream(uint32 streamIdx) const;

    /**
     * @brief Removes a vertex element of the specified type and semantics if
     *        it exists.
     */
    void
    clearIfItExists(VERTEX_ELEMENT_TYPE::E type,
                    VERTEX_ELEMENT_SEMANTIC::E semantic,
                    uint32 semanticIdx,
                    uint32 streamIdx);

   private:
    Vector<VertexElement> m_vertexElements;

    /*************************************************************************/
    /**
     * RTTI
     */
    /*************************************************************************/

   public:
    friend class VertexDataDescRTTI;

    static RTTITypeBase*
    getRTTIStatic();

    RTTITypeBase*
    getRTTI() const override;
  };
}
