#pragma once

#include "gePrerequisitesUtil.h"

#include <d3d11.h>
#include <vector>
#include <exception>

using namespace geEngineSDK;

using std::vector;
using std::exception;

class GraphicsBuffer
{
public:
  GraphicsBuffer() = default;
  virtual ~GraphicsBuffer() {
    if (m_pBuffer)
      m_pBuffer->Release();
  }

public:


public:
  ID3D11Buffer* m_pBuffer = nullptr;
};

template<typename TVERTEX>
class VertexBuffer final : public GraphicsBuffer
{
public:
  VertexBuffer() = default;
  ~VertexBuffer() {}

  void reserve(size_t numObjects) {
    m_vertexData.reserve(numObjects);
  }

  void add(const TVERTEX& vertex) {
    m_vertexData.push_back(vertex);
  }

  void add(const vector<TVERTEX>& vertices) {
    m_vertexData.insert(m_vertexData.end(), vertices.begin(), vertices.end());
  }

  void add(TVERTEX* pVertices, size_t numVertices) {
    m_vertexData.insert(m_vertexData.end(), pVertices, pVertices + numVertices);
  }

  void clear() {
    m_vertexData.clear();
  }

  void createHardwareBuffer(ID3D11Device* pd3dDevice,
    unsigned int usage = D3D11_USAGE_DEFAULT) {
    D3D11_BUFFER_DESC bd;
    memset(&bd, 0, sizeof(bd));

    bd.Usage = static_cast<D3D11_USAGE>(usage);
    bd.ByteWidth = static_cast<uint32>(sizeof(TVERTEX) * m_vertexData.size());
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.CPUAccessFlags = 0;

    D3D11_SUBRESOURCE_DATA InitData;
    memset(&InitData, 0, sizeof(InitData));
    InitData.pSysMem = &m_vertexData[0];

    HRESULT hr = pd3dDevice->CreateBuffer(&bd, &InitData, &m_pBuffer);
    if (FAILED(hr)) {
      throw exception("Failed to create Vertex Buffer.");
    }
  }

  void setBuffer(ID3D11DeviceContext* pImmediateContext,
                 uint32 startSlot = 0,
                 uint32 numBuffers = 0)
  {
    uint32 stride = sizeof(TVERTEX);
    uint32 offset = 0;
    pImmediateContext->IASetVertexBuffers(startSlot,
                                          numBuffers,
                                          &m_pBuffer,
                                          &stride,
                                          &offset);
  }

 private:
  vector<TVERTEX> m_vertexData;
};

template<typename ITYPE>
class IndexBuffer final : public GraphicsBuffer
{
public:
  IndexBuffer() = default;
  ~IndexBuffer() {}

  void reserve(size_t numObjects) {
    m_indexData.reserve(numObjects);
  }

  void add(const ITYPE& index) {
    m_indexData.push_back(index);
  }

  void add(const vector<ITYPE>& indices) {
    m_indexData.insert(m_indexData.end(), indices.begin(), indices.end());
  }

  void add(ITYPE* pIndices, size_t numIndices) {
    m_indexData.insert(m_indexData.end(), pIndices, pIndices + numIndices);
  }

  void clear() {
    m_indexData.clear();
  }

  void createHardwareBuffer(ID3D11Device* pd3dDevice,
                            uint32 usage = D3D11_USAGE_DEFAULT) {
    D3D11_BUFFER_DESC bd;
    memset(&bd, 0, sizeof(bd));

    bd.Usage = static_cast<D3D11_USAGE>(usage);
    bd.ByteWidth = static_cast<uint32>(sizeof(ITYPE) * m_indexData.size());
    bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    bd.CPUAccessFlags = 0;

    D3D11_SUBRESOURCE_DATA InitData;
    memset(&InitData, 0, sizeof(InitData));
    InitData.pSysMem = &m_indexData[0];

    HRESULT hr = pd3dDevice->CreateBuffer(&bd, &InitData, &m_pBuffer);
    if (FAILED(hr)) {
      throw exception("Failed to create Index Buffer.");
    }
  }

  void setBuffer(ID3D11DeviceContext* pImmediateContext,
                 uint32 offset = 0)
  {
    uint32 format = DXGI_FORMAT_R32_UINT;
    if (sizeof(ITYPE) == 2) {
      format = DXGI_FORMAT_R16_UINT;
    }

    pImmediateContext->IASetIndexBuffer(m_pBuffer,
                                        static_cast<DXGI_FORMAT>(format),
                                        offset);
  }

 private:
  vector<ITYPE> m_indexData;
};
