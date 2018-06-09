/*****************************************************************************/
/**
 * @file    geGPUBuffer.cpp
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2018/06/08
 * @brief   Handles a generic GPU buffer.
 *
 * Handles a generic GPU buffer that you may use for storing any kind of data.
 *
 * @bug     No known bugs.
 */
/*****************************************************************************/

/*****************************************************************************/
/**
 * Includes
 */
/*****************************************************************************/
#include "geGPUBuffer.h"
#include "geRenderAPI.h"
#include "geHardwareBufferManager.h"

#include <geException.h>

namespace geEngineSDK {
  uint32 getBufferSize(const GPU_BUFFER_DESC& desc)
  {
    uint32 elementSize;

    if (desc.type == GPU_BUFFER_TYPE::kSTANDARD)
      elementSize = GPUBuffer::getFormatSize(desc.format);
    else
      elementSize = desc.elementSize;

    return elementSize * desc.elementCount;
  }

  GPUBufferProperties::GPUBufferProperties(const GPU_BUFFER_DESC& desc)
    : mDesc(desc)
  {
    if (mDesc.type == GPU_BUFFER_TYPE::kSTANDARD)
      mDesc.elementSize = GPUBuffer::getFormatSize(mDesc.format);
  }

  GPUBuffer::GPUBuffer(const GPU_BUFFER_DESC& desc)
    :mProperties(desc)
  {
  }

  SPtr<geCoreThread::GPUBuffer> GPUBuffer::getCore() const
  {
    return std::static_pointer_cast<geCoreThread::GPUBuffer>(m_coreSpecific);
  }

  SPtr<geCoreThread::CoreObject> GPUBuffer::createCore() const
  {
    return geCoreThread::HardwareBufferManager::instance().createGPUBufferInternal(mProperties.mDesc);
  }

  uint32 GPUBuffer::getFormatSize(GPU_BUFFER_FORMAT::E format)
  {
    static bool lookupInitialized = false;

    static uint32 lookup[GPU_BUFFER_FORMAT::kCOUNT];
    if (!lookupInitialized) {
      lookup[GPU_BUFFER_FORMAT::k16X1F] = 2;
      lookup[GPU_BUFFER_FORMAT::k16X2F] = 4;
      lookup[GPU_BUFFER_FORMAT::k16X4F] = 8;
      lookup[GPU_BUFFER_FORMAT::k32X1F] = 4;
      lookup[GPU_BUFFER_FORMAT::k32X2F] = 8;
      lookup[GPU_BUFFER_FORMAT::k32X3F] = 12;
      lookup[GPU_BUFFER_FORMAT::k32X4F] = 16;
      lookup[GPU_BUFFER_FORMAT::k8X1] = 1;
      lookup[GPU_BUFFER_FORMAT::k8X2] = 2;
      lookup[GPU_BUFFER_FORMAT::k8X4] = 4;
      lookup[GPU_BUFFER_FORMAT::k16X1] = 2;
      lookup[GPU_BUFFER_FORMAT::k16X2] = 4;
      lookup[GPU_BUFFER_FORMAT::k16X4] = 8;
      lookup[GPU_BUFFER_FORMAT::k8X1S] = 1;
      lookup[GPU_BUFFER_FORMAT::k8X2S] = 2;
      lookup[GPU_BUFFER_FORMAT::k8X4S] = 4;
      lookup[GPU_BUFFER_FORMAT::k16X1S] = 2;
      lookup[GPU_BUFFER_FORMAT::k16X2S] = 4;
      lookup[GPU_BUFFER_FORMAT::k16X4S] = 8;
      lookup[GPU_BUFFER_FORMAT::k32X1S] = 4;
      lookup[GPU_BUFFER_FORMAT::k32X2S] = 8;
      lookup[GPU_BUFFER_FORMAT::k32X3S] = 12;
      lookup[GPU_BUFFER_FORMAT::k32X4S] = 16;
      lookup[GPU_BUFFER_FORMAT::k8X1U] = 1;
      lookup[GPU_BUFFER_FORMAT::k8X2U] = 2;
      lookup[GPU_BUFFER_FORMAT::k8X4U] = 4;
      lookup[GPU_BUFFER_FORMAT::k16X1U] = 1;
      lookup[GPU_BUFFER_FORMAT::k16X2U] = 2;
      lookup[GPU_BUFFER_FORMAT::k16X4U] = 4;
      lookup[GPU_BUFFER_FORMAT::k32X1U] = 4;
      lookup[GPU_BUFFER_FORMAT::k32X2U] = 8;
      lookup[GPU_BUFFER_FORMAT::k32X3U] = 12;
      lookup[GPU_BUFFER_FORMAT::k32X4U] = 16;

      lookupInitialized = true;
    }

    if (format >= GPU_BUFFER_FORMAT::kCOUNT)
      return 0;

    return lookup[(uint32)format];
  }

  SPtr<GPUBuffer> GPUBuffer::create(const GPU_BUFFER_DESC& desc)
  {
    return HardwareBufferManager::instance().createGPUBuffer(desc);
  }

  namespace geCoreThread {
    GPUBuffer::GPUBuffer(const GPU_BUFFER_DESC& desc, uint32 deviceMask)
      :HardwareBuffer(getBufferSize(desc)), mProperties(desc)
    {
    }

    GPUBuffer::~GPUBuffer() {
      // Make sure that derived classes call clearBufferViews
      // I can't call it here since it needs a virtual method call
    }

    SPtr<GPUBuffer>
    GPUBuffer::create(const GPU_BUFFER_DESC& desc, GPU_DEVICE_FLAGS::E deviceMask) {
      return HardwareBufferManager::instance().createGPUBuffer(desc, deviceMask);
    }
  }
}
