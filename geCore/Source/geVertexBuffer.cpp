/*****************************************************************************/
/**
 * @file    geVertexBuffer.cpp
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2018/06/08
 * @brief   Specialization of a hardware buffer used for holding vertex data.
 *
 * Specialization of a hardware buffer used for holding vertex data.
 *
 * @bug     No known bugs.
 */
/*****************************************************************************/

/*****************************************************************************/
/**
 * Includes
 */
/*****************************************************************************/
#include "geVertexBuffer.h"
#include "geHardwareBufferManager.h"

namespace geEngineSDK {
  using std::static_pointer_cast;

  VertexBufferProperties::VertexBufferProperties(uint32 numVertices,
                                                 uint32 vertexSize)
    : m_numVertices(numVertices),
      m_vertexSize(vertexSize)
  {}

  VertexBuffer::VertexBuffer(const VERTEX_BUFFER_DESC& desc)
    : m_properties(desc.numVerts, desc.vertexSize),
      m_usage(desc.usage),
      m_streamOut(desc.streamOut)
  {}

  SPtr<geCoreThread::CoreObject>
  VertexBuffer::createCore() const {
    VERTEX_BUFFER_DESC desc;
    desc.vertexSize = m_properties.m_vertexSize;
    desc.numVerts = m_properties.m_numVertices;
    desc.usage = m_usage;
    desc.streamOut = m_streamOut;

    return geCoreThread::HardwareBufferManager::instance().
             createVertexBufferInternal(desc);
  }

  SPtr<geCoreThread::VertexBuffer>
  VertexBuffer::getCore() const {
    return static_pointer_cast<geCoreThread::VertexBuffer>(m_coreSpecific);
  }

  SPtr<VertexBuffer>
  VertexBuffer::create(const VERTEX_BUFFER_DESC& desc) {
    return HardwareBufferManager::instance().createVertexBuffer(desc);
  }

  namespace geCoreThread {
    VertexBuffer::VertexBuffer(const VERTEX_BUFFER_DESC& desc,
                               GPU_DEVICE_FLAGS::E /*deviceMask*/)
      : HardwareBuffer(desc.vertexSize * desc.numVerts),
        m_properties(desc.numVerts, desc.vertexSize)
    {}

    SPtr<VertexBuffer>
    VertexBuffer::create(const VERTEX_BUFFER_DESC& desc,
                         GPU_DEVICE_FLAGS::E deviceMask) {
      return HardwareBufferManager::instance().createVertexBuffer(desc,
                                                                  deviceMask);
    }
  }
}
