/*****************************************************************************/
/**
 * @file    geMeshManager.cpp
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2018/06/12
 * @brief   Manager that handles creation of Meshes.
 *
 * Manager that handles creation of Mesh%es..
 *
 * @bug     No known bugs.
 */
/*****************************************************************************/

/*****************************************************************************/
/**
 * Includes
 */
/*****************************************************************************/
#include "geMeshManager.h"
//#include "geMesh.h"
#include "geVertexDataDesc.h"
#include "geCoreApplication.h"

#include <geVector3.h>

namespace geEngineSDK {
  void
  MeshManager::onStartUp() {
    /*
    SPtr<VertexDataDesc> vertexDesc = ge_shared_ptr_new<VertexDataDesc>();
    vertexDesc->addVertElem(VERTEX_ELEMENT_TYPE::kFLOAT3,
                            VERTEX_ELEMENT_SEMANTIC::kPOSITION);

    m_dummyMeshData = ge_shared_ptr_new<MeshData>(1, 3, vertexDesc);

    auto vecIter = m_dummyMeshData->getVec3DataIter(VERTEX_ELEMENT_SEMANTIC::kPOSITION);
    vecIter.setValue(Vector3(0, 0, 0));

    auto indices = m_dummyMeshData->getIndices32();
    indices[0] = 0;
    indices[1] = 0;
    indices[2] = 0;

    m_dummyMesh = Mesh::create(m_dummyMeshData);
    */
  }
}
