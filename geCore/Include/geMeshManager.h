/*****************************************************************************/
/**
 * @file    geMeshManager.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2018/06/12
 * @brief   Manager that handles creation of Meshes.
 *
 * Manager that handles creation of Mesh%es..
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
//#include "geMesh.h"

#include <geModule.h>

namespace geEngineSDK {
  class GE_CORE_EXPORT MeshManager final : public Module<MeshManager>
  {
   public:
    MeshManager() = default;
    ~MeshManager() = default;

    /**
     * @brief Returns some dummy mesh data with one triangle you may use for
     *        initializing a mesh.
     */
    /*
    SPtr<MeshData>
    getDummyMeshData() const {
      return mDummyMeshData;
    }
    */
    /**
     * @brief Returns a dummy mesh containing one triangle.
     */
    HMesh
    getDummyMesh() const {
      return m_dummyMesh;
    }

   protected:
    /**
     * @copydoc Module::onStartUp
     */
    void
    onStartUp() override;

   private:
    //SPtr<MeshData> m_dummyMeshData;
    HMesh m_dummyMesh;
  };
}
