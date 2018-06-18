/*****************************************************************************/
/**
 * @file    geD3D11InputLayoutManager.cpp
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2018/06/17
 * @brief   Handles creation and caching of DirectX 11 input layout objects.
 *
 * Handles creation and caching of DirectX 11 input layout objects.
 *
 * @bug     No known bugs.
 */
/*****************************************************************************/

/*****************************************************************************/
/**
 * Includes
 */
/*****************************************************************************/
#include "geD3D11InputLayoutManager.h"
#include "geD3D11Mappings.h"
#include "geD3D11RenderAPI.h"
#include "geD3D11Device.h"
#include "geD3D11GPUProgram.h"

#include <geHardwareBufferManager.h>
#include <geRenderStats.h>
#include <geDebug.h>

namespace geEngineSDK {
  using std::max;

  namespace geCoreThread {
    size_t
    D3D11InputLayoutManager::HashFunc::operator()
                            (const D3D11InputLayoutManager::VertexDeclarationKey &key) const {
      size_t hash = 0;
      hash_combine(hash, key.vertxDeclId);
      hash_combine(hash, key.vertexProgramId);

      return hash;
    }

    bool
    D3D11InputLayoutManager::EqualFunc::operator()
                              (const D3D11InputLayoutManager::VertexDeclarationKey &a,
                               const D3D11InputLayoutManager::VertexDeclarationKey &b) const {
      if (a.vertxDeclId != b.vertxDeclId) {
        return false;
      }

      if (a.vertexProgramId != b.vertexProgramId) {
        return false;
      }

      return true;
    }

    D3D11InputLayoutManager::D3D11InputLayoutManager()
      : m_lastUsedCounter(0),
        m_warningShown(false)
    {}

    D3D11InputLayoutManager::~D3D11InputLayoutManager() {
      while (m_inputLayoutMap.begin() != m_inputLayoutMap.end()) {
        auto firstElem = m_inputLayoutMap.begin();

        SAFE_RELEASE(firstElem->second->inputLayout);
        ge_delete(firstElem->second);

        m_inputLayoutMap.erase(firstElem);
        GE_INC_RENDER_STAT_CAT(ResDestroyed, D3D11_RENDER_STAT_RESOURCE_TYPE::kInputLayout);
      }
    }

    ID3D11InputLayout*
    D3D11InputLayoutManager::retrieveInputLayout(
                                              const SPtr<VertexDeclaration>& vertexShaderDecl,
                                              const SPtr<VertexDeclaration>& vertexBufferDecl,
                                              D3D11GPUProgram& vertexProgram) {
      VertexDeclarationKey pair;
      pair.vertxDeclId = vertexBufferDecl->getId();
      pair.vertexProgramId = vertexProgram.getProgramId();

      auto iterFind = m_inputLayoutMap.find(pair);
      if (m_inputLayoutMap.end() == iterFind) {
        if (m_inputLayoutMap.size() >= DECLARATION_BUFFER_SIZE){
          //Prune so the buffer doesn't just infinitely grow
          removeLeastUsed();
        }

        addNewInputLayout(vertexShaderDecl, vertexBufferDecl, vertexProgram);

        iterFind = m_inputLayoutMap.find(pair);

        if (iterFind == m_inputLayoutMap.end()) {
          //We failed to create input layout
          return nullptr;
        }
      }

      iterFind->second->lastUsedIdx = ++m_lastUsedCounter;
      return iterFind->second->inputLayout;
    }

    void
    D3D11InputLayoutManager::addNewInputLayout(
                                              const SPtr<VertexDeclaration>& vertexShaderDecl,
                                              const SPtr<VertexDeclaration>& vertexBufferDecl,
                                              D3D11GPUProgram& vertexProgram) {
      const auto& bufferDeclProps = vertexBufferDecl->getProperties();
      const auto& shaderDeclProps = vertexShaderDecl->getProperties();

      Vector<D3D11_INPUT_ELEMENT_DESC> declElements;

      const auto& bufferElems = bufferDeclProps.getElements();
      const auto& shaderElems = shaderDeclProps.getElements();

      int32 maxStreamIdx = -1;
      for (const auto & bufferElem : bufferElems)
      {
        declElements.emplace_back();
        D3D11_INPUT_ELEMENT_DESC& elementDesc = declElements.back();

        elementDesc.SemanticName = D3D11Mappings::get(bufferElem.getSemantic());
        elementDesc.SemanticIndex = bufferElem.getSemanticIdx();
        elementDesc.Format = D3D11Mappings::get(bufferElem.getType());
        elementDesc.InputSlot = bufferElem.getStreamIdx();
        elementDesc.AlignedByteOffset = static_cast<WORD>(bufferElem.getOffset());

        if (bufferElem.getInstanceStepRate() == 0)
        {
          elementDesc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
          elementDesc.InstanceDataStepRate = 0;
        }
        else
        {
          elementDesc.InputSlotClass = D3D11_INPUT_PER_INSTANCE_DATA;
          elementDesc.InstanceDataStepRate = bufferElem.getInstanceStepRate();
        }

        maxStreamIdx = max(maxStreamIdx, static_cast<int32>(bufferElem.getStreamIdx()));
      }

      //Find elements missing in buffer and add a dummy stream for them
      for (const auto & shaderElem : shaderElems) {
        bool foundElement = false;
        for (const auto & bufferElem : bufferElems) {
          if (shaderElem.getSemantic() == bufferElem.getSemantic() &&
              shaderElem.getSemanticIdx() == bufferElem.getSemanticIdx()) {
            foundElement = true;
            break;
          }
        }

        if (!foundElement) {
          declElements.emplace_back();
          D3D11_INPUT_ELEMENT_DESC& elementDesc = declElements.back();

          elementDesc.SemanticName = D3D11Mappings::get(shaderElem.getSemantic());
          elementDesc.SemanticIndex = shaderElem.getSemanticIdx();
          elementDesc.Format = D3D11Mappings::get(shaderElem.getType());
          elementDesc.InputSlot = static_cast<uint32>(maxStreamIdx + 1);
          elementDesc.AlignedByteOffset = 0;
          elementDesc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
          elementDesc.InstanceDataStepRate = 0;
        }
      }

      auto d3d11rs = static_cast<D3D11RenderAPI*>(RenderAPI::instancePtr());
      D3D11Device& device = d3d11rs->getPrimaryDevice();

      const DataBlob& microcode = vertexProgram.getMicroCode();

      auto newEntry = ge_new<InputLayoutEntry>();
      newEntry->lastUsedIdx = ++m_lastUsedCounter;
      newEntry->inputLayout = nullptr;

      HRESULT hr = device.getD3D11Device()->CreateInputLayout(&declElements[0],
                                                     static_cast<uint32>(declElements.size()),
                                                              microcode.data,
                                                              microcode.size,
                                                              &newEntry->inputLayout);

      if (FAILED(hr) || device.hasError()) {
        GE_EXCEPT(RenderingAPIException,
                  "Unable to set D3D11 vertex declaration" +
                  device.getErrorDescription());
      }

      //Create key and add to the layout map
      VertexDeclarationKey pair;
      pair.vertxDeclId = vertexBufferDecl->getId();
      pair.vertexProgramId = vertexProgram.getProgramId();

      m_inputLayoutMap[pair] = newEntry;

      GE_INC_RENDER_STAT_CAT(ResCreated, D3D11_RENDER_STAT_RESOURCE_TYPE::kInputLayout);
    }

    void
    D3D11InputLayoutManager::removeLeastUsed() {
      if (!m_warningShown) {
        LOGWRN("Input layout buffer is full, pruning last " +
               toString(NUM_ELEMENTS_TO_PRUNE) +
               " elements. This is probably okay unless you are creating a "
               "massive amount of input layouts as they will get re-created "
               "every frame. In that case you should increase the layout "
               "buffer size. This warning won't be shown again.");

        m_warningShown = true;
      }

      Map<uint32, VertexDeclarationKey> leastFrequentlyUsedMap;

      for (auto & iter : m_inputLayoutMap) {
        leastFrequentlyUsedMap[iter.second->lastUsedIdx] = iter.first;
      }

      uint32 elemsRemoved = 0;
      for (auto & iter : leastFrequentlyUsedMap) {
        auto inputLayoutIter = m_inputLayoutMap.find(iter.second);

        SAFE_RELEASE(inputLayoutIter->second->inputLayout);
        ge_delete(inputLayoutIter->second);

        m_inputLayoutMap.erase(inputLayoutIter);
        GE_INC_RENDER_STAT_CAT(ResDestroyed, D3D11_RENDER_STAT_RESOURCE_TYPE::kInputLayout);

        ++elemsRemoved;
        if (elemsRemoved >= NUM_ELEMENTS_TO_PRUNE) {
          break;
        }
      }
    }
  }
}
