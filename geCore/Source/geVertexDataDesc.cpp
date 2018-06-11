/*****************************************************************************/
/**
 * @file    geVertexDataDesc.cpp
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

/*****************************************************************************/
/**
 * Includes
 */
/*****************************************************************************/
#include "geVertexDataDesc.h"
#include "geVertexDataDescRTTI.h"
#include "geHardwareBufferManager.h"

#include <geNumericLimits.h>

namespace geEngineSDK {
  using std::max;
  using std::find_if;

  void
  VertexDataDesc::addVertElem(VERTEX_ELEMENT_TYPE::E type,
                              VERTEX_ELEMENT_SEMANTIC::E semantic,
                              uint32 semanticIdx,
                              uint32 streamIdx,
                              uint32 instanceStepRate) {
    clearIfItExists(type, semantic, semanticIdx, streamIdx);

    VertexElement newElement(static_cast<uint16>(streamIdx),
                             0,
                             type,
                             semantic,
                             static_cast<uint16>(semanticIdx),
                             instanceStepRate);

    //Insert it so it is sorted by stream
    auto insertToIndex = static_cast<uint32>(m_vertexElements.size());
    uint32 idx = 0;
    for (auto& elem : m_vertexElements) {
      if (elem.getStreamIdx() > streamIdx) {
        insertToIndex = idx;
        break;
      }
      ++idx;
    }

    m_vertexElements.insert(m_vertexElements.begin() + insertToIndex, newElement);
  }

  Vector<VertexElement>
  VertexDataDesc::createElements() const {
    uint32 maxStreamIdx = getMaxStreamIdx();

    uint32 numStreams = maxStreamIdx + 1;
    auto streamOffsets = ge_newN<uint32>(numStreams);
    for (uint32 i = 0; i < numStreams; ++i) {
      streamOffsets[i] = 0;
    }

    Vector<VertexElement> declarationElements;
    for (auto& vertElem : m_vertexElements) {
      uint32 streamIdx = vertElem.getStreamIdx();

      declarationElements.emplace_back(static_cast<uint16>(streamIdx),
                                       streamOffsets[streamIdx],
                                       vertElem.getType(),
                                       vertElem.getSemantic(),
                                       vertElem.getSemanticIdx(),
                                       vertElem.getInstanceStepRate());

      streamOffsets[streamIdx] += vertElem.getSize();
    }

    ge_deleteN(streamOffsets, numStreams);

    return declarationElements;
  }

  uint32
  VertexDataDesc::getMaxStreamIdx() const {
    uint32 maxStreamIdx = 0;
    auto numElems = static_cast<uint32>(m_vertexElements.size());
    for (uint32 i = 0; i < numElems; ++i) {
      for (auto& vertElem : m_vertexElements) {
        maxStreamIdx = max(static_cast<uint32>(maxStreamIdx),
                           static_cast<uint32>(vertElem.getStreamIdx()));
      }
    }

    return maxStreamIdx;
  }

  bool
  VertexDataDesc::hasStream(uint32 streamIdx) const {
    for (auto& vertElem : m_vertexElements) {
      if (vertElem.getStreamIdx() == streamIdx) {
        return true;
      }
    }

    return false;
  }

  bool
  VertexDataDesc::hasElement(VERTEX_ELEMENT_SEMANTIC::E semantic,
                             uint32 semanticIdx,
                             uint32 streamIdx) const {
    auto findIter = find_if(m_vertexElements.begin(),
                            m_vertexElements.end(),
    [semantic, semanticIdx, streamIdx](const VertexElement& x)
    {
      return x.getSemantic() == semantic &&
             x.getSemanticIdx() == semanticIdx &&
             x.getStreamIdx() == streamIdx;
    });

    if (m_vertexElements.end() != findIter) {
      return true;
    }

    return false;
  }

  uint32
  VertexDataDesc::getElementSize(VERTEX_ELEMENT_SEMANTIC::E semantic,
                                 uint32 semanticIdx,
                                 uint32 streamIdx) const {
    for (auto& element : m_vertexElements) {
      if (element.getSemantic() == semantic &&
          element.getSemanticIdx() == semanticIdx &&
          element.getStreamIdx() == streamIdx) {
        return element.getSize();
      }
    }

    return NumLimit::MAX_UINT32;
  }

  uint32
  VertexDataDesc::getElementOffsetFromStream(VERTEX_ELEMENT_SEMANTIC::E semantic,
                                             uint32 semanticIdx,
                                             uint32 streamIdx) const {
    uint32 vertexOffset = 0;
    for (auto& element : m_vertexElements) {
      if (element.getStreamIdx() != streamIdx) {
        continue;
      }

      if (element.getSemantic() == semantic &&
          element.getSemanticIdx() == semanticIdx) {
        break;
      }

      vertexOffset += element.getSize();
    }

    return vertexOffset;
  }

  uint32
  VertexDataDesc::getVertexStride(uint32 streamIdx) const {
    uint32 vertexStride = 0;
    for (auto& element : m_vertexElements) {
      if (element.getStreamIdx() == streamIdx) {
        vertexStride += element.getSize();
      }
    }

    return vertexStride;
  }

  uint32
  VertexDataDesc::getVertexStride() const {
    uint32 vertexStride = 0;
    for (auto& element : m_vertexElements) {
      vertexStride += element.getSize();
    }

    return vertexStride;
  }

  uint32
  VertexDataDesc::getStreamOffset(uint32 streamIdx) const {
    uint32 streamOffset = 0;
    for (auto& element : m_vertexElements) {
      if (element.getStreamIdx() == streamIdx) {
        break;
      }

      streamOffset += element.getSize();
    }

    return streamOffset;
  }

  const VertexElement*
  VertexDataDesc::getElement(VERTEX_ELEMENT_SEMANTIC::E semantic,
                             uint32 semanticIdx,
                             uint32 streamIdx) const {
    auto findIter = find_if(m_vertexElements.begin(),
                            m_vertexElements.end(),
    [semantic, semanticIdx, streamIdx](const VertexElement& x)
    {
      return x.getSemantic() == semantic &&
             x.getSemanticIdx() == semanticIdx &&
             x.getStreamIdx() == streamIdx;
    });

    if (m_vertexElements.end() != findIter) {
      return &(*findIter);
    }

    return nullptr;
  }

  void
  VertexDataDesc::clearIfItExists(VERTEX_ELEMENT_TYPE::E /*type*/,
                                  VERTEX_ELEMENT_SEMANTIC::E semantic,
                                  uint32 semanticIdx,
                                  uint32 streamIdx) {
    auto findIter = find_if(m_vertexElements.begin(),
                            m_vertexElements.end(),
    [semantic, semanticIdx, streamIdx](const VertexElement& x)
    {
      return x.getSemantic() == semantic &&
             x.getSemanticIdx() == semanticIdx &&
             x.getStreamIdx() == streamIdx;
    });

    if (m_vertexElements.end() != findIter) {
      m_vertexElements.erase(findIter);
    }
  }

  SPtr<VertexDataDesc>
  VertexDataDesc::create() {
    return ge_shared_ptr_new<VertexDataDesc>();
  }

  /***************************************************************************/
  /**
   * RTTI
   */
  /***************************************************************************/

  RTTITypeBase*
  VertexDataDesc::getRTTIStatic() {
    return VertexDataDescRTTI::instance();
  }

  RTTITypeBase*
  VertexDataDesc::getRTTI() const {
    return VertexDataDesc::getRTTIStatic();
  }
}
