/*****************************************************************************/
/**
 * @file    geD3D11InputLayoutManager.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2018/06/17
 * @brief   Handles creation and caching of DirectX 11 input layout objects.
 *
 * Handles creation and caching of DirectX 11 input layout objects.
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
#include "gePrerequisitesD3D11.h"

#include <geVertexDeclaration.h>

namespace geEngineSDK {
  using std::size_t;

  namespace geCoreThread {
    class D3D11InputLayoutManager
    {
     public:
      /**
       * @brief Key uniquely identifying vertex declaration and vertex shader
       *        combination.
       */
      struct VertexDeclarationKey
      {
        uint64 vertxDeclId;
        uint32 vertexProgramId;
      };

      /**
       * @brief Creates a hash from vertex declaration key.
       */
      class HashFunc
      {
       public:
        size_t
        operator()(const VertexDeclarationKey &key) const;
      };

      /**
       * @brief Compares two vertex declaration keys.
       */
      class EqualFunc
      {
       public:
        bool
        operator()(const VertexDeclarationKey &a,
                   const VertexDeclarationKey &b) const;
      };

      /**
       * @brief Contains data about a single instance of DX11 input layout
       *        object.
       */
      struct InputLayoutEntry
      {
        InputLayoutEntry() = default;

        ID3D11InputLayout* inputLayout;
        uint32 lastUsedIdx;
      };

     public:
      D3D11InputLayoutManager();
      ~D3D11InputLayoutManager();

      /**
       * @brief Finds an existing or creates a new D3D11 input layout.
       *        Input layout maps a vertex declaration from a vertex buffer to
       *        vertex program input declaration
       * @param[in] vertexShaderDecl  Vertex declaration describing vertex
       *                              program input parameters.
       * @param[in] vertexBufferDecl  Vertex declaration describing structure
       *                              of a vertex buffer to be bound as input
       *                              to the GPU program.
       * @param[in] vertexProgram     Instance of the vertex program we are
       *                              creating input layout for.
       * @note  Error will be thrown if the vertex buffer doesn't provide all
       *        the necessary data that the shader expects.
       */
      ID3D11InputLayout*
      retrieveInputLayout(const SPtr<VertexDeclaration>& vertexShaderDecl,
                          const SPtr<VertexDeclaration>& vertexBufferDecl,
                          D3D11GPUProgram& vertexProgram);

     private:
      /**
       * @brief Creates a new input layout using the specified parameters and
       *        stores it in the input layout map.
       */
      void
      addNewInputLayout(const SPtr<VertexDeclaration>& vertexShaderDecl,
                        const SPtr<VertexDeclaration>& vertexBufferDecl,
                        D3D11GPUProgram& vertexProgram);

      /**
       * @brief Destroys least used input layout.
       */
      void
      removeLeastUsed();

     private:
      static const int32 DECLARATION_BUFFER_SIZE = 1024;
      static const int32 NUM_ELEMENTS_TO_PRUNE = 64;

      UnorderedMap<VertexDeclarationKey,
                   InputLayoutEntry*,
                   HashFunc,
                   EqualFunc> m_inputLayoutMap;

      bool m_warningShown;
      uint32 m_lastUsedCounter;
    };
  }
}
