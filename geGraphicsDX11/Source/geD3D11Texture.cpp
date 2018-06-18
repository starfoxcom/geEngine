/*****************************************************************************/
/**
 * @file    geD3D11Texture.cpp
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2018/06/12
 * @brief   DirectX 11 implementation of a texture.
 *
 * DirectX 11 implementation of a texture.
 *
 * @bug     No known bugs.
 */
/*****************************************************************************/

/*****************************************************************************/
/**
 * Includes
 */
/*****************************************************************************/
#include "geD3D11Texture.h"

#include "geD3D11Mappings.h"
#include "geD3D11Device.h"
#include "geD3D11RenderAPI.h"
#include "geD3D11TextureView.h"
#include "geD3D11CommandBuffer.h"

#include <geCoreThread.h>
#include <geRenderStats.h>
#include <geException.h>
#include <geAsyncOp.h>
#include <geNumericLimits.h>
#include <geMath.h>

namespace geEngineSDK {
  using std::static_pointer_cast;

  namespace geCoreThread {
    D3D11Texture::D3D11Texture(const TEXTURE_DESC& desc,
                               const SPtr<PixelData>& initialData,
                               GPU_DEVICE_FLAGS::E deviceMask)
      : Texture(desc, initialData, deviceMask),
        m_tex1D(nullptr),
        m_tex2D(nullptr),
        m_tex3D(nullptr),
        m_dxgiFormat(DXGI_FORMAT_UNKNOWN),
        m_dxgiColorFormat(DXGI_FORMAT_UNKNOWN),
        m_tex(nullptr),
        m_internalFormat(PixelFormat::kUNKNOWN),
        m_stagingBuffer(nullptr),
        m_dxgiDepthStencilFormat(DXGI_FORMAT_UNKNOWN),
        m_lockedSubresourceIdx(NumLimit::MAX_UINT32),
        m_lockedForReading(false),
        m_staticBuffer(nullptr) {
      GE_ASSERT((deviceMask == GPU_DEVICE_FLAGS::kDEFAULT ||
                 deviceMask == GPU_DEVICE_FLAGS::kPRIMARY) &&
                "Multiple GPUs not supported natively on DirectX 11.");
    }

    D3D11Texture::~D3D11Texture() {
      clearBufferViews();

      SAFE_RELEASE(m_tex);
      SAFE_RELEASE(m_tex1D);
      SAFE_RELEASE(m_tex2D);
      SAFE_RELEASE(m_tex3D);
      SAFE_RELEASE(m_stagingBuffer);

      GE_INC_RENDER_STAT_CAT(ResDestroyed, RENDER_STAT_RESOURCE_TYPE::kTexture);
    }

    void
    D3D11Texture::initialize() {
      THROW_IF_NOT_CORE_THREAD;

      switch (m_properties.getTextureType())
      {
        case TEX_TYPE_1D:
          createTex1D();
          break;
        case TEX_TYPE_2D:
        case TEX_TYPE_CUBE_MAP:
          createTex2D();
          break;
        case TEX_TYPE_3D:
          createTex3D();
          break;
        default:
          GE_EXCEPT(RenderingAPIException, "Unknown texture type");
      }

      GE_INC_RENDER_STAT_CAT(ResCreated, RENDER_STAT_RESOURCE_TYPE::kTexture);
      Texture::initialize();
    }

    void
    D3D11Texture::copyImpl(const SPtr<Texture>& target,
                           const TEXTURE_COPY_DESC& desc,
                           const SPtr<CommandBuffer>& commandBuffer) {
      auto executeRef =
      [this](const SPtr<Texture>& target, const TEXTURE_COPY_DESC& desc)
      {
        auto other = static_cast<D3D11Texture*>(target.get());

        uint32 srcResIdx = D3D11CalcSubresource(desc.srcMip,
                                                desc.srcFace,
                                                m_properties.getNumMipmaps() + 1);
        uint32 destResIdx = D3D11CalcSubresource(desc.dstMip,
                                                 desc.dstFace,
                                                 target->getProperties().getNumMipmaps() + 1);

        auto rs = static_cast<D3D11RenderAPI*>(RenderAPI::instancePtr());
        D3D11Device& device = rs->getPrimaryDevice();
        ID3D11DeviceContext* immContext = device.getImmediateContext();

        bool srcHasMultisample = m_properties.getNumSamples() > 1;
        bool destHasMultisample = target->getProperties().getNumSamples() > 1;

        bool copyEntireSurface = desc.srcVolume.getWidth() == 0 ||
                                 desc.srcVolume.getHeight() == 0 ||
                                 desc.srcVolume.getDepth() == 0;

        if (srcHasMultisample && !destHasMultisample) { //Resolving from MS to non-MS texture
          if (copyEntireSurface) {
            immContext->ResolveSubresource(other->getDX11Resource(),
                                           destResIdx,
                                           m_tex,
                                           srcResIdx,
                                           m_dxgiFormat);
          }
          else {
            //Need to first resolve to a temporary texture, then copy
            TEXTURE_DESC tempDesc;
            tempDesc.width = m_properties.getWidth();
            tempDesc.height = m_properties.getHeight();
            tempDesc.format = m_properties.getFormat();
            tempDesc.hwGamma = m_properties.isHardwareGammaEnabled();

            auto temporary = static_pointer_cast<D3D11Texture>(Texture::create(tempDesc));
            immContext->ResolveSubresource(temporary->getDX11Resource(),
                                           0,
                                           m_tex,
                                           srcResIdx,
                                           m_dxgiFormat);

            TEXTURE_COPY_DESC tempCopyDesc;
            tempCopyDesc.dstMip = desc.dstMip;
            tempCopyDesc.dstFace = desc.dstFace;
            tempCopyDesc.dstPosition = desc.dstPosition;

            temporary->copy(target, tempCopyDesc);
          }
        }
        else {
          D3D11_BOX srcRegion;
          srcRegion.left = desc.srcVolume.left;
          srcRegion.right = desc.srcVolume.right;
          srcRegion.top = desc.srcVolume.top;
          srcRegion.bottom = desc.srcVolume.bottom;
          srcRegion.front = desc.srcVolume.front;
          srcRegion.back = desc.srcVolume.back;

          D3D11_BOX* srcRegionPtr = nullptr;
          if (!copyEntireSurface) {
            srcRegionPtr = &srcRegion;
          }

          immContext->CopySubresourceRegion(other->getDX11Resource(),
                                            destResIdx,
                                            static_cast<uint32>(desc.dstPosition.v[0]),
                                            static_cast<uint32>(desc.dstPosition.v[1]),
                                            static_cast<uint32>(desc.dstPosition.v[2]),
                                            m_tex,
                                            srcResIdx,
                                            srcRegionPtr);

          if (device.hasError()) {
            String errorDescription = device.getErrorDescription();
            GE_EXCEPT(RenderingAPIException,
                      "D3D11 device cannot copy subresource\n"
                      "Error Description:" + errorDescription);
          }
        }
      };

      if (nullptr == commandBuffer) {
        executeRef(target, desc);
      }
      else {
        auto execute = [=]() { executeRef(target, desc); };

        auto cb = static_pointer_cast<D3D11CommandBuffer>(commandBuffer);
        cb->queueCommand(execute);
      }
    }

    PixelData
    D3D11Texture::lockImpl(GPU_LOCK_OPTIONS::E options,
                           uint32 mipLevel,
                           uint32 face,
                           uint32 /*deviceIdx*/,
                           uint32 /*queueIdx*/) {
      if (m_properties.getNumSamples() > 1) {
        GE_EXCEPT(InvalidStateException,
          "Multisampled textures cannot be accessed from the CPU directly.");
      }

#if GE_PROFILING_ENABLED
      if (GPU_LOCK_OPTIONS::kREAD_ONLY == options ||
          GPU_LOCK_OPTIONS::kREAD_WRITE == options) {
        GE_INC_RENDER_STAT_CAT(ResRead, RENDER_STAT_RESOURCE_TYPE::kTexture);
      }

      if (GPU_LOCK_OPTIONS::kREAD_WRITE == options ||
          GPU_LOCK_OPTIONS::kWRITE_ONLY == options ||
          GPU_LOCK_OPTIONS::kWRITE_ONLY_DISCARD == options ||
          GPU_LOCK_OPTIONS::kWRITE_ONLY_NO_OVERWRITE == options) {
        GE_INC_RENDER_STAT_CAT(ResWrite, RENDER_STAT_RESOURCE_TYPE::kTexture);
      }
#endif
      uint32 mipWidth = Math::max(1u, m_properties.getWidth() >> mipLevel);
      uint32 mipHeight = Math::max(1u, m_properties.getHeight() >> mipLevel);
      uint32 mipDepth = Math::max(1u, m_properties.getDepth() >> mipLevel);

      PixelData lockedArea(mipWidth, mipHeight, mipDepth, m_internalFormat);

      D3D11_MAP flags = D3D11Mappings::getLockOptions(options);
      uint32 rowPitch, slicePitch;
      if (D3D11_MAP_READ == flags || D3D11_MAP_READ_WRITE == flags) {
        uint8* data = reinterpret_cast<uint8*>(mapStagingBuffer(flags,
                                                                face,
                                                                mipLevel,
                                                                rowPitch,
                                                                slicePitch));
        lockedArea.setExternalBuffer(data);

        if (PixelUtil::isCompressed(m_properties.getFormat())) {
          //Doesn't make sense to provide pitch values in pixels in this case
          lockedArea.setRowPitch(0);
          lockedArea.setSlicePitch(0);
        }
        else {
          uint32 bytesPerPixel = PixelUtil::getNumElemBytes(m_properties.getFormat());
          lockedArea.setRowPitch(rowPitch / bytesPerPixel);
          lockedArea.setSlicePitch(slicePitch / bytesPerPixel);
        }

        m_lockedForReading = true;
      }
      else {
        if ((m_properties.getUsage() & TU_DYNAMIC) != 0) {
          auto data = reinterpret_cast<uint8*>(map(m_tex,
                                                   flags,
                                                   face,
                                                   mipLevel,
                                                   rowPitch,
                                                   slicePitch));
          lockedArea.setExternalBuffer(data);

          if (PixelUtil::isCompressed(m_properties.getFormat())) {
            //Doesn't make sense to provide pitch values in pixels in this case
            lockedArea.setRowPitch(0);
            lockedArea.setSlicePitch(0);
          }
          else {
            uint32 bytesPerPixel = PixelUtil::getNumElemBytes(m_properties.getFormat());
            lockedArea.setRowPitch(rowPitch / bytesPerPixel);
            lockedArea.setSlicePitch(slicePitch / bytesPerPixel);
          }
        }
        else {
          lockedArea.setExternalBuffer(reinterpret_cast<uint8*>
                                         (mapStaticBuffer(lockedArea, mipLevel, face)));
        }

        m_lockedForReading = false;
      }

      return lockedArea;
    }

    void
    D3D11Texture::unlockImpl() {
      if (m_lockedForReading) {
        unmapStagingBuffer();
      }
      else {
        if ((m_properties.getUsage() & TU_DYNAMIC) != 0) {
          unmap(m_tex);
        }
        else {
          unmapStaticBuffer();
        }
      }
    }

    void
    D3D11Texture::readDataImpl(PixelData& dest,
                               uint32 mipLevel,
                               uint32 face,
                               uint32 deviceIdx,
                               uint32 queueIdx) {
      if (m_properties.getNumSamples() > 1) {
        LOGERR("Multisampled textures cannot be accessed from the CPU directly.");
        return;
      }

      PixelData myData = lock(GPU_LOCK_OPTIONS::kREAD_ONLY,
                              mipLevel,
                              face,
                              deviceIdx,
                              queueIdx);

      PixelUtil::bulkPixelConversion(myData, dest);
      unlock();
    }

    void
    D3D11Texture::writeDataImpl(const PixelData& src,
                                uint32 mipLevel,
                                uint32 face,
                                bool discardWholeBuffer,
                                uint32 queueIdx) {
      PixelFormat format = m_properties.getFormat();

      if (m_properties.getNumSamples() > 1) {
        LOGERR("Multisampled textures cannot be accessed from the CPU directly.");
        return;
      }

      //TODO: This is a residue from when we used -1 as a valid mip map level
      mipLevel = Math::clamp(mipLevel, mipLevel, m_properties.getNumMipmaps());

      face = Math::clamp(face, 0U, m_properties.getNumFaces() - 1);

      if (face > 0 && m_properties.getTextureType() == TEX_TYPE_3D) {
        LOGERR("3D texture arrays are not supported.");
        return;
      }

      if ((m_properties.getUsage() & TU_DYNAMIC) != 0) {
        PixelData myData = lock(discardWholeBuffer ?
                             GPU_LOCK_OPTIONS::kWRITE_ONLY_DISCARD :
                             GPU_LOCK_OPTIONS::kWRITE_ONLY, mipLevel, face, 0, queueIdx);
        PixelUtil::bulkPixelConversion(src, myData);
        unlock();
      }
      else if ((m_properties.getUsage() & TU_DEPTHSTENCIL) == 0) {
        auto rs = static_cast<D3D11RenderAPI*>(RenderAPI::instancePtr());
        D3D11Device& device = rs->getPrimaryDevice();

        auto subresourceIdx = D3D11CalcSubresource(mipLevel,
                                                   face,
                                                   m_properties.getNumMipmaps() + 1);
        uint32 rowWidth = D3D11Mappings::getSizeInBytes(format, src.getWidth());
        uint32 sliceWidth = D3D11Mappings::getSizeInBytes(format,
                                                          src.getWidth(),
                                                          src.getHeight());

        device.getImmediateContext()->UpdateSubresource(m_tex,
                                                        subresourceIdx,
                                                        nullptr,
                                                        src.getData(),
                                                        rowWidth,
                                                        sliceWidth);

        if (device.hasError()) {
          String errorDescription = device.getErrorDescription();
          GE_EXCEPT(RenderingAPIException,
                    "D3D11 device cannot map texture\n"
                    "Error Description:" + errorDescription);
        }

        GE_INC_RENDER_STAT_CAT(ResWrite, RENDER_STAT_RESOURCE_TYPE::kTexture);
      }
      else {
        GE_EXCEPT(RenderingAPIException,
                  "Trying to write into a buffer with unsupported usage: " +
                  toString(m_properties.getUsage()));
      }
    }

    void
    D3D11Texture::createTex1D() {
      uint32 width = m_properties.getWidth();
      int32 usage = m_properties.getUsage();
      uint32 numMips = m_properties.getNumMipmaps();
      auto format = m_properties.getFormat();
      bool hwGamma = m_properties.isHardwareGammaEnabled();
      auto closestFormat = D3D11Mappings::getClosestSupportedPF(format, TEX_TYPE_1D, usage);
      uint32 numFaces = m_properties.getNumFaces();

      //We must have those defined here
      GE_ASSERT(width > 0);

      //Determine which D3D11 pixel format we'll use
      HRESULT hr;
      DXGI_FORMAT d3dPF = D3D11Mappings::getPF(closestFormat, hwGamma);

      if (closestFormat != format) {
        LOGWRN(StringUtil::format("Provided pixel format is not supported by "
                                    "the driver: {0}. Falling back on: {1}.",
                                  format,
                                  closestFormat));
      }

      m_internalFormat = closestFormat;
      m_dxgiColorFormat = d3dPF;
      m_dxgiDepthStencilFormat = d3dPF;

      //TODO - Consider making this a parameter eventually
      bool readableDepth = true;

      D3D11_TEXTURE1D_DESC desc;
      desc.Width = static_cast<uint32>(width);
      desc.ArraySize = numFaces == 0 ? 1 : numFaces;
      desc.Format = d3dPF;
      desc.MiscFlags = 0;

      if ((usage & TU_RENDERTARGET) != 0) {
        desc.Usage = D3D11_USAGE_DEFAULT;
        desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
        desc.CPUAccessFlags = 0;
        desc.MipLevels = 1;
      }
      else if ((usage & TU_DEPTHSTENCIL) != 0) {
        desc.Usage = D3D11_USAGE_DEFAULT;

        if (readableDepth) {
          desc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
        }
        else {
          desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
        }

        desc.CPUAccessFlags = 0;
        desc.MipLevels = 1;
        desc.Format = D3D11Mappings::getTypelessDepthStencilPF(closestFormat);

        m_dxgiColorFormat = D3D11Mappings::getShaderResourceDepthStencilPF(closestFormat);
        m_dxgiDepthStencilFormat = d3dPF;
      }
      else {
        desc.Usage = D3D11Mappings::getUsage((GPU_BUFFER_USAGE::E)usage);
        desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
        desc.CPUAccessFlags = D3D11Mappings::getAccessFlags((GPU_BUFFER_USAGE::E)usage);

        //Determine total number of mipmaps including main one (d3d11 convention)
        desc.MipLevels = (numMips == TEXTURE_MIPMAP::kUNLIMITED ||
                          (1U << numMips) > width) ? 0 : numMips + 1;
      }

      if ((usage & TU_LOADSTORE) != 0) {
        desc.BindFlags |= D3D11_BIND_UNORDERED_ACCESS;
      }

      //Create the texture
      auto rs = static_cast<D3D11RenderAPI*>(RenderAPI::instancePtr());
      D3D11Device& device = rs->getPrimaryDevice();
      hr = device.getD3D11Device()->CreateTexture1D(&desc, nullptr, &m_tex1D);

      //Check result and except if failed
      if (FAILED(hr) || device.hasError()) {
        String errorDescription = device.getErrorDescription();
        GE_EXCEPT(RenderingAPIException,
                  "Error creating texture\n"
                  "Error Description:" + errorDescription);
      }

      hr = m_tex1D->QueryInterface(__uuidof(ID3D11Resource),
                                   reinterpret_cast<void**>(&m_tex));

      if (FAILED(hr) || device.hasError()) {
        String errorDescription = device.getErrorDescription();
        GE_EXCEPT(RenderingAPIException,
                  "Can't get base texture\n"
                  "Error Description:" + errorDescription);
      }

      m_tex1D->GetDesc(&desc);

      if (numMips != (desc.MipLevels - 1)) {
        GE_EXCEPT(RenderingAPIException,
                  "Driver returned different number of mip maps than "
                  "requested. Requested: " + toString(numMips) + ". Got: " +
                  toString(desc.MipLevels - 1) + ".");
      }

      m_dxgiFormat = desc.Format;

      //Create texture view
      if ((usage & TU_DEPTHSTENCIL) == 0 || readableDepth) {
        TEXTURE_VIEW_DESC viewDesc;
        viewDesc.mostDetailMip = 0;
        viewDesc.numMips = desc.MipLevels;
        viewDesc.firstArraySlice = 0;
        viewDesc.numArraySlices = desc.ArraySize;
        viewDesc.usage = GPU_VIEW_USAGE::kDEFAULT;

        m_shaderResourceView = ge_shared_ptr<D3D11TextureView>
                                 (GE_PVT_NEW(D3D11TextureView, this, viewDesc));
      }
    }

    void
    D3D11Texture::createTex2D() {
      auto width = m_properties.getWidth();
      auto height = m_properties.getHeight();
      auto usage = m_properties.getUsage();
      auto numMips = m_properties.getNumMipmaps();
      auto format = m_properties.getFormat();
      auto hwGamma = m_properties.isHardwareGammaEnabled();
      auto sampleCount = m_properties.getNumSamples();
      auto texType = m_properties.getTextureType();
      auto closestFormat = D3D11Mappings::getClosestSupportedPF(format, texType, usage);
      auto numFaces = m_properties.getNumFaces();

      //TODO: Consider making this a parameter eventually
      bool readableDepth = true;

      //We must have those defined here
      GE_ASSERT(width > 0 || height > 0);

      //Determine which D3D11 pixel format we'll use
      HRESULT hr;
      DXGI_FORMAT d3dPF = D3D11Mappings::getPF(closestFormat, hwGamma);

      if (closestFormat != format) {
        LOGWRN(StringUtil::format("Provided pixel format is not supported by "
                                  "the driver: {0}. Falling back on: {1}.",
                                  format,
                                  closestFormat));
      }

      m_internalFormat = closestFormat;
      m_dxgiColorFormat = d3dPF;
      m_dxgiDepthStencilFormat = d3dPF;

      D3D11_TEXTURE2D_DESC desc;
      desc.Width = width;
      desc.Height = height;
      desc.ArraySize = numFaces == 0 ? 1 : numFaces;
      desc.Format = d3dPF;
      desc.MiscFlags = 0;

      if ((usage & TU_RENDERTARGET) != 0) {
        desc.Usage = D3D11_USAGE_DEFAULT;
        
        //TODO: Add flags to allow RT be created without shader resource flags
        //(might be more optimal)
        desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
        desc.CPUAccessFlags = 0;

        DXGI_SAMPLE_DESC sampleDesc;
        auto rs = static_cast<D3D11RenderAPI*>(RenderAPI::instancePtr());
        rs->determineMultisampleSettings(sampleCount, d3dPF, &sampleDesc);
        desc.SampleDesc = sampleDesc;
      }
      else if ((usage & TU_DEPTHSTENCIL) != 0) {
        desc.Usage = D3D11_USAGE_DEFAULT;
        desc.CPUAccessFlags = 0;
        desc.Format = D3D11Mappings::getTypelessDepthStencilPF(closestFormat);

        if (readableDepth) {
          desc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
        }
        else {
          desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
        }

        DXGI_SAMPLE_DESC sampleDesc;
        auto rs = static_cast<D3D11RenderAPI*>(RenderAPI::instancePtr());
        rs->determineMultisampleSettings(sampleCount, d3dPF, &sampleDesc);
        desc.SampleDesc = sampleDesc;

        m_dxgiColorFormat = D3D11Mappings::getShaderResourceDepthStencilPF(closestFormat);
        m_dxgiDepthStencilFormat = d3dPF;
      }
      else {
        desc.Usage = D3D11Mappings::getUsage((GPU_BUFFER_USAGE::E)usage);
        desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
        desc.CPUAccessFlags = D3D11Mappings::getAccessFlags((GPU_BUFFER_USAGE::E)usage);

        DXGI_SAMPLE_DESC sampleDesc;
        sampleDesc.Count = 1;
        sampleDesc.Quality = 0;
        desc.SampleDesc = sampleDesc;
      }

      //Determine total number of mipmaps including main one (d3d11 convention)
      desc.MipLevels = (TEXTURE_MIPMAP::kUNLIMITED == numMips ||
                        (1U << numMips) > width) ? 0 : numMips + 1;

      if (TEX_TYPE_CUBE_MAP == texType) {
        desc.MiscFlags |= D3D11_RESOURCE_MISC_TEXTURECUBE;
      }

      if ((usage & TU_LOADSTORE) != 0) {
        if (desc.SampleDesc.Count <= 1) {
          desc.BindFlags |= D3D11_BIND_UNORDERED_ACCESS;
        }
        else {
          LOGWRN("Unable to create a load-store texture with multiple "
                 "samples. This is not supported on DirectX 11. Ignoring "
                 "load-store usage flag.");
        }
      }

      //Create the texture
      auto rs = static_cast<D3D11RenderAPI*>(RenderAPI::instancePtr());
      D3D11Device& device = rs->getPrimaryDevice();
      hr = device.getD3D11Device()->CreateTexture2D(&desc, nullptr, &m_tex2D);

      //Check result and except if failed
      if (FAILED(hr) || device.hasError()) {
        String errorDescription = device.getErrorDescription();
        GE_EXCEPT(RenderingAPIException,
                  "Error creating texture\n"
                  "Error Description:" + errorDescription);
      }

      hr = m_tex2D->QueryInterface(__uuidof(ID3D11Resource),
                                   reinterpret_cast<void**>(&m_tex));

      if (FAILED(hr) || device.hasError()) {
        String errorDescription = device.getErrorDescription();
        GE_EXCEPT(RenderingAPIException,
                  "Can't get base texture\n"
                  "Error Description:" + errorDescription);
      }

      m_tex2D->GetDesc(&desc);

      if (numMips != (desc.MipLevels - 1)) {
        GE_EXCEPT(RenderingAPIException,
                  "Driver returned different number of mip maps than "
                  "requested. Requested: " + toString(numMips) + ". Got: " +
                  toString(desc.MipLevels - 1) + ".");
      }

      m_dxgiFormat = desc.Format;

      //Create shader texture view
      if ((usage & TU_DEPTHSTENCIL) == 0 || readableDepth) {
        TEXTURE_VIEW_DESC viewDesc;
        viewDesc.mostDetailMip = 0;
        viewDesc.numMips = desc.MipLevels;
        viewDesc.firstArraySlice = 0;
        viewDesc.numArraySlices = desc.ArraySize;
        viewDesc.usage = GPU_VIEW_USAGE::kDEFAULT;

        m_shaderResourceView = ge_shared_ptr<D3D11TextureView>
                                 (GE_PVT_NEW(D3D11TextureView, this, viewDesc));
      }
    }

    void
    D3D11Texture::createTex3D() {
      auto width = m_properties.getWidth();
      auto height = m_properties.getHeight();
      auto depth = m_properties.getDepth();
      auto usage = m_properties.getUsage();
      auto numMips = m_properties.getNumMipmaps();
      auto format = m_properties.getFormat();
      auto hwGamma = m_properties.isHardwareGammaEnabled();
      auto closestFormat = D3D11Mappings::getClosestSupportedPF(format, TEX_TYPE_3D, usage);

      //TODO: Consider making this a parameter eventually
      bool readableDepth = true;

      //We must have those defined here
      GE_ASSERT(width > 0 && height > 0 && depth > 0);

      //Determine which D3D11 pixel format we'll use
      HRESULT hr;
      DXGI_FORMAT d3dPF = D3D11Mappings::getPF(closestFormat, hwGamma);

      if (closestFormat != format) {
        LOGWRN(StringUtil::format("Provided pixel format is not supported by "
                                  "the driver: {0}. Falling back on: {1}.",
                                  format,
                                  closestFormat));
      }

      m_internalFormat = closestFormat;
      m_dxgiColorFormat = d3dPF;
      m_dxgiDepthStencilFormat = d3dPF;

      D3D11_TEXTURE3D_DESC desc;
      desc.Width = width;
      desc.Height = height;
      desc.Depth = depth;
      desc.Format = d3dPF;
      desc.MiscFlags = 0;

      if ((m_properties.getUsage() & TU_RENDERTARGET) != 0) {
        desc.Usage = D3D11_USAGE_DEFAULT;
        desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
        desc.CPUAccessFlags = 0;
        desc.MipLevels = 1;
      }
      else if ((m_properties.getUsage() & TU_DEPTHSTENCIL) != 0) {
        desc.Usage = D3D11_USAGE_DEFAULT;
        desc.CPUAccessFlags = 0;
        desc.MipLevels = 1;

        if (readableDepth) {
          desc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
        }
        else {
          desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
        }

        m_dxgiColorFormat = D3D11Mappings::getShaderResourceDepthStencilPF(closestFormat);
        m_dxgiDepthStencilFormat = d3dPF;
      }
      else {
        desc.Usage = D3D11Mappings::getUsage(static_cast<GPU_BUFFER_USAGE::E>(usage));
        desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
        desc.CPUAccessFlags = D3D11Mappings::getAccessFlags
                                (static_cast<GPU_BUFFER_USAGE::E>(usage));

        //Determine total number of mipmaps including main one
        //(d3d11 convention)
        desc.MipLevels = (TEXTURE_MIPMAP::kUNLIMITED == numMips ||
                         (1U << numMips) > Math::max3(width, height, depth)) ?
                         0 : numMips + 1;
      }

      if ((usage & TU_LOADSTORE) != 0) {
        desc.BindFlags |= D3D11_BIND_UNORDERED_ACCESS;
      }

      //Create the texture
      auto rs = static_cast<D3D11RenderAPI*>(RenderAPI::instancePtr());
      D3D11Device& device = rs->getPrimaryDevice();
      hr = device.getD3D11Device()->CreateTexture3D(&desc, nullptr, &m_tex3D);

      //Check result and except if failed
      if (FAILED(hr) || device.hasError()) {
        String errorDescription = device.getErrorDescription();
        GE_EXCEPT(RenderingAPIException,
                  "Error creating texture\n"
                  "Error Description:" + errorDescription);
      }

      hr = m_tex3D->QueryInterface(__uuidof(ID3D11Resource),
                                   reinterpret_cast<void**>(&m_tex));

      if (FAILED(hr) || device.hasError()) {
        String errorDescription = device.getErrorDescription();
        GE_EXCEPT(RenderingAPIException,
                  "Can't get base texture\n"
                  "Error Description:" + errorDescription);
      }

      //Create texture view
      m_tex3D->GetDesc(&desc);

      if (m_properties.getNumMipmaps() != (desc.MipLevels - 1)) {
        GE_EXCEPT(RenderingAPIException,
                  "Driver returned different number of mip maps than "
                  "requested. Requested: " +
                  toString(m_properties.getNumMipmaps()) +
                  ". Got: " + toString(desc.MipLevels - 1) + ".");
      }

      m_dxgiFormat = desc.Format;

      if ((usage & TU_DEPTHSTENCIL) == 0 || readableDepth) {
        TEXTURE_VIEW_DESC viewDesc;
        viewDesc.mostDetailMip = 0;
        viewDesc.numMips = desc.MipLevels;
        viewDesc.firstArraySlice = 0;
        viewDesc.numArraySlices = 1;
        viewDesc.usage = GPU_VIEW_USAGE::kDEFAULT;

        m_shaderResourceView = ge_shared_ptr<D3D11TextureView>
                                 (GE_PVT_NEW(D3D11TextureView, this, viewDesc));
      }
    }

    void*
    D3D11Texture::map(ID3D11Resource* res,
                      D3D11_MAP flags,
                      uint32 mipLevel,
                      uint32 face,
                      uint32& rowPitch,
                      uint32& slicePitch) {
      D3D11_MAPPED_SUBRESOURCE pMappedResource;
      pMappedResource.pData = nullptr;

      //TODO: This is a residue from when we used -1 as a valid mip map level
      mipLevel = Math::clamp(mipLevel, mipLevel, m_properties.getNumMipmaps());

      face = Math::clamp(face, 0U, m_properties.getNumFaces() - 1);

      if (m_properties.getTextureType() == TEX_TYPE_3D) {
        face = 0;
      }

      auto rs = static_cast<D3D11RenderAPI*>(RenderAPI::instancePtr());
      D3D11Device& device = rs->getPrimaryDevice();

      m_lockedSubresourceIdx = D3D11CalcSubresource(mipLevel,
                                                    face,
                                                    m_properties.getNumMipmaps() + 1);
      device.getImmediateContext()->Map(res,
                                        m_lockedSubresourceIdx,
                                        flags,
                                        0,
                                        &pMappedResource);

      if (device.hasError()) {
        String errorDescription = device.getErrorDescription();
        GE_EXCEPT(RenderingAPIException,
                  "D3D11 device cannot map texture\n"
                  "Error Description:" + errorDescription);
      }

      rowPitch = pMappedResource.RowPitch;
      slicePitch = pMappedResource.DepthPitch;

      return pMappedResource.pData;
    }

    void
    D3D11Texture::unmap(ID3D11Resource* res) {
      auto rs = static_cast<D3D11RenderAPI*>(RenderAPI::instancePtr());
      D3D11Device& device = rs->getPrimaryDevice();
      device.getImmediateContext()->Unmap(res, m_lockedSubresourceIdx);

      if (device.hasError()) {
        String errorDescription = device.getErrorDescription();
        GE_EXCEPT(RenderingAPIException,
                  "D3D11 device unmap resource\n"
                  "Error Description:" + errorDescription);
      }
    }

    void*
    D3D11Texture::mapStagingBuffer(D3D11_MAP flags,
                                   uint32 mipLevel,
                                   uint32 face,
                                   uint32& rowPitch,
                                   uint32& slicePitch) {
      //NOTE: I am creating and destroying a staging resource every time a
      //texture is read. Consider offering a flag on init that will keep this
      //active all the time (at the cost of double memory).
      //Reading is a slow operation anyway so I don't believe doing it as we
      //are now will influence it much.

      if (!m_stagingBuffer) {
        createStagingBuffer();
      }

      auto rs = static_cast<D3D11RenderAPI*>(RenderAPI::instancePtr());
      D3D11Device& device = rs->getPrimaryDevice();
      device.getImmediateContext()->CopyResource(m_stagingBuffer, m_tex);

      return map(m_stagingBuffer, flags, face, mipLevel, rowPitch, slicePitch);
    }

    void
    D3D11Texture::unmapStagingBuffer() {
      unmap(m_stagingBuffer);
      SAFE_RELEASE(m_stagingBuffer);
    }

    void*
    D3D11Texture::mapStaticBuffer(const PixelData& lock, uint32 mipLevel, uint32 face) {
      //uint32 sizeOfImage = lock.getConsecutiveSize();
      m_lockedSubresourceIdx = D3D11CalcSubresource(mipLevel,
                                                    face,
                                                    m_properties.getNumMipmaps() + 1);

      m_staticBuffer = ge_new<PixelData>(lock.getWidth(),
                                         lock.getHeight(),
                                         lock.getDepth(),
                                         lock.getFormat());

      m_staticBuffer->allocateInternalBuffer();

      return m_staticBuffer->getData();
    }

    void
    D3D11Texture::unmapStaticBuffer() {
      uint32 rowWidth = D3D11Mappings::getSizeInBytes(m_staticBuffer->getFormat(),
                                                      m_staticBuffer->getWidth());

      uint32 sliceWidth = D3D11Mappings::getSizeInBytes(m_staticBuffer->getFormat(),
                                                        m_staticBuffer->getWidth(),
                                                        m_staticBuffer->getHeight());

      auto rs = static_cast<D3D11RenderAPI*>(RenderAPI::instancePtr());
      D3D11Device& device = rs->getPrimaryDevice();
      device.getImmediateContext()->UpdateSubresource(m_tex,
                                                      m_lockedSubresourceIdx,
                                                      nullptr,
                                                      m_staticBuffer->getData(),
                                                      rowWidth,
                                                      sliceWidth);

      if (device.hasError()) {
        String errorDescription = device.getErrorDescription();
        GE_EXCEPT(RenderingAPIException,
                  "D3D11 device cannot map texture\n"
                  "Error Description:" + errorDescription);
      }

      if (nullptr != m_staticBuffer) {
        ge_delete(m_staticBuffer);
      }
    }

    ID3D11ShaderResourceView*
    D3D11Texture::getSRV() const {
      return m_shaderResourceView->getSRV();
    }

    void
    D3D11Texture::createStagingBuffer() {
      auto rs = static_cast<D3D11RenderAPI*>(RenderAPI::instancePtr());
      D3D11Device& device = rs->getPrimaryDevice();
      ID3D11Device* pDevice = device.getD3D11Device();
      switch (m_properties.getTextureType())
      {
        case TEX_TYPE_1D:
        {
          D3D11_TEXTURE1D_DESC desc;
          m_tex1D->GetDesc(&desc);

          desc.BindFlags = 0;
          desc.MiscFlags = 0;
          desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE | D3D11_CPU_ACCESS_READ;
          desc.Usage = D3D11_USAGE_STAGING;

          pDevice->CreateTexture1D(&desc,
                                   nullptr,
                                   reinterpret_cast<ID3D11Texture1D**>(&m_stagingBuffer));
        }
        break;
        case TEX_TYPE_2D:
        case TEX_TYPE_CUBE_MAP:
        {
          D3D11_TEXTURE2D_DESC desc;
          m_tex2D->GetDesc(&desc);

          desc.BindFlags = 0;
          desc.MiscFlags = 0;
          desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE | D3D11_CPU_ACCESS_READ;
          desc.Usage = D3D11_USAGE_STAGING;

          pDevice->CreateTexture2D(&desc,
                                   nullptr,
                                   reinterpret_cast<ID3D11Texture2D**>(&m_stagingBuffer));
        }
        break;
        case TEX_TYPE_3D:
        {
          D3D11_TEXTURE3D_DESC desc;
          m_tex3D->GetDesc(&desc);

          desc.BindFlags = 0;
          desc.MiscFlags = 0;
          desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE | D3D11_CPU_ACCESS_READ;
          desc.Usage = D3D11_USAGE_STAGING;

          pDevice->CreateTexture3D(&desc,
                                   nullptr,
                                   reinterpret_cast<ID3D11Texture3D**>(&m_stagingBuffer));
        }
        break;
      }
    }

    SPtr<TextureView>
    D3D11Texture::createView(const TEXTURE_VIEW_DESC& desc) {
      return ge_shared_ptr<D3D11TextureView>(GE_PVT_NEW(D3D11TextureView, this, desc));
    }
  }
}
