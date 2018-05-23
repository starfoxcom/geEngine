/*****************************************************************************/
/**
 * @file    geRenderTexture.cpp
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2018/05/20
 * @brief   Render target specialization that allows to render on textures.
 *
 * Render target specialization that allows you to render into one or multiple
 * textures. Such textures can then be used in other operations as GPU program
 * input.
 *
 * @bug     No known bugs.
 */
/*****************************************************************************/

/*****************************************************************************/
/**
 * Includes
 */
/*****************************************************************************/
#include "geRenderTexture.h"
#include "geTexture.h"
#include "geTextureManager.h"
#include "geResources.h"
#include "geCoreThread.h"

#include <geNumericLimits.h>
#include <geException.h>

namespace geEngineSDK {
  using std::static_pointer_cast;

  RenderTextureProperties::RenderTextureProperties(const RENDER_TEXTURE_DESC& desc,
                                                   bool requiresFlipping) {
    uint32 firstIdx = NumLimit::MAX_UINT32;
    bool requiresHwGamma = false;
    for (uint32 i = 0; i < GE_MAX_MULTIPLE_RENDER_TARGETS; ++i) {
      HTexture texture = desc.colorSurfaces[i].texture;

      if (!texture.isLoaded()) {
        continue;
      }

      if (NumLimit::MAX_UINT32 == firstIdx) {
        firstIdx = i;
      }

      requiresHwGamma |= texture->getProperties().isHardwareGammaEnabled();
    }

    if (NumLimit::MAX_UINT32 == firstIdx) {
      HTexture texture = desc.depthStencilSurface.texture;
      if (texture.isLoaded()) {
        const TextureProperties& texProps = texture->getProperties();
        construct(&texProps,
                  desc.depthStencilSurface.numFaces,
                  desc.depthStencilSurface.mipLevel,
                  requiresFlipping,
                  false);
      }
    }
    else {
      HTexture texture = desc.colorSurfaces[firstIdx].texture;
      const TextureProperties& texProps = texture->getProperties();
      construct(&texProps,
                desc.colorSurfaces[firstIdx].numFaces,
                desc.colorSurfaces[firstIdx].mipLevel,
                requiresFlipping,
                requiresHwGamma);
    }
  }

  RenderTextureProperties::RenderTextureProperties(
                                const geCoreThread::RENDER_TEXTURE_DESC& desc,
                                bool requiresFlipping) {
    uint32 firstIdx = NumLimit::MAX_UINT32;
    bool requiresHwGamma = false;
    for (uint32 i = 0; i < GE_MAX_MULTIPLE_RENDER_TARGETS; ++i) {
      SPtr<geCoreThread::Texture> texture = desc.colorSurfaces[i].texture;

      if (nullptr == texture) {
        continue;
      }

      if (NumLimit::MAX_UINT32 == firstIdx) {
        firstIdx = i;
      }

      requiresHwGamma |= texture->getProperties().isHardwareGammaEnabled();
    }

    if (NumLimit::MAX_UINT32 == firstIdx) {
      SPtr<geCoreThread::Texture> texture = desc.depthStencilSurface.texture;
      if (nullptr != texture) {
        const TextureProperties& texProps = texture->getProperties();
        construct(&texProps,
                  desc.depthStencilSurface.numFaces,
                  desc.depthStencilSurface.mipLevel,
                  requiresFlipping,
                  false);
      }
    }
    else {
      SPtr<geCoreThread::Texture> texture = desc.colorSurfaces[firstIdx].texture;
      const TextureProperties& texProps = texture->getProperties();
      construct(&texProps,
                desc.colorSurfaces[firstIdx].numFaces,
                desc.colorSurfaces[firstIdx].mipLevel,
                requiresFlipping,
                requiresHwGamma);
    }
  }

  void
  RenderTextureProperties::construct(const TextureProperties* textureProps,
                                     uint32 numSlices,
                                     uint32 mipLevel,
                                     bool requiresFlipping,
                                     bool hwGamma) {
    if (nullptr != textureProps) {
      PixelUtil::getSizeForMipLevel(textureProps->getWidth(),
                                    textureProps->getHeight(),
                                    textureProps->getDepth(),
                                    mipLevel,
                                    m_width,
                                    m_height,
                                    numSlices);

      m_numSlices = numSlices; //TODO: This is nonsence!!!
      m_multisampleCount = textureProps->getNumSamples();
    }

    m_isWindow = false;
    m_requiresTextureFlipping = requiresFlipping;
    this->m_hwGamma = hwGamma;
  }

  SPtr<RenderTexture>
  RenderTexture::create(const TEXTURE_DESC& desc,
                        bool createDepth,
                        PixelFormat depthStencilFormat) {
    return TextureManager::instance().createRenderTexture(desc,
                                                          createDepth,
                                                          depthStencilFormat);
  }

  SPtr<RenderTexture>
  RenderTexture::create(const RENDER_TEXTURE_DESC& desc) {
    return TextureManager::instance().createRenderTexture(desc);
  }

  SPtr<geCoreThread::RenderTexture>
  RenderTexture::getCore() const {
    return static_pointer_cast<geCoreThread::RenderTexture>(m_coreSpecific);
  }

  RenderTexture::RenderTexture(const RENDER_TEXTURE_DESC& desc)
    : m_desc(desc) {
    for (uint32 i = 0; i < GE_MAX_MULTIPLE_RENDER_TARGETS; ++i) {
      if (nullptr != desc.colorSurfaces[i].texture) {
        m_bindableColorTex[i] = desc.colorSurfaces[i].texture;
      }
    }

    if (nullptr != desc.depthStencilSurface.texture) {
      m_bindableDepthStencilTex = desc.depthStencilSurface.texture;
    }
  }

  SPtr<geCoreThread::CoreObject>
  RenderTexture::createCore() const {
    geCoreThread::RENDER_TEXTURE_DESC coreDesc;

    for (uint32 i = 0; i < GE_MAX_MULTIPLE_RENDER_TARGETS; ++i) {
      geCoreThread::RENDER_SURFACE_DESC surfaceDesc;
      if (m_desc.colorSurfaces[i].texture.isLoaded()) {
        surfaceDesc.texture = m_desc.colorSurfaces[i].texture->getCore();
      }

      surfaceDesc.face = m_desc.colorSurfaces[i].face;
      surfaceDesc.numFaces = m_desc.colorSurfaces[i].numFaces;
      surfaceDesc.mipLevel = m_desc.colorSurfaces[i].mipLevel;

      coreDesc.colorSurfaces[i] = surfaceDesc;
    }

    if (m_desc.depthStencilSurface.texture.isLoaded()) {
      coreDesc.depthStencilSurface.texture = m_desc.depthStencilSurface.texture->getCore();
    }

    coreDesc.depthStencilSurface.face = m_desc.depthStencilSurface.face;
    coreDesc.depthStencilSurface.numFaces = m_desc.depthStencilSurface.numFaces;
    coreDesc.depthStencilSurface.mipLevel = m_desc.depthStencilSurface.mipLevel;

    return geCoreThread::TextureManager::instance().createRenderTextureInternal(coreDesc);
  }

  CoreSyncData
  RenderTexture::syncToCore(FrameAlloc* allocator) {
    SIZE_T size = sizeof(RenderTextureProperties);
    uint8* buffer = allocator->alloc(size);

    auto& props = const_cast<RenderTextureProperties&>(getProperties());

    memcpy(buffer, (void*)&props, size);
    return CoreSyncData(buffer, static_cast<uint32>(size));
  }

  const RenderTextureProperties&
  RenderTexture::getProperties() const {
    return static_cast<const RenderTextureProperties&>(getPropertiesInternal());
  }

  namespace geCoreThread {
    RenderTexture::RenderTexture(const RENDER_TEXTURE_DESC& desc, uint32 /*deviceIdx*/)
      : m_desc(desc)
    {}

    void
    RenderTexture::initialize() {
      RenderTarget::initialize();

      for (uint32 i = 0; i < GE_MAX_MULTIPLE_RENDER_TARGETS; ++i) {
        if (nullptr != m_desc.colorSurfaces[i].texture) {
          SPtr<Texture> texture = m_desc.colorSurfaces[i].texture;

          if ((texture->getProperties().getUsage() & TU_RENDERTARGET) == 0) {
            GE_EXCEPT(InvalidParametersException,
                      "Texture is not created with render target usage.");
          }

          m_colorSurfaces[i] = texture->requestView(m_desc.colorSurfaces[i].mipLevel,
                                                    1,
                                                    m_desc.colorSurfaces[i].face,
                                                    m_desc.colorSurfaces[i].numFaces,
                                                    GPU_VIEW_USAGE::kRENDERTARGET);
        }
      }

      if (nullptr != m_desc.depthStencilSurface.texture) {
        SPtr<Texture> texture = m_desc.depthStencilSurface.texture;

        if ((texture->getProperties().getUsage() & TU_DEPTHSTENCIL) == 0) {
          GE_EXCEPT(InvalidParametersException,
                    "Texture is not created with depth stencil usage.");
        }

        m_depthStencilSurface = texture->requestView(m_desc.depthStencilSurface.mipLevel,
                                                     1,
                                                     m_desc.depthStencilSurface.face,
                                                     m_desc.depthStencilSurface.numFaces,
                                                     GPU_VIEW_USAGE::kDEPTHSTENCIL);
      }

      throwIfBuffersDontMatch();
    }

    SPtr<RenderTexture>
    RenderTexture::create(const RENDER_TEXTURE_DESC& desc, uint32 deviceIdx) {
      return TextureManager::instance().createRenderTexture(desc, deviceIdx);
    }

    void
    RenderTexture::syncToCore(const CoreSyncData& data) {
      auto& props = const_cast<RenderTextureProperties&>(getProperties());
      props = data.getData<RenderTextureProperties>();
    }

    const RenderTextureProperties&
    RenderTexture::getProperties() const {
      return static_cast<const RenderTextureProperties&>(getPropertiesInternal());
    }

    void
    RenderTexture::throwIfBuffersDontMatch() const {
      uint32 firstSurfaceIdx = NumLimit::MAX_UINT32;
      for (uint32 i = 0; i <GE_MAX_MULTIPLE_RENDER_TARGETS; ++i) {
        if (nullptr == m_colorSurfaces[i]) {
          continue;
        }

        if (NumLimit::MAX_UINT32 == firstSurfaceIdx) {
          firstSurfaceIdx = i;
          continue;
        }

        const TextureProperties&
          curTexProps = m_desc.colorSurfaces[i].texture->getProperties();
        const TextureProperties&
          firstTexProps = m_desc.colorSurfaces[firstSurfaceIdx].texture->getProperties();

        uint32 curMsCount = curTexProps.getNumSamples();
        uint32 firstMsCount = firstTexProps.getNumSamples();

        uint32 curNumSlices = m_colorSurfaces[i]->getNumArraySlices();
        uint32 firstNumSlices = m_colorSurfaces[firstSurfaceIdx]->getNumArraySlices();

        if (0 == curMsCount) {
          curMsCount = 1;
        }

        if (0 == firstMsCount) {
          firstMsCount = 1;
        }

        if (curTexProps.getWidth() != firstTexProps.getWidth() ||
            curTexProps.getHeight() != firstTexProps.getHeight() ||
            curTexProps.getDepth() != firstTexProps.getDepth() ||
            curMsCount != firstMsCount ||
            curNumSlices != firstNumSlices) {
          String errorInfo = "\nWidth: " + toString(curTexProps.getWidth()) +
                             "/" + toString(firstTexProps.getWidth());
          errorInfo += "\nHeight: " + toString(curTexProps.getHeight()) +
                       "/" + toString(firstTexProps.getHeight());
          errorInfo += "\nDepth: " + toString(curTexProps.getDepth()) +
                       "/" + toString(firstTexProps.getDepth());
          errorInfo += "\nNum. slices: " + toString(curNumSlices) +
                       "/" + toString(firstNumSlices);
          errorInfo += "\nMultisample Count: " + toString(curMsCount) +
                       "/" + toString(firstMsCount);

          GE_EXCEPT(InvalidParametersException,
                    "Provided color textures don't match!" + errorInfo);
        }
      }

      if (NumLimit::MAX_UINT32 != firstSurfaceIdx) {
        const TextureProperties&
          firstTexProps = m_desc.colorSurfaces[firstSurfaceIdx].texture->getProperties();
        SPtr<TextureView> firstSurfaceView = m_colorSurfaces[firstSurfaceIdx];

        uint32 numSlices;
        if (firstTexProps.getTextureType() == TEX_TYPE_3D) {
          numSlices = firstTexProps.getDepth();
        }
        else {
          numSlices = firstTexProps.getNumFaces();
        }

        if ((firstSurfaceView->getFirstArraySlice() +
             firstSurfaceView->getNumArraySlices()) > numSlices) {
          GE_EXCEPT(InvalidParametersException,
                    "Number of faces is out of range. Face: " +
                      toString(firstSurfaceView->getFirstArraySlice() +
                      firstSurfaceView->getNumArraySlices()) +
                      ". Max num faces: " + toString(numSlices));
        }

        if (firstSurfaceView->getMostDetailedMip() > firstTexProps.getNumMipmaps()) {
          GE_EXCEPT(InvalidParametersException,
                    "Number of mip maps is out of range. Mip level: " +
                    toString(firstSurfaceView->getMostDetailedMip()) +
                    ". Max num mipmaps: " +
                    toString(firstTexProps.getNumMipmaps()));
        }

        if (nullptr == m_depthStencilSurface) {
          return;
        }

        auto& depthTexProps = m_desc.depthStencilSurface.texture->getProperties();
        uint32 depthMsCount = depthTexProps.getNumSamples();
        uint32 colorMsCount = firstTexProps.getNumSamples();

        if (0 == depthMsCount) {
          depthMsCount = 1;
        }

        if (0 == colorMsCount) {
          colorMsCount = 1;
        }

        if (depthTexProps.getWidth() != firstTexProps.getWidth() ||
            depthTexProps.getHeight() != firstTexProps.getHeight() ||
            depthMsCount != colorMsCount) {
          String errorInfo = "\nWidth: " + toString(depthTexProps.getWidth()) +
                             "/" + toString(firstTexProps.getWidth());
          errorInfo += "\nHeight: " + toString(depthTexProps.getHeight()) +
                       "/" + toString(firstTexProps.getHeight());
          errorInfo += "\nMultisample Count: " + toString(depthMsCount) +
                       "/" + toString(colorMsCount);

          GE_EXCEPT(InvalidParametersException,
                    "Texture and depth stencil buffer don't match!" + errorInfo);
        }
      }
    }
  }
}
