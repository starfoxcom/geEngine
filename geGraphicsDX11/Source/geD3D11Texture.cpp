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
#include <geMath.h>

namespace geEngineSDK {
  namespace geCoreThread {
    D3D11Texture::D3D11Texture(const TEXTURE_DESC& desc,
                               const SPtr<PixelData>& initialData,
                               GPU_DEVICE_FLAGS::E deviceMask)
      : Texture(desc, initialData, deviceMask),
      m1DTex(nullptr), m2DTex(nullptr), m3DTex(nullptr), mDXGIFormat(DXGI_FORMAT_UNKNOWN), mDXGIColorFormat(DXGI_FORMAT_UNKNOWN),
      mTex(nullptr), mInternalFormat(PixelFormat::kUNKNOWN), mStagingBuffer(nullptr), mDXGIDepthStencilFormat(DXGI_FORMAT_UNKNOWN),
      mLockedSubresourceIdx(-1), mLockedForReading(false), mStaticBuffer(nullptr)
    {
      GE_ASSERT((deviceMask == GPU_DEVICE_FLAGS::kDEFAULT || deviceMask == GPU_DEVICE_FLAGS::kPRIMARY) && "Multiple GPUs not supported natively on DirectX 11.");
    }

    D3D11Texture::~D3D11Texture()
    {
      clearBufferViews();

      SAFE_RELEASE(mTex);
      SAFE_RELEASE(m1DTex);
      SAFE_RELEASE(m2DTex);
      SAFE_RELEASE(m3DTex);
      SAFE_RELEASE(mStagingBuffer);

      GE_INC_RENDER_STAT_CAT(ResDestroyed, RenderStatObject_Texture);
    }

    void D3D11Texture::initialize()
    {
      THROW_IF_NOT_CORE_THREAD;

      switch (m_properties.getTextureType())
      {
      case TEX_TYPE_1D:
        create1DTex();
        break;
      case TEX_TYPE_2D:
      case TEX_TYPE_CUBE_MAP:
        create2DTex();
        break;
      case TEX_TYPE_3D:
        create3DTex();
        break;
      default:
        GE_EXCEPT(RenderingAPIException, "Unknown texture type");
      }

      GE_INC_RENDER_STAT_CAT(ResCreated, RenderStatObject_Texture);
      Texture::initialize();
    }

    void D3D11Texture::copyImpl(const SPtr<Texture>& target, const TEXTURE_COPY_DESC& desc,
      const SPtr<CommandBuffer>& commandBuffer)
    {
      auto executeRef = [this](const SPtr<Texture>& target, const TEXTURE_COPY_DESC& desc)
      {
        D3D11Texture* other = static_cast<D3D11Texture*>(target.get());

        uint32 srcResIdx = D3D11CalcSubresource(desc.srcMip, desc.srcFace, m_properties.getNumMipmaps() + 1);
        uint32 destResIdx = D3D11CalcSubresource(desc.dstMip, desc.dstFace, target->getProperties().getNumMipmaps() + 1);

        D3D11RenderAPI* rs = static_cast<D3D11RenderAPI*>(RenderAPI::instancePtr());
        D3D11Device& device = rs->getPrimaryDevice();

        bool srcHasMultisample = m_properties.getNumSamples() > 1;
        bool destHasMultisample = target->getProperties().getNumSamples() > 1;

        bool copyEntireSurface = desc.srcVolume.getWidth() == 0 ||
          desc.srcVolume.getHeight() == 0 ||
          desc.srcVolume.getDepth() == 0;

        if (srcHasMultisample && !destHasMultisample) // Resolving from MS to non-MS texture
        {
          if (copyEntireSurface)
            device.getImmediateContext()->ResolveSubresource(other->getDX11Resource(), destResIdx, mTex, srcResIdx, mDXGIFormat);
          else
          {
            // Need to first resolve to a temporary texture, then copy
            TEXTURE_DESC tempDesc;
            tempDesc.width = m_properties.getWidth();
            tempDesc.height = m_properties.getHeight();
            tempDesc.format = m_properties.getFormat();
            tempDesc.hwGamma = m_properties.isHardwareGammaEnabled();

            SPtr<D3D11Texture> temporary = std::static_pointer_cast<D3D11Texture>(Texture::create(tempDesc));
            device.getImmediateContext()->ResolveSubresource(temporary->getDX11Resource(), 0, mTex, srcResIdx, mDXGIFormat);

            TEXTURE_COPY_DESC tempCopyDesc;
            tempCopyDesc.dstMip = desc.dstMip;
            tempCopyDesc.dstFace = desc.dstFace;
            tempCopyDesc.dstPosition = desc.dstPosition;

            temporary->copy(target, tempCopyDesc);
          }
        }
        else
        {
          D3D11_BOX srcRegion;
          srcRegion.left = desc.srcVolume.left;
          srcRegion.right = desc.srcVolume.right;
          srcRegion.top = desc.srcVolume.top;
          srcRegion.bottom = desc.srcVolume.bottom;
          srcRegion.front = desc.srcVolume.front;
          srcRegion.back = desc.srcVolume.back;

          D3D11_BOX* srcRegionPtr = nullptr;
          if (!copyEntireSurface)
            srcRegionPtr = &srcRegion;

          device.getImmediateContext()->CopySubresourceRegion(
            other->getDX11Resource(),
            destResIdx,
            (uint32)desc.dstPosition.v[0],
            (uint32)desc.dstPosition.v[1],
            (uint32)desc.dstPosition.v[2],
            mTex,
            srcResIdx,
            srcRegionPtr);

          if (device.hasError())
          {
            String errorDescription = device.getErrorDescription();
            GE_EXCEPT(RenderingAPIException, "D3D11 device cannot copy subresource\nError Description:" + errorDescription);
          }
        }
      };

      if (commandBuffer == nullptr)
        executeRef(target, desc);
      else
      {
        auto execute = [=]() { executeRef(target, desc); };

        SPtr<D3D11CommandBuffer> cb = std::static_pointer_cast<D3D11CommandBuffer>(commandBuffer);
        cb->queueCommand(execute);
      }
    }

    PixelData D3D11Texture::lockImpl(GPU_LOCK_OPTIONS::E options, uint32 mipLevel, uint32 face, uint32 deviceIdx,
      uint32 queueIdx)
    {
      if (m_properties.getNumSamples() > 1)
        GE_EXCEPT(InvalidStateException, "Multisampled textures cannot be accessed from the CPU directly.");

#if GE_PROFILING_ENABLED
      if (options == GPU_LOCK_OPTIONS::kREAD_ONLY || options == GPU_LOCK_OPTIONS::kREAD_WRITE)
      {
        GE_INC_RENDER_STAT_CAT(ResRead, RenderStatObject_Texture);
      }

      if (options == GPU_LOCK_OPTIONS::kREAD_WRITE || options == GPU_LOCK_OPTIONS::kWRITE_ONLY || options == GPU_LOCK_OPTIONS::kWRITE_ONLY_DISCARD || options == GPU_LOCK_OPTIONS::kWRITE_ONLY_NO_OVERWRITE)
      {
        GE_INC_RENDER_STAT_CAT(ResWrite, RenderStatObject_Texture);
      }
#endif

      uint32 mipWidth = std::max(1u, m_properties.getWidth() >> mipLevel);
      uint32 mipHeight = std::max(1u, m_properties.getHeight() >> mipLevel);
      uint32 mipDepth = std::max(1u, m_properties.getDepth() >> mipLevel);

      PixelData lockedArea(mipWidth, mipHeight, mipDepth, mInternalFormat);

      D3D11_MAP flags = D3D11Mappings::getLockOptions(options);
      uint32 rowPitch, slicePitch;
      if (flags == D3D11_MAP_READ || flags == D3D11_MAP_READ_WRITE)
      {
        uint8* data = (uint8*)mapstagingbuffer(flags, face, mipLevel, rowPitch, slicePitch);
        lockedArea.setExternalBuffer(data);

        if (PixelUtil::isCompressed(m_properties.getFormat()))
        {
          // Doesn't make sense to provide pitch values in pixels in this case
          lockedArea.setRowPitch(0);
          lockedArea.setSlicePitch(0);
        }
        else
        {
          uint32 bytesPerPixel = PixelUtil::getNumElemBytes(m_properties.getFormat());

          lockedArea.setRowPitch(rowPitch / bytesPerPixel);
          lockedArea.setSlicePitch(slicePitch / bytesPerPixel);
        }

        mLockedForReading = true;
      }
      else
      {
        if ((m_properties.getUsage() & TU_DYNAMIC) != 0)
        {
          uint8* data = (uint8*)map(mTex, flags, face, mipLevel, rowPitch, slicePitch);
          lockedArea.setExternalBuffer(data);

          if (PixelUtil::isCompressed(m_properties.getFormat()))
          {
            // Doesn't make sense to provide pitch values in pixels in this case
            lockedArea.setRowPitch(0);
            lockedArea.setSlicePitch(0);
          }
          else
          {
            uint32 bytesPerPixel = PixelUtil::getNumElemBytes(m_properties.getFormat());

            lockedArea.setRowPitch(rowPitch / bytesPerPixel);
            lockedArea.setSlicePitch(slicePitch / bytesPerPixel);
          }
        }
        else
          lockedArea.setExternalBuffer((uint8*)mapstaticbuffer(lockedArea, mipLevel, face));

        mLockedForReading = false;
      }

      return lockedArea;
    }

    void D3D11Texture::unlockImpl()
    {
      if (mLockedForReading)
        unmapstagingbuffer();
      else
      {
        if ((m_properties.getUsage() & TU_DYNAMIC) != 0)
          unmap(mTex);
        else
          unmapstaticbuffer();
      }
    }

    void D3D11Texture::readDataImpl(PixelData& dest, uint32 mipLevel, uint32 face, uint32 deviceIdx, uint32 queueIdx)
    {
      if (m_properties.getNumSamples() > 1)
      {
        LOGERR("Multisampled textures cannot be accessed from the CPU directly.");
        return;
      }

      PixelData myData = lock(GPU_LOCK_OPTIONS::kREAD_ONLY, mipLevel, face, deviceIdx, queueIdx);
      PixelUtil::bulkPixelConversion(myData, dest);
      unlock();
    }

    void D3D11Texture::writeDataImpl(const PixelData& src, uint32 mipLevel, uint32 face, bool discardWholeBuffer,
      uint32 queueIdx)
    {
      PixelFormat format = m_properties.getFormat();

      if (m_properties.getNumSamples() > 1)
      {
        LOGERR("Multisampled textures cannot be accessed from the CPU directly.");
        return;
      }

      mipLevel = Math::clamp(mipLevel, (uint32)mipLevel, m_properties.getNumMipmaps());
      face = Math::clamp(face, (uint32)0, m_properties.getNumFaces() - 1);

      if (face > 0 && m_properties.getTextureType() == TEX_TYPE_3D)
      {
        LOGERR("3D texture arrays are not supported.");
        return;
      }

      if ((m_properties.getUsage() & TU_DYNAMIC) != 0)
      {
        PixelData myData = lock(discardWholeBuffer ? GPU_LOCK_OPTIONS::kWRITE_ONLY_DISCARD : GPU_LOCK_OPTIONS::kWRITE_ONLY, mipLevel, face, 0, queueIdx);
        PixelUtil::bulkPixelConversion(src, myData);
        unlock();
      }
      else if ((m_properties.getUsage() & TU_DEPTHSTENCIL) == 0)
      {
        D3D11RenderAPI* rs = static_cast<D3D11RenderAPI*>(RenderAPI::instancePtr());
        D3D11Device& device = rs->getPrimaryDevice();

        UINT subresourceIdx = D3D11CalcSubresource(mipLevel, face, m_properties.getNumMipmaps() + 1);
        uint32 rowWidth = D3D11Mappings::getSizeInBytes(format, src.getWidth());
        uint32 sliceWidth = D3D11Mappings::getSizeInBytes(format, src.getWidth(), src.getHeight());

        device.getImmediateContext()->UpdateSubresource(mTex, subresourceIdx, nullptr, src.getData(), rowWidth, sliceWidth);

        if (device.hasError())
        {
          String errorDescription = device.getErrorDescription();
          GE_EXCEPT(RenderingAPIException, "D3D11 device cannot map texture\nError Description:" + errorDescription);
        }

        GE_INC_RENDER_STAT_CAT(ResWrite, RenderStatObject_Texture);
      }
      else
      {
        GE_EXCEPT(RenderingAPIException, "Trying to write into a buffer with unsupported usage: " + toString(m_properties.getUsage()));
      }
    }

    void D3D11Texture::create1DTex()
    {
      uint32 width = m_properties.getWidth();
      int usage = m_properties.getUsage();
      uint32 numMips = m_properties.getNumMipmaps();
      PixelFormat format = m_properties.getFormat();
      bool hwGamma = m_properties.isHardwareGammaEnabled();
      PixelFormat closestFormat = D3D11Mappings::getClosestSupportedPF(format, TEX_TYPE_1D, usage);
      uint32 numFaces = m_properties.getNumFaces();

      // We must have those defined here
      GE_ASSERT(width > 0);

      // Determine which D3D11 pixel format we'll use
      HRESULT hr;
      DXGI_FORMAT d3dPF = D3D11Mappings::getPF(closestFormat, hwGamma);

      if (format != closestFormat)
      {
        LOGWRN(StringUtil::format("Provided pixel format is not supported by the driver: {0}. Falling back on: {1}.",
          format, closestFormat));
      }

      mInternalFormat = closestFormat;
      mDXGIColorFormat = d3dPF;
      mDXGIDepthStencilFormat = d3dPF;

      // TODO - Consider making this a parameter eventually
      bool readableDepth = true;

      D3D11_TEXTURE1D_DESC desc;
      desc.Width = static_cast<uint32>(width);
      desc.ArraySize = numFaces == 0 ? 1 : numFaces;
      desc.Format = d3dPF;
      desc.MiscFlags = 0;

      if ((usage & TU_RENDERTARGET) != 0)
      {
        desc.Usage = D3D11_USAGE_DEFAULT;
        desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
        desc.CPUAccessFlags = 0;
        desc.MipLevels = 1;
      }
      else if ((usage & TU_DEPTHSTENCIL) != 0)
      {
        desc.Usage = D3D11_USAGE_DEFAULT;

        if (readableDepth)
          desc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
        else
          desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

        desc.CPUAccessFlags = 0;
        desc.MipLevels = 1;
        desc.Format = D3D11Mappings::getTypelessDepthStencilPF(closestFormat);

        mDXGIColorFormat = D3D11Mappings::getShaderResourceDepthStencilPF(closestFormat);
        mDXGIDepthStencilFormat = d3dPF;
      }
      else
      {
        desc.Usage = D3D11Mappings::getUsage((GPU_BUFFER_USAGE::E)usage);
        desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
        desc.CPUAccessFlags = D3D11Mappings::getAccessFlags((GPU_BUFFER_USAGE::E)usage);

        // Determine total number of mipmaps including main one (d3d11 convention)
        desc.MipLevels = (numMips == TEXTURE_MIPMAP::kUNLIMITED || (1U << numMips) > width) ? 0 : numMips + 1;
      }

      if ((usage & TU_LOADSTORE) != 0)
        desc.BindFlags |= D3D11_BIND_UNORDERED_ACCESS;

      // Create the texture
      D3D11RenderAPI* rs = static_cast<D3D11RenderAPI*>(RenderAPI::instancePtr());
      D3D11Device& device = rs->getPrimaryDevice();
      hr = device.getD3D11Device()->CreateTexture1D(&desc, nullptr, &m1DTex);

      // Check result and except if failed
      if (FAILED(hr) || device.hasError())
      {
        String errorDescription = device.getErrorDescription();
        GE_EXCEPT(RenderingAPIException, "Error creating texture\nError Description:" + errorDescription);
      }

      hr = m1DTex->QueryInterface(__uuidof(ID3D11Resource), (void **)&mTex);

      if (FAILED(hr) || device.hasError())
      {
        String errorDescription = device.getErrorDescription();
        GE_EXCEPT(RenderingAPIException, "Can't get base texture\nError Description:" + errorDescription);
      }

      m1DTex->GetDesc(&desc);

      if (numMips != (desc.MipLevels - 1))
      {
        GE_EXCEPT(RenderingAPIException, "Driver returned different number of mip maps than requested. " \
          "Requested: " + toString(numMips) + ". Got: " + toString(desc.MipLevels - 1) + ".");
      }

      mDXGIFormat = desc.Format;

      // Create texture view
      if ((usage & TU_DEPTHSTENCIL) == 0 || readableDepth)
      {
        TEXTURE_VIEW_DESC viewDesc;
        viewDesc.mostDetailMip = 0;
        viewDesc.numMips = desc.MipLevels;
        viewDesc.firstArraySlice = 0;
        viewDesc.numArraySlices = desc.ArraySize;
        viewDesc.usage = GPU_VIEW_USAGE::kDEFAULT;

        mShaderResourceView = ge_shared_ptr<D3D11TextureView>(new (ge_alloc<D3D11TextureView>()) D3D11TextureView(this, viewDesc));
      }
    }

    void D3D11Texture::create2DTex()
    {
      uint32 width = m_properties.getWidth();
      uint32 height = m_properties.getHeight();
      int usage = m_properties.getUsage();
      uint32 numMips = m_properties.getNumMipmaps();
      PixelFormat format = m_properties.getFormat();
      bool hwGamma = m_properties.isHardwareGammaEnabled();
      uint32 sampleCount = m_properties.getNumSamples();
      TextureType texType = m_properties.getTextureType();
      PixelFormat closestFormat = D3D11Mappings::getClosestSupportedPF(format, texType, usage);
      uint32 numFaces = m_properties.getNumFaces();

      // TODO - Consider making this a parameter eventually
      bool readableDepth = true;

      // We must have those defined here
      GE_ASSERT(width > 0 || height > 0);

      // Determine which D3D11 pixel format we'll use
      HRESULT hr;
      DXGI_FORMAT d3dPF = D3D11Mappings::getPF(closestFormat, hwGamma);

      if (format != closestFormat)
      {
        LOGWRN(StringUtil::format("Provided pixel format is not supported by the driver: {0}. Falling back on: {1}.",
          format, closestFormat));
      }

      mInternalFormat = closestFormat;
      mDXGIColorFormat = d3dPF;
      mDXGIDepthStencilFormat = d3dPF;

      D3D11_TEXTURE2D_DESC desc;
      desc.Width = static_cast<uint32>(width);
      desc.Height = static_cast<uint32>(height);
      desc.ArraySize = numFaces == 0 ? 1 : numFaces;;
      desc.Format = d3dPF;
      desc.MiscFlags = 0;

      if ((usage & TU_RENDERTARGET) != 0)
      {
        desc.Usage = D3D11_USAGE_DEFAULT;
        desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE; // TODO - Add flags to allow RT be created without shader resource flags (might be more optimal)
        desc.CPUAccessFlags = 0;

        DXGI_SAMPLE_DESC sampleDesc;
        D3D11RenderAPI* rs = static_cast<D3D11RenderAPI*>(RenderAPI::instancePtr());
        rs->determineMultisampleSettings(sampleCount, d3dPF, &sampleDesc);
        desc.SampleDesc = sampleDesc;
      }
      else if ((usage & TU_DEPTHSTENCIL) != 0)
      {
        desc.Usage = D3D11_USAGE_DEFAULT;
        desc.CPUAccessFlags = 0;
        desc.Format = D3D11Mappings::getTypelessDepthStencilPF(closestFormat);

        if (readableDepth)
          desc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
        else
          desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

        DXGI_SAMPLE_DESC sampleDesc;
        D3D11RenderAPI* rs = static_cast<D3D11RenderAPI*>(RenderAPI::instancePtr());
        rs->determineMultisampleSettings(sampleCount, d3dPF, &sampleDesc);
        desc.SampleDesc = sampleDesc;

        mDXGIColorFormat = D3D11Mappings::getShaderResourceDepthStencilPF(closestFormat);
        mDXGIDepthStencilFormat = d3dPF;
      }
      else
      {
        desc.Usage = D3D11Mappings::getUsage((GPU_BUFFER_USAGE::E)usage);
        desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
        desc.CPUAccessFlags = D3D11Mappings::getAccessFlags((GPU_BUFFER_USAGE::E)usage);

        DXGI_SAMPLE_DESC sampleDesc;
        sampleDesc.Count = 1;
        sampleDesc.Quality = 0;
        desc.SampleDesc = sampleDesc;
      }

      // Determine total number of mipmaps including main one (d3d11 convention)
      desc.MipLevels = (numMips == TEXTURE_MIPMAP::kUNLIMITED || (1U << numMips) > width) ? 0 : numMips + 1;

      if (texType == TEX_TYPE_CUBE_MAP)
        desc.MiscFlags |= D3D11_RESOURCE_MISC_TEXTURECUBE;

      if ((usage & TU_LOADSTORE) != 0)
      {
        if (desc.SampleDesc.Count <= 1)
          desc.BindFlags |= D3D11_BIND_UNORDERED_ACCESS;
        else
        {
          LOGWRN("Unable to create a load-store texture with multiple samples. This is not supported on DirectX 11. "
            "Ignoring load-store usage flag.");
        }
      }

      // Create the texture
      D3D11RenderAPI* rs = static_cast<D3D11RenderAPI*>(RenderAPI::instancePtr());
      D3D11Device& device = rs->getPrimaryDevice();
      hr = device.getD3D11Device()->CreateTexture2D(&desc, nullptr, &m2DTex);

      // Check result and except if failed
      if (FAILED(hr) || device.hasError())
      {
        String errorDescription = device.getErrorDescription();
        GE_EXCEPT(RenderingAPIException, "Error creating texture\nError Description:" + errorDescription);
      }

      hr = m2DTex->QueryInterface(__uuidof(ID3D11Resource), (void **)&mTex);

      if (FAILED(hr) || device.hasError())
      {
        String errorDescription = device.getErrorDescription();
        GE_EXCEPT(RenderingAPIException, "Can't get base texture\nError Description:" + errorDescription);
      }

      m2DTex->GetDesc(&desc);

      if (numMips != (desc.MipLevels - 1))
      {
        GE_EXCEPT(RenderingAPIException, "Driver returned different number of mip maps than requested. " \
          "Requested: " + toString(numMips) + ". Got: " + toString(desc.MipLevels - 1) + ".");
      }

      mDXGIFormat = desc.Format;

      // Create shader texture view
      if ((usage & TU_DEPTHSTENCIL) == 0 || readableDepth)
      {
        TEXTURE_VIEW_DESC viewDesc;
        viewDesc.mostDetailMip = 0;
        viewDesc.numMips = desc.MipLevels;
        viewDesc.firstArraySlice = 0;
        viewDesc.numArraySlices = desc.ArraySize;
        viewDesc.usage = GPU_VIEW_USAGE::kDEFAULT;

        mShaderResourceView = ge_shared_ptr<D3D11TextureView>(new (ge_alloc<D3D11TextureView>()) D3D11TextureView(this, viewDesc));
      }
    }

    void D3D11Texture::create3DTex()
    {
      uint32 width = m_properties.getWidth();
      uint32 height = m_properties.getHeight();
      uint32 depth = m_properties.getDepth();
      int usage = m_properties.getUsage();
      uint32 numMips = m_properties.getNumMipmaps();
      PixelFormat format = m_properties.getFormat();
      bool hwGamma = m_properties.isHardwareGammaEnabled();
      PixelFormat closestFormat = D3D11Mappings::getClosestSupportedPF(format, TEX_TYPE_3D, usage);

      // TODO - Consider making this a parameter eventually
      bool readableDepth = true;

      // We must have those defined here
      GE_ASSERT(width > 0 && height > 0 && depth > 0);

      // Determine which D3D11 pixel format we'll use
      HRESULT hr;
      DXGI_FORMAT d3dPF = D3D11Mappings::getPF(closestFormat, hwGamma);

      if (format != closestFormat)
      {
        LOGWRN(StringUtil::format("Provided pixel format is not supported by the driver: {0}. Falling back on: {1}.",
          format, closestFormat));
      }

      mInternalFormat = closestFormat;
      mDXGIColorFormat = d3dPF;
      mDXGIDepthStencilFormat = d3dPF;

      D3D11_TEXTURE3D_DESC desc;
      desc.Width = static_cast<uint32>(width);
      desc.Height = static_cast<uint32>(height);
      desc.Depth = static_cast<uint32>(depth);
      desc.Format = d3dPF;
      desc.MiscFlags = 0;

      if ((m_properties.getUsage() & TU_RENDERTARGET) != 0)
      {
        desc.Usage = D3D11_USAGE_DEFAULT;
        desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
        desc.CPUAccessFlags = 0;
        desc.MipLevels = 1;
      }
      else if ((m_properties.getUsage() & TU_DEPTHSTENCIL) != 0)
      {
        desc.Usage = D3D11_USAGE_DEFAULT;
        desc.CPUAccessFlags = 0;
        desc.MipLevels = 1;

        if (readableDepth)
          desc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
        else
          desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

        mDXGIColorFormat = D3D11Mappings::getShaderResourceDepthStencilPF(closestFormat);
        mDXGIDepthStencilFormat = d3dPF;
      }
      else
      {
        desc.Usage = D3D11Mappings::getUsage((GPU_BUFFER_USAGE::E)usage);
        desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
        desc.CPUAccessFlags = D3D11Mappings::getAccessFlags((GPU_BUFFER_USAGE::E)usage);

        // Determine total number of mipmaps including main one (d3d11 convention)
        desc.MipLevels = (numMips == TEXTURE_MIPMAP::kUNLIMITED || (1U << numMips)
        > std::max(std::max(width, height), depth)) ? 0 : numMips + 1;
      }

      if ((usage & TU_LOADSTORE) != 0)
        desc.BindFlags |= D3D11_BIND_UNORDERED_ACCESS;

      // Create the texture
      D3D11RenderAPI* rs = static_cast<D3D11RenderAPI*>(RenderAPI::instancePtr());
      D3D11Device& device = rs->getPrimaryDevice();
      hr = device.getD3D11Device()->CreateTexture3D(&desc, nullptr, &m3DTex);

      // Check result and except if failed
      if (FAILED(hr) || device.hasError())
      {
        String errorDescription = device.getErrorDescription();
        GE_EXCEPT(RenderingAPIException, "Error creating texture\nError Description:" + errorDescription);
      }

      hr = m3DTex->QueryInterface(__uuidof(ID3D11Resource), (void **)&mTex);

      if (FAILED(hr) || device.hasError())
      {
        String errorDescription = device.getErrorDescription();
        GE_EXCEPT(RenderingAPIException, "Can't get base texture\nError Description:" + errorDescription);
      }

      // Create texture view
      m3DTex->GetDesc(&desc);

      if (m_properties.getNumMipmaps() != (desc.MipLevels - 1))
      {
        GE_EXCEPT(RenderingAPIException, "Driver returned different number of mip maps than requested. " \
          "Requested: " + toString(m_properties.getNumMipmaps()) + ". Got: " + toString(desc.MipLevels - 1) + ".");
      }

      mDXGIFormat = desc.Format;

      if ((usage & TU_DEPTHSTENCIL) == 0 || readableDepth)
      {
        TEXTURE_VIEW_DESC viewDesc;
        viewDesc.mostDetailMip = 0;
        viewDesc.numMips = desc.MipLevels;
        viewDesc.firstArraySlice = 0;
        viewDesc.numArraySlices = 1;
        viewDesc.usage = GPU_VIEW_USAGE::kDEFAULT;

        mShaderResourceView = ge_shared_ptr<D3D11TextureView>(new (ge_alloc<D3D11TextureView>()) D3D11TextureView(this, viewDesc));
      }
    }

    void* D3D11Texture::map(ID3D11Resource* res, D3D11_MAP flags, uint32 mipLevel, uint32 face, uint32& rowPitch, uint32& slicePitch)
    {
      D3D11_MAPPED_SUBRESOURCE pMappedResource;
      pMappedResource.pData = nullptr;

      mipLevel = Math::clamp(mipLevel, (uint32)mipLevel, m_properties.getNumMipmaps());
      face = Math::clamp(face, (uint32)0, m_properties.getNumFaces() - 1);

      if (m_properties.getTextureType() == TEX_TYPE_3D)
        face = 0;

      D3D11RenderAPI* rs = static_cast<D3D11RenderAPI*>(RenderAPI::instancePtr());
      D3D11Device& device = rs->getPrimaryDevice();

      mLockedSubresourceIdx = D3D11CalcSubresource(mipLevel, face, m_properties.getNumMipmaps() + 1);
      device.getImmediateContext()->Map(res, mLockedSubresourceIdx, flags, 0, &pMappedResource);

      if (device.hasError())
      {
        String errorDescription = device.getErrorDescription();
        GE_EXCEPT(RenderingAPIException, "D3D11 device cannot map texture\nError Description:" + errorDescription);
      }

      rowPitch = pMappedResource.RowPitch;
      slicePitch = pMappedResource.DepthPitch;

      return pMappedResource.pData;
    }

    void D3D11Texture::unmap(ID3D11Resource* res)
    {
      D3D11RenderAPI* rs = static_cast<D3D11RenderAPI*>(RenderAPI::instancePtr());
      D3D11Device& device = rs->getPrimaryDevice();
      device.getImmediateContext()->Unmap(res, mLockedSubresourceIdx);

      if (device.hasError())
      {
        String errorDescription = device.getErrorDescription();
        GE_EXCEPT(RenderingAPIException, "D3D11 device unmap resource\nError Description:" + errorDescription);
      }
    }

    void* D3D11Texture::mapstagingbuffer(D3D11_MAP flags, uint32 mipLevel, uint32 face, uint32& rowPitch, uint32& slicePitch)
    {
      // Note: I am creating and destroying a staging resource every time a texture is read. 
      // Consider offering a flag on init that will keep this active all the time (at the cost of double memory).
      // Reading is slow operation anyway so I don't believe doing it as we are now will influence it much.

      if (!mStagingBuffer)
        createStagingBuffer();

      D3D11RenderAPI* rs = static_cast<D3D11RenderAPI*>(RenderAPI::instancePtr());
      D3D11Device& device = rs->getPrimaryDevice();
      device.getImmediateContext()->CopyResource(mStagingBuffer, mTex);

      return map(mStagingBuffer, flags, face, mipLevel, rowPitch, slicePitch);
    }

    void D3D11Texture::unmapstagingbuffer()
    {
      unmap(mStagingBuffer);
      SAFE_RELEASE(mStagingBuffer);
    }

    void* D3D11Texture::mapstaticbuffer(PixelData lock, uint32 mipLevel, uint32 face)
    {
      uint32 sizeOfImage = lock.getConsecutiveSize();
      mLockedSubresourceIdx = D3D11CalcSubresource(mipLevel, face, m_properties.getNumMipmaps() + 1);

      mStaticBuffer = ge_new<PixelData>(lock.getWidth(), lock.getHeight(), lock.getDepth(), lock.getFormat());
      mStaticBuffer->allocateInternalBuffer();

      return mStaticBuffer->getData();
    }

    void D3D11Texture::unmapstaticbuffer()
    {
      uint32 rowWidth = D3D11Mappings::getSizeInBytes(mStaticBuffer->getFormat(), mStaticBuffer->getWidth());
      uint32 sliceWidth = D3D11Mappings::getSizeInBytes(mStaticBuffer->getFormat(), mStaticBuffer->getWidth(), mStaticBuffer->getHeight());

      D3D11RenderAPI* rs = static_cast<D3D11RenderAPI*>(RenderAPI::instancePtr());
      D3D11Device& device = rs->getPrimaryDevice();
      device.getImmediateContext()->UpdateSubresource(mTex, mLockedSubresourceIdx, nullptr, mStaticBuffer->getData(), rowWidth, sliceWidth);

      if (device.hasError())
      {
        String errorDescription = device.getErrorDescription();
        GE_EXCEPT(RenderingAPIException, "D3D11 device cannot map texture\nError Description:" + errorDescription);
      }

      if (mStaticBuffer != nullptr)
        ge_delete(mStaticBuffer);
    }

    ID3D11ShaderResourceView* D3D11Texture::getSRV() const
    {
      return mShaderResourceView->getSRV();
    }

    void D3D11Texture::createStagingBuffer()
    {
      D3D11RenderAPI* rs = static_cast<D3D11RenderAPI*>(RenderAPI::instancePtr());
      D3D11Device& device = rs->getPrimaryDevice();
      switch (m_properties.getTextureType())
      {
      case TEX_TYPE_1D:
      {
        D3D11_TEXTURE1D_DESC desc;
        m1DTex->GetDesc(&desc);

        desc.BindFlags = 0;
        desc.MiscFlags = 0;
        desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE | D3D11_CPU_ACCESS_READ;
        desc.Usage = D3D11_USAGE_STAGING;

        device.getD3D11Device()->CreateTexture1D(&desc, nullptr, (ID3D11Texture1D**)(&mStagingBuffer));
      }
      break;
      case TEX_TYPE_2D:
      case TEX_TYPE_CUBE_MAP:
      {
        D3D11_TEXTURE2D_DESC desc;
        m2DTex->GetDesc(&desc);

        desc.BindFlags = 0;
        desc.MiscFlags = 0;
        desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE | D3D11_CPU_ACCESS_READ;
        desc.Usage = D3D11_USAGE_STAGING;

        device.getD3D11Device()->CreateTexture2D(&desc, nullptr, (ID3D11Texture2D**)(&mStagingBuffer));
      }
      break;
      case TEX_TYPE_3D:
      {
        D3D11_TEXTURE3D_DESC desc;
        m3DTex->GetDesc(&desc);

        desc.BindFlags = 0;
        desc.MiscFlags = 0;
        desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE | D3D11_CPU_ACCESS_READ;
        desc.Usage = D3D11_USAGE_STAGING;

        device.getD3D11Device()->CreateTexture3D(&desc, nullptr, (ID3D11Texture3D**)(&mStagingBuffer));
      }
      break;
      }
    }

    SPtr<TextureView> D3D11Texture::createView(const TEXTURE_VIEW_DESC& desc)
    {
      return ge_shared_ptr<D3D11TextureView>(new (ge_alloc<D3D11TextureView>()) D3D11TextureView(this, desc));
    }
  }
}
