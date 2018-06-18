/*****************************************************************************/
/**
 * @file    geD3D11HardwareBufferManager.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2018/06/16
 * @brief   Handles creation of DirectX 11 hardware buffers.
 *
 * Handles creation of DirectX 11 hardware buffers.
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

#include <geHardwareBufferManager.h>

namespace geEngineSDK {
  namespace geCoreThread {
    class D3D11HardwareBufferManager : public HardwareBufferManager
    {
     public:
      D3D11HardwareBufferManager(D3D11Device& device);

     protected:
      /**
       * @copydoc HardwareBufferManager::createVertexBufferInternal
       */
      SPtr<VertexBuffer>
      createVertexBufferInternal(const VERTEX_BUFFER_DESC& desc,
                                 GPU_DEVICE_FLAGS::E deviceMask =
                                   GPU_DEVICE_FLAGS::kDEFAULT) override;

      /**
       * @copydoc HardwareBufferManager::createIndexBufferInternal
       */
      SPtr<IndexBuffer>
      createIndexBufferInternal(const INDEX_BUFFER_DESC& desc,
                                GPU_DEVICE_FLAGS::E deviceMask =
                                  GPU_DEVICE_FLAGS::kDEFAULT) override;

      /**
       * @copydoc HardwareBufferManager::createGPUParamBlockBufferInternal
       */
      SPtr<GPUParamBlockBuffer>
      createGPUParamBlockBufferInternal(uint32 size,
                                        GPU_PARAM_BLOCK_USAGE::E usage =
                                          GPU_PARAM_BLOCK_USAGE::kDYNAMIC,
                                        GPU_DEVICE_FLAGS::E deviceMask =
                                          GPU_DEVICE_FLAGS::kDEFAULT) override;

      /**
       * @copydoc HardwareBufferManager::createGPUBufferInternal
       */
      SPtr<GPUBuffer>
      createGPUBufferInternal(const GPU_BUFFER_DESC& desc,
                              GPU_DEVICE_FLAGS::E deviceMask =
                                GPU_DEVICE_FLAGS::kDEFAULT) override;

      D3D11Device& m_device;
    };
  }
}
