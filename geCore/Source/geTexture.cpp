/*****************************************************************************/
/**
 * @file    geTexture.cpp
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2018/05/19
 * @brief   Abstract class representing a texture.
 *
 * Abstract class representing a texture. Specific render systems have their
 * own Texture implementations. Internally represented as one or more surfaces
 * with pixels in a certain number of dimensions, backed by a hardware buffer.
 *
 * @bug     No known bugs.
 */
/*****************************************************************************/

/*****************************************************************************/
/**
 * Includes
 */
/*****************************************************************************/
#include "geTexture.h"
#include "geTextureRTTI.h"
#include "geCoreThread.h"
#include "geResources.h"
#include "gePixelUtil.h"

#include <geDataStream.h>
#include <geException.h>
#include <geDebug.h>
#include <geAsyncOp.h>

namespace geEngineSDK {
  using std::function;
  using std::bind;
  using std::static_pointer_cast;
  using namespace std::placeholders;

  TEXTURE_COPY_DESC TEXTURE_COPY_DESC::DEFAULT = TEXTURE_COPY_DESC();

  TextureProperties::TextureProperties(const TEXTURE_DESC& desc)
    : m_desc(desc)
  {}

  bool
  TextureProperties::hasAlpha() const {
    return PixelUtil::hasAlpha(m_desc.format);
  }

  uint32
  TextureProperties::getNumFaces() const {
    uint32 facesPerSlice = getTextureType() == TEX_TYPE_CUBE_MAP ? 6 : 1;
    return facesPerSlice * m_desc.numArraySlices;
  }

  void
  TextureProperties::mapFromSubresourceIdx(uint32 subresourceIdx,
                                           uint32& face,
                                           uint32& mip) const {
    uint32 numMipmaps = getNumMipmaps() + 1;
    face = Math::floor((subresourceIdx) / static_cast<float>(numMipmaps));
    mip = subresourceIdx % numMipmaps;
  }

  uint32
  TextureProperties::mapToSubresourceIdx(uint32 face, uint32 mip) const {
    return face * (getNumMipmaps() + 1) + mip;
  }

  SPtr<PixelData>
  TextureProperties::allocBuffer(uint32 /*face*/, uint32 mipLevel) const {
    uint32 width = getWidth();
    uint32 height = getHeight();
    uint32 depth = getDepth();

    for (uint32 j = 0; j < mipLevel; ++j) {
      if (width != 1) {
        width = width >> 1;
      }
      if (height != 1) {
        height = height >> 1;
      }
      if (depth != 1) {
        depth = depth >> 1;
      }
    }

    SPtr<PixelData>
      dst = ge_shared_ptr_new<PixelData>(width, height, depth, getFormat());
    dst->allocateInternalBuffer();

    return dst;
  }

  Texture::Texture(const TEXTURE_DESC& desc)
    : m_properties(desc)
  {}

  Texture::Texture(const TEXTURE_DESC& desc, const SPtr<PixelData>& pixelData)
    : m_properties(desc), m_initData(pixelData) {
    if (nullptr != m_initData) {
      m_initData->_lock();
    }
  }

  void
  Texture::initialize() {
    m_size = calculateSize();

    //Allocate CPU buffers if needed
    if ((m_properties.getUsage() & TU_CPUCACHED) != 0) {
      createCPUBuffers();

      if (nullptr != m_initData) {
        updateCPUBuffers(0, *m_initData);
      }
    }

    Resource::initialize();
  }

  SPtr<geCoreThread::CoreObject>
  Texture::createCore() const {
    const TextureProperties& props = getProperties();

    SPtr<geCoreThread::CoreObject> coreObj =
      geCoreThread::TextureManager::instance().createTextureInternal(props.m_desc,
                                                                     m_initData);
    if ((m_properties.getUsage() & TU_CPUCACHED) == 0) {
      m_initData = nullptr;
    }
    return coreObj;
  }

  AsyncOp
  Texture::writeData(const SPtr<PixelData>& data,
                     uint32 face,
                     uint32 mipLevel,
                     bool discardEntireBuffer) {
    uint32 subresourceIdx = m_properties.mapToSubresourceIdx(face, mipLevel);
    updateCPUBuffers(subresourceIdx, *data);

    data->_lock();

    function<void(const SPtr<geCoreThread::Texture>&,
                  uint32,
                  uint32,
                  const SPtr<PixelData>&,
                  bool, AsyncOp&)> func =
      [&](const SPtr<geCoreThread::Texture>& texture,
          uint32 _face,
          uint32 _mipLevel,
          const SPtr<PixelData>& _pixData,
          bool _discardEntireBuffer,
          AsyncOp& asyncOp)
    {
      texture->writeData(*_pixData, _mipLevel, _face, _discardEntireBuffer);
      _pixData->_unlock();
      asyncOp._completeOperation();

    };

    return g_coreThread().queueReturnCommand(bind(func,
                                                  getCore(),
                                                  face,
                                                  mipLevel,
                                                  data,
                                                  discardEntireBuffer,
                                                  _1));
  }

  AsyncOp
  Texture::readData(const SPtr<PixelData>& data, uint32 face, uint32 mipLevel) {
    data->_lock();

    function<void(const SPtr<geCoreThread::Texture>&,
                  uint32,
                  uint32,
                  const SPtr<PixelData>&,
                  AsyncOp&)> func =
      [&](const SPtr<geCoreThread::Texture>& texture,
          uint32 _face,
          uint32 _mipLevel,
          const SPtr<PixelData>& _pixData,
          AsyncOp& asyncOp)
    {
      //Make sure any queued command start executing before reading
      geCoreThread::RenderAPI::instance().submitCommandBuffer(nullptr);

      texture->readData(*_pixData, _mipLevel, _face);
      _pixData->_unlock();
      asyncOp._completeOperation();
    };

    return g_coreThread().queueReturnCommand(bind(func,
                                                  getCore(),
                                                  face,
                                                  mipLevel,
                                                  data,
                                                  _1));
  }

  uint32
  Texture::calculateSize() const {
    return m_properties.getNumFaces() *
            PixelUtil::getMemorySize(m_properties.getWidth(),
                                     m_properties.getHeight(),
                                     m_properties.getDepth(),
                                     m_properties.getFormat());
  }

  void
  Texture::updateCPUBuffers(uint32 subresourceIdx, const PixelData& pixelData) {
    if ((m_properties.getUsage() & TU_CPUCACHED) == 0) {
      return;
    }

    if (static_cast<uint32>(m_cpuSubresourceData.size()) <= subresourceIdx) {
      LOGERR("Invalid subresource index: " +
             toString(subresourceIdx) +
             ". Supported range: 0 .. " +
             toString(static_cast<uint32>(m_cpuSubresourceData.size())));
      return;
    }

    uint32 mipLevel;
    uint32 face;
    m_properties.mapFromSubresourceIdx(subresourceIdx, face, mipLevel);

    uint32 mipWidth, mipHeight, mipDepth;
    PixelUtil::getSizeForMipLevel(m_properties.getWidth(),
                                  m_properties.getHeight(),
                                  m_properties.getDepth(),
                                  mipLevel,
                                  mipWidth,
                                  mipHeight,
                                  mipDepth);

    if (pixelData.getWidth() != mipWidth ||
        pixelData.getHeight() != mipHeight ||
        pixelData.getDepth() != mipDepth ||
        pixelData.getFormat() != m_properties.getFormat()) {
      LOGERR("Provided buffer is not of valid dimensions or format in order "
             "to update this texture.");
      return;
    }

    if (m_cpuSubresourceData[subresourceIdx]->getSize() != pixelData.getSize()) {
      GE_EXCEPT(InternalErrorException, "Buffer sizes don't match.");
    }

    uint8* dest = m_cpuSubresourceData[subresourceIdx]->getData();
    uint8* src = pixelData.getData();

    memcpy(dest, src, pixelData.getSize());
  }

  void
  Texture::readCachedData(PixelData& dest, uint32 face, uint32 mipLevel) {
    if ((m_properties.getUsage() & TU_CPUCACHED) == 0) {
      LOGERR("Attempting to read CPU data from a texture that is created "
             "without CPU caching.");
      return;
    }

    uint32 mipWidth, mipHeight, mipDepth;
    PixelUtil::getSizeForMipLevel(m_properties.getWidth(),
                                  m_properties.getHeight(),
                                  m_properties.getDepth(),
                                  mipLevel,
                                  mipWidth,
                                  mipHeight,
                                  mipDepth);

    if (dest.getWidth() != mipWidth ||
        dest.getHeight() != mipHeight ||
        dest.getDepth() != mipDepth ||
        dest.getFormat() != m_properties.getFormat()) {
      LOGERR("Provided buffer is not of valid dimensions or format in order "
             "to read from this texture.");
      return;
    }

    uint32 subresourceIdx = m_properties.mapToSubresourceIdx(face, mipLevel);
    if (static_cast<uint32>(m_cpuSubresourceData.size()) <= subresourceIdx) {
      LOGERR("Invalid subresource index: " +
             toString(subresourceIdx) +
             ". Supported range: 0 .. " +
             toString(static_cast<uint32>(m_cpuSubresourceData.size())));
      return;
    }

    if (m_cpuSubresourceData[subresourceIdx]->getSize() != dest.getSize()) {
      GE_EXCEPT(InternalErrorException, "Buffer sizes don't match.");
    }

    uint8* srcPtr = m_cpuSubresourceData[subresourceIdx]->getData();
    uint8* destPtr = dest.getData();

    memcpy(destPtr, srcPtr, dest.getSize());
  }

  void
  Texture::createCPUBuffers() {
    uint32 numFaces = m_properties.getNumFaces();
    uint32 numMips = m_properties.getNumMipmaps() + 1;

    uint32 numSubresources = numFaces * numMips;
    m_cpuSubresourceData.resize(numSubresources);

    for (uint32 i = 0; i < numFaces; ++i) {
      uint32 curWidth = m_properties.getWidth();
      uint32 curHeight = m_properties.getHeight();
      uint32 curDepth = m_properties.getDepth();

      for (uint32 j = 0; j < numMips; ++j) {
        uint32 subresourceIdx = m_properties.mapToSubresourceIdx(i, j);

        m_cpuSubresourceData[subresourceIdx] =
          ge_shared_ptr_new<PixelData>(curWidth,
                                       curHeight,
                                       curDepth,
                                       m_properties.getFormat());
        m_cpuSubresourceData[subresourceIdx]->allocateInternalBuffer();

        if (curWidth > 1) {
          curWidth = curWidth >> 1;
        }

        if (curHeight > 1) {
          curHeight = curHeight >> 1;
        }

        if (curDepth > 1) {
          curDepth = curDepth >> 1;
        }
      }
    }
  }

  SPtr<geCoreThread::Texture>
  Texture::getCore() const {
    return static_pointer_cast<geCoreThread::Texture>(m_coreSpecific);
  }

  /***************************************************************************/
  /**
   * Serialization
   */
  /***************************************************************************/

  RTTITypeBase*
  Texture::getRTTIStatic() {
    return TextureRTTI::instance();
  }

  RTTITypeBase*
  Texture::getRTTI() const {
    return Texture::getRTTIStatic();
  }

  /***************************************************************************/
  /**
   * Statics
   */
  /***************************************************************************/
  HTexture
  Texture::create(const TEXTURE_DESC& desc) {
    SPtr<Texture> texturePtr = _createPtr(desc);
    return static_resource_cast<Texture>(g_resources()._createResourceHandle(texturePtr));
  }

  HTexture
  Texture::create(const SPtr<PixelData>& pixelData, int32 usage, bool hwGammaCorrection) {
    SPtr<Texture> texturePtr = _createPtr(pixelData, usage, hwGammaCorrection);
    return static_resource_cast<Texture>(g_resources()._createResourceHandle(texturePtr));
  }

  SPtr<Texture>
  Texture::_createPtr(const TEXTURE_DESC& desc) {
    return TextureManager::instance().createTexture(desc);
  }

  SPtr<Texture>
  Texture::_createPtr(const SPtr<PixelData>& pixelData,
                      int32 usage,
                      bool hwGammaCorrection) {
    TEXTURE_DESC desc;
    desc.type = pixelData->getDepth() > 1 ? TEX_TYPE_3D : TEX_TYPE_2D;
    desc.width = pixelData->getWidth();
    desc.height = pixelData->getHeight();
    desc.depth = pixelData->getDepth();
    desc.format = pixelData->getFormat();
    desc.usage = usage;
    desc.hwGamma = hwGammaCorrection;

    return TextureManager::instance().createTexture(desc, pixelData);
  }

  namespace geCoreThread {
    SPtr<Texture> Texture::WHITE;
    SPtr<Texture> Texture::BLACK;
    SPtr<Texture> Texture::NORMAL;

    Texture::Texture(const TEXTURE_DESC& desc,
                     const SPtr<PixelData>& initData,
                     GPU_DEVICE_FLAGS::E /*deviceMask*/)
      : m_properties(desc),
        m_initData(initData)
    {}

    void
    Texture::initialize() {
      if (nullptr != m_initData) {
        writeData(*m_initData, 0, 0, true);
        m_initData->_unlock();
        m_initData = nullptr;
      }
      CoreObject::initialize();
    }

    void
    Texture::writeData(const PixelData& src,
                       uint32 mipLevel,
                       uint32 face,
                       bool discardEntireBuffer,
                       uint32 queueIdx) {
      THROW_IF_NOT_CORE_THREAD;

      if (discardEntireBuffer) {
        if ((m_properties.getUsage() & TU_DYNAMIC) == 0) {
          //Buffer discard is enabled but buffer was not created as dynamic.
          //Disabling discard.
          discardEntireBuffer = false;
        }
      }

      writeDataImpl(src, mipLevel, face, discardEntireBuffer, queueIdx);
    }

    void
    Texture::readData(PixelData& dest,
                      uint32 mipLevel,
                      uint32 face,
                      uint32 deviceIdx,
                      uint32 queueIdx) {
      THROW_IF_NOT_CORE_THREAD;

      PixelData& pixelData = static_cast<PixelData&>(dest);

      uint32 mipWidth, mipHeight, mipDepth;
      PixelUtil::getSizeForMipLevel(m_properties.getWidth(),
                                    m_properties.getHeight(),
                                    m_properties.getDepth(),
                                    mipLevel,
                                    mipWidth,
                                    mipHeight,
                                    mipDepth);

      if (pixelData.getWidth() != mipWidth ||
          pixelData.getHeight() != mipHeight ||
          pixelData.getDepth() != mipDepth ||
          pixelData.getFormat() != m_properties.getFormat()) {
        LOGERR("Provided buffer is not of valid dimensions or format in "
               "order to read from this texture.");
        return;
      }

      readDataImpl(pixelData, mipLevel, face, deviceIdx, queueIdx);
    }

    PixelData
    Texture::lock(GPU_LOCK_OPTIONS::E options,
                  uint32 mipLevel,
                  uint32 face,
                  uint32 deviceIdx,
                  uint32 queueIdx) {
      THROW_IF_NOT_CORE_THREAD;

      if (m_properties.getNumMipmaps() < mipLevel) {
        LOGERR("Invalid mip level: " +
               toString(mipLevel) +
               ". Min is 0, max is " +
               toString(m_properties.getNumMipmaps()));
        return PixelData(0, 0, 0, PixelFormat::kUNKNOWN);
      }

      if (m_properties.getNumFaces() <= face) {
        LOGERR("Invalid face index: " +
               toString(face) +
               ". Min is 0, max is " +
               toString(m_properties.getNumFaces()));
        return PixelData(0, 0, 0, PixelFormat::kUNKNOWN);
      }

      return lockImpl(options, mipLevel, face, deviceIdx, queueIdx);
    }

    void
    Texture::unlock() {
      THROW_IF_NOT_CORE_THREAD;
      unlockImpl();
    }

    void
    Texture::copy(const SPtr<Texture>& target,
                  const TEXTURE_COPY_DESC& desc,
                  const SPtr<CommandBuffer>& commandBuffer) {
      THROW_IF_NOT_CORE_THREAD;

      if (target->m_properties.getTextureType() != m_properties.getTextureType()) {
        LOGERR("Source and destination textures must be of same type.");
        return;
      }

      //NOTE: It might be okay to use different formats of the same size
      if (m_properties.getFormat() != target->m_properties.getFormat()) {
        LOGERR("Source and destination texture formats must match.");
        return;
      }

      if (target->m_properties.getNumSamples() > 1 &&
          m_properties.getNumSamples() != target->m_properties.getNumSamples()) {
        LOGERR("When copying to a multisampled texture, source texture must "
               "have the same number of samples.");
        return;
      }

      if (m_properties.getNumFaces() <= desc.srcFace) {
        LOGERR("Invalid source face index.");
        return;
      }

      if (target->m_properties.getNumFaces() <= desc.dstFace) {
        LOGERR("Invalid destination face index.");
        return;
      }

      if (m_properties.getNumMipmaps() < desc.srcMip) {
        LOGERR("Source mip level out of range. Valid range is [0, " +
               toString(m_properties.getNumMipmaps()) + "].");
        return;
      }

      if (target->m_properties.getNumMipmaps() < desc.dstMip) {
        LOGERR("Destination mip level out of range. Valid range is [0, " +
               toString(target->m_properties.getNumMipmaps()) + "].");
        return;
      }

      uint32 srcWidth, srcHeight, srcDepth;
      PixelUtil::getSizeForMipLevel(m_properties.getWidth(),
                                    m_properties.getHeight(),
                                    m_properties.getDepth(),
                                    desc.srcMip,
                                    srcWidth,
                                    srcHeight,
                                    srcDepth);

      uint32 dstWidth, dstHeight, dstDepth;
      PixelUtil::getSizeForMipLevel(target->m_properties.getWidth(),
                                    target->m_properties.getHeight(),
                                    target->m_properties.getDepth(),
                                    desc.dstMip,
                                    dstWidth,
                                    dstHeight,
                                    dstDepth);

      if (desc.dstPosition[0] < 0 || desc.dstPosition[0] >= static_cast<int32>(dstWidth) ||
          desc.dstPosition[1] < 0 || desc.dstPosition[1] >= static_cast<int32>(dstHeight) ||
          desc.dstPosition[2] < 0 || desc.dstPosition[2] >= static_cast<int32>(dstDepth)) {
        LOGERR("Destination position falls outside the destination texture.");
        return;
      }

      bool entireSurface = desc.srcVolume.getWidth() == 0 ||
                           desc.srcVolume.getHeight() == 0 ||
                           desc.srcVolume.getDepth() == 0;

      uint32 dstRight = static_cast<int32>(desc.dstPosition[0]);
      uint32 dstBottom = static_cast<int32>(desc.dstPosition[1]);
      uint32 dstBack = static_cast<int32>(desc.dstPosition[2]);
      if (!entireSurface) {
        if (desc.srcVolume.left >= srcWidth ||
            desc.srcVolume.right > srcWidth ||
            desc.srcVolume.top >= srcHeight ||
            desc.srcVolume.bottom > srcHeight ||
            desc.srcVolume.front >= srcDepth ||
            desc.srcVolume.back > srcDepth) {
          LOGERR("Source volume falls outside the source texture.");
          return;
        }

        dstRight += desc.srcVolume.getWidth();
        dstBottom += desc.srcVolume.getHeight();
        dstBack += desc.srcVolume.getDepth();
      }
      else {
        dstRight += srcWidth;
        dstBottom += srcHeight;
        dstBack += srcDepth;
      }

      if (dstRight > dstWidth || dstBottom > dstHeight || dstBack > dstDepth) {
        LOGERR("Destination volume falls outside the destination texture.");
        return;
      }

      copyImpl(target, desc, commandBuffer);
    }

    void
    Texture::clear(const LinearColor& value,
                   uint32 mipLevel,
                   uint32 face,
                   uint32 queueIdx) {
      THROW_IF_NOT_CORE_THREAD;

      if (m_properties.getNumFaces() <= face) {
        LOGERR("Invalid face index.");
        return;
      }

      if (m_properties.getNumMipmaps() < mipLevel) {
        LOGERR("Mip level out of range. Valid range is [0, " +
               toString(m_properties.getNumMipmaps()) + "].");
        return;
      }

      clearImpl(value, mipLevel, face, queueIdx);
    }

    void
    Texture::clearImpl(const LinearColor& value,
                       uint32 mipLevel,
                       uint32 face,
                       uint32 queueIdx) {
      SPtr<PixelData> data = m_properties.allocBuffer(face, mipLevel);
      data->setColors(value);
      writeData(*data, mipLevel, face, true, queueIdx);
    }

    /*************************************************************************/
    /**
     * Texture View
     */
    /*************************************************************************/

    SPtr<TextureView>
    Texture::createView(const TEXTURE_VIEW_DESC& desc) {
      return ge_shared_ptr<TextureView>(new (ge_alloc<TextureView>()) TextureView(desc));
    }

    void
    Texture::clearBufferViews() {
      m_textureViews.clear();
    }

    SPtr<TextureView>
    Texture::requestView(uint32 mostDetailMip,
                         uint32 numMips,
                         uint32 firstArraySlice,
                         uint32 numArraySlices,
                         GPU_VIEW_USAGE::E usage) {
      THROW_IF_NOT_CORE_THREAD;

      const TextureProperties& texProps = getProperties();

      TEXTURE_VIEW_DESC key;
      key.mostDetailMip = mostDetailMip;
      key.numMips = numMips == 0 ? (texProps.getNumMipmaps() + 1) : numMips;
      key.firstArraySlice = firstArraySlice;
      key.numArraySlices = numArraySlices == 0 ? texProps.getNumFaces() : numArraySlices;
      key.usage = usage;

      auto iterFind = m_textureViews.find(key);
      if (m_textureViews.end() == iterFind) {
        m_textureViews[key] = createView(key);
        iterFind = m_textureViews.find(key);
      }

      return iterFind->second;
    }

    /*************************************************************************/
    /**
     * Statics
     */
    /*************************************************************************/
    SPtr<Texture>
    Texture::create(const TEXTURE_DESC& desc, GPU_DEVICE_FLAGS::E deviceMask) {
      return TextureManager::instance().createTexture(desc, deviceMask);
    }

    SPtr<Texture>
    Texture::create(const SPtr<PixelData>& pixelData,
                    int32 usage,
                    bool hwGammaCorrection,
                    GPU_DEVICE_FLAGS::E deviceMask) {
      TEXTURE_DESC desc;
      desc.type = pixelData->getDepth() > 1 ? TEX_TYPE_3D : TEX_TYPE_2D;
      desc.width = pixelData->getWidth();
      desc.height = pixelData->getHeight();
      desc.depth = pixelData->getDepth();
      desc.format = pixelData->getFormat();
      desc.usage = usage;
      desc.hwGamma = hwGammaCorrection;

      SPtr<Texture> newTex =
        TextureManager::instance().createTextureInternal(desc, pixelData, deviceMask);
      newTex->initialize();

      return newTex;
    }
  }
}
