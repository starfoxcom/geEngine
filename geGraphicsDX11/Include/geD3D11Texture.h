/*****************************************************************************/
/**
 * @file    geD3D11Texture.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2018/06/12
 * @brief   DirectX 11 implementation of a texture.
 *
 * DirectX 11 implementation of a texture.
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

#include <geTexture.h>

namespace geEngineSDK {
  namespace geCoreThread {
    class D3D11Texture : public Texture
    {
     public:
      ~D3D11Texture();

      /**
       * @brief Returns internal DX11 texture resource object.
       */
      ID3D11Resource* getDX11Resource() const { return mTex; }

      /**
       * @brief Returns shader resource view associated with the texture.
       */
      ID3D11ShaderResourceView* getSRV() const;

      /**
       * @brief Returns DXGI pixel format that was used to create the texture.
       */
      DXGI_FORMAT getDXGIFormat() const { return mDXGIFormat; }

      /**
       * @brief Returns DXGI pixel used for reading the texture as a shader resource or writing as a render target.
       */
      DXGI_FORMAT getColorFormat() const { return mDXGIColorFormat; }

      /**
       * @brief Returns DXGI pixel used for writing to a depth stencil texture.
       */
      DXGI_FORMAT getDepthStencilFormat() const { return mDXGIDepthStencilFormat; }

     protected:
      friend class D3D11TextureManager;

      D3D11Texture(const TEXTURE_DESC& desc, const SPtr<PixelData>& initialData, GPU_DEVICE_FLAGS::E deviceMask);

      /**
       * @copydoc CoreObject::initialize()
       */
      void initialize() override;

      /**
       * @copydoc Texture::lockImpl
       */
      PixelData lockImpl(GPU_LOCK_OPTIONS::E options, uint32 mipLevel = 0, uint32 face = 0, uint32 deviceIdx = 0,
        uint32 queueIdx = 0) override;

      /**
       * @copydoc Texture::unlockImpl
       */
      void unlockImpl() override;

      /**
       * @copydoc Texture::copyImpl
       */
      void copyImpl(const SPtr<Texture>& target, const TEXTURE_COPY_DESC& desc,
        const SPtr<CommandBuffer>& commandBuffer) override;

      /**
       * @copydoc Texture::readData
       */
      void readDataImpl(PixelData& dest, uint32 mipLevel = 0, uint32 face = 0, uint32 deviceIdx = 0,
        uint32 queueIdx = 0) override;

      /**
       * @copydoc Texture::writeData
       */
      void writeDataImpl(const PixelData& src, uint32 mipLevel = 0, uint32 face = 0, bool discardWholeBuffer = false,
        uint32 queueIdx = 0) override;

      /**
       * @brief Creates a blank DX11 1D texture object.
       */
      void create1DTex();

      /**
       * @brief Creates a blank DX11 2D texture object.
       */
      void create2DTex();

      /**
       * @brief Creates a blank DX11 3D texture object.
       */
      void create3DTex();

      /**
       * @brief Creates a staging buffer that is used as a temporary buffer for read operations on textures that do not support direct reading.
       */
      void createStagingBuffer();

      /**
       * @brief Maps the specified texture surface for reading/writing.
       * @param[in] res       Texture resource to map.
       * @param[in] flags     Mapping flags that let the API know what are we planning to do with mapped memory.
       * @param[in] mipLevel  Mip level to map (0 being the base level).
       * @param[in] face      Texture face to map, in case texture has more than one.
       * @param[out] rowPitch Output size of a single row in bytes.
       * @param[out] slicePitch Output size of a single slice in bytes (relevant only for 3D textures).
       * @return  Pointer to the mapped area of memory.
       * @note Non-staging textures must be dynamic in order to be mapped directly and only for writing. No restrictions are made on staging textures.
       */
      void* map(ID3D11Resource* res, D3D11_MAP flags, uint32 mipLevel, uint32 face, uint32& rowPitch, uint32& slicePitch);

      /**
       * @brief Unmaps a previously mapped texture.
       */
      void unmap(ID3D11Resource* res);

      /**
       * @brief Copies texture data into a staging buffer and maps the staging buffer. Will create a staging buffer if one doesn't already exist (potentially wasting a lot of memory).
       * @param[in] flags     Mapping flags that let the API know what are we planning to do with mapped memory.
       * @param[in] mipLevel  Mip level to map (0 being the base level).
       * @param[in] face      Texture face to map, in case texture has more than one.
       * @param[out] rowPitch Output size of a single row in bytes.
       * @param[out] slicePitch	Output size of a single slice in bytes (relevant only for 3D textures).
       * @return  Pointer to the mapped area of memory.
       */
      void* mapstagingbuffer(D3D11_MAP flags, uint32 mipLevel, uint32 face, uint32& rowPitch, uint32& slicePitch);

      /**
       * @brief Unmaps a previously mapped staging buffer.
       */
      void unmapstagingbuffer();

      /**
       * @brief Maps a static buffer, for writing only. Returned pointer points to temporary CPU memory that will be copied to the mapped resource on "unmap" call.
       * @param[in] lock      Area of the texture to lock.
       * @param[in] mipLevel  Mip level to map (0 being the base level).
       * @param[in] face      Texture face to map, in case texture has more than one.
       */
      void*
      mapstaticbuffer(PixelData lock, uint32 mipLevel, uint32 face);

      /**
       * @brief Unmaps a previously mapped static buffer and flushes its data to the actual GPU buffer.
       */
      void
      unmapstaticbuffer();

      /**
       * @brief Creates an empty and uninitialized texture view object.
       */
      SPtr<TextureView> createView(const TEXTURE_VIEW_DESC& desc) override;

     protected:
      ID3D11Texture1D * m1DTex;
      ID3D11Texture2D* m2DTex;
      ID3D11Texture3D* m3DTex;
      ID3D11Resource* mTex;

      SPtr<D3D11TextureView> mShaderResourceView;

      PixelFormat mInternalFormat;
      DXGI_FORMAT mDXGIFormat;
      DXGI_FORMAT mDXGIColorFormat;
      DXGI_FORMAT mDXGIDepthStencilFormat;

      ID3D11Resource* mStagingBuffer;
      PixelData* mStaticBuffer;
      uint32 mLockedSubresourceIdx;
      bool mLockedForReading;
    };
  }
}
