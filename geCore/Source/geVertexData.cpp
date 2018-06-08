/*****************************************************************************/
/**
 * @file    geVertexData.cpp
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

/*****************************************************************************/
/**
 * Includes
 */
/*****************************************************************************/
#include "geVertexData.h"
#include "geHardwareBufferManager.h"
#include "geRenderAPI.h"

#include <geVector3.h>
#include <geException.h>

namespace geEngineSDK {
  using std::max;

  namespace geCoreThread {
    void
    VertexData::setBuffer(uint32 index, SPtr<VertexBuffer> buffer) {
      m_vertexBuffers[index] = buffer;
      recalculateMaxIndex();
    }

    SPtr<VertexBuffer>
    VertexData::getBuffer(uint32 index) const {
      auto iterFind = m_vertexBuffers.find(index);
      if (iterFind != m_vertexBuffers.end()) {
        return iterFind->second;
      }

      return nullptr;
    }

    bool
    VertexData::isBufferBound(uint32 index) const {
      auto iterFind = m_vertexBuffers.find(index);
      if (iterFind != m_vertexBuffers.end()) {
        if (nullptr != iterFind->second) {
          return true;
        }
      }

      return false;
    }

    void
    VertexData::recalculateMaxIndex() {
      m_maxBufferIdx = 0;
      for (auto& bufferData : m_vertexBuffers) {
        m_maxBufferIdx = max(bufferData.first, m_maxBufferIdx);
      }
    }
  }
}
