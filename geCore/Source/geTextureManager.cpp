/*****************************************************************************/
/**
 * @file    geTextureManager.cpp
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2018/05/21
 * @brief   Interface for creation of textures.
 *
 * Defines an interface for creation of textures. Render systems provide their
 * own implementations.
 *
 * @bug     No known bugs.
 */
/*****************************************************************************/

/*****************************************************************************/
/**
 * Includes
 */
/*****************************************************************************/
#include "geTextureManager.h"
#include "gePixelUtil.h"
#include "geRenderAPI.h"

#include <geException.h>

namespace geEngineSDK {
  SPtr<Texture>
  TextureManager::createTexture(const TEXTURE_DESC& desc) {
    auto tex = GE_PVT_NEW(Texture, desc);
    SPtr<Texture> ret = ge_core_ptr<Texture>(tex);

    ret->_setThisPtr(ret);
    ret->initialize();

    return ret;
  }

  SPtr<Texture>
  TextureManager::createTexture(const TEXTURE_DESC& desc,
                                const SPtr<PixelData>& pixelData) {
    auto tex = GE_PVT_NEW(Texture, desc, pixelData);
    SPtr<Texture> ret = ge_core_ptr<Texture>(tex);

    ret->_setThisPtr(ret);
    ret->initialize();

    return ret;
  }

  SPtr<Texture>
  TextureManager::_createEmpty() {
    auto tex = GE_PVT_NEW(Texture);
    SPtr<Texture> texture = ge_core_ptr<Texture>(tex);
    texture->_setThisPtr(texture);

    return texture;
  }

  SPtr<RenderTexture>
  TextureManager::createRenderTexture(const TEXTURE_DESC& colorDesc,
                                      bool createDepth,
                                      PixelFormat depthStencilFormat) {
    TEXTURE_DESC textureDesc = colorDesc;
    textureDesc.usage = TU_RENDERTARGET;
    textureDesc.numMips = 0;

    HTexture texture = Texture::create(textureDesc);

    HTexture depthStencil;
    if (createDepth) {
      textureDesc.format = depthStencilFormat;
      textureDesc.hwGamma = false;
      textureDesc.usage = TU_DEPTHSTENCIL;

      depthStencil = Texture::create(textureDesc);
    }

    RENDER_TEXTURE_DESC desc;
    desc.colorSurfaces[0].texture = texture;
    desc.colorSurfaces[0].face = 0;
    desc.colorSurfaces[0].numFaces = 1;
    desc.colorSurfaces[0].mipLevel = 0;

    desc.depthStencilSurface.texture = depthStencil;
    desc.depthStencilSurface.face = 0;
    desc.depthStencilSurface.numFaces = 1;
    desc.depthStencilSurface.mipLevel = 0;

    SPtr<RenderTexture> newRT = createRenderTexture(desc);

    return newRT;
  }

  SPtr<RenderTexture>
  TextureManager::createRenderTexture(const RENDER_TEXTURE_DESC& desc) {
    SPtr<RenderTexture> newRT = createRenderTextureImpl(desc);
    newRT->_setThisPtr(newRT);
    newRT->initialize();

    return newRT;
  }

  namespace geCoreThread {
    void
    TextureManager::onStartUp() {
      TEXTURE_DESC desc;
      desc.type = TEX_TYPE_2D;
      desc.width = 2;
      desc.height = 2;
      desc.format = PixelFormat::kRGBA8;
      desc.usage = TU_STATIC;

      //White built-in texture
      SPtr<Texture> whiteTexture = createTexture(desc);

      SPtr<PixelData> whitePixelData = PixelData::create(2, 2, 1, PixelFormat::kRGBA8);
      whitePixelData->setColorAt(LinearColor::White, 0, 0);
      whitePixelData->setColorAt(LinearColor::White, 0, 1);
      whitePixelData->setColorAt(LinearColor::White, 1, 0);
      whitePixelData->setColorAt(LinearColor::White, 1, 1);

      whiteTexture->writeData(*whitePixelData);
      Texture::WHITE = whiteTexture;

      //Black built-in texture
      SPtr<Texture> blackTexture = createTexture(desc);

      SPtr<PixelData> blackPixelData = PixelData::create(2, 2, 1, PixelFormat::kRGBA8);
      blackPixelData->setColorAt(LinearColor::Transparent, 0, 0);
      blackPixelData->setColorAt(LinearColor::Transparent, 0, 1);
      blackPixelData->setColorAt(LinearColor::Transparent, 1, 0);
      blackPixelData->setColorAt(LinearColor::Transparent, 1, 1);

      blackTexture->writeData(*blackPixelData);
      Texture::BLACK = blackTexture;

      //Normal (Y = Up) built-in texture
      SPtr<Texture> normalTexture = createTexture(desc);
      SPtr<PixelData> normalPixelData = PixelData::create(2, 2, 1, PixelFormat::kRGBA8);

      LinearColor encodedNormal(0.5f, 0.5f, 1.0f);
      normalPixelData->setColorAt(encodedNormal, 0, 0);
      normalPixelData->setColorAt(encodedNormal, 0, 1);
      normalPixelData->setColorAt(encodedNormal, 1, 0);
      normalPixelData->setColorAt(encodedNormal, 1, 1);

      normalTexture->writeData(*normalPixelData);
      Texture::NORMAL = normalTexture;
    }

    void
    TextureManager::onShutDown() {
      //Need to make sure these are freed while still on the core thread
      Texture::WHITE = nullptr;
      Texture::BLACK = nullptr;
      Texture::NORMAL = nullptr;
    }

    SPtr<Texture>
    TextureManager::createTexture(const TEXTURE_DESC& desc,
                                  GPU_DEVICE_FLAGS::E deviceMask) {
      SPtr<Texture> newTex = createTextureInternal(desc, nullptr, deviceMask);
      newTex->initialize();

      return newTex;
    }

    SPtr<RenderTexture>
    TextureManager::createRenderTexture(const RENDER_TEXTURE_DESC& desc,
                                        uint32 deviceIdx) {
      SPtr<RenderTexture> newRT = createRenderTextureInternal(desc, deviceIdx);
      newRT->initialize();

      return newRT;
    }
  }
}
