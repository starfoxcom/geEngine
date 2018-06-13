/*****************************************************************************/
/**
 * @file    geD3D11TextureView.cpp
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2018/06/12
 * @brief   DirectX implementation of a texture resource view.
 *
 * DirectX implementation of a texture resource view.
 *
 * @bug     No known bugs.
 */
/*****************************************************************************/

/*****************************************************************************/
/**
 * Includes
 */
/*****************************************************************************/
#include "geD3D11TextureView.h"
#include "geD3D11RenderAPI.h"
#include "geD3D11Device.h"
#include "geD3D11Texture.h"
#include "geD3D11Mappings.h"

#include <geRenderStats.h>
#include <geException.h>

namespace geEngineSDK {
  namespace geCoreThread {
    D3D11TextureView::D3D11TextureView(const D3D11Texture* texture,
                                       const TEXTURE_VIEW_DESC& desc)
      : TextureView(desc),
        m_srv(nullptr),
        m_uav(nullptr),
        m_rtv(nullptr),
        m_wDepthWStencilView(nullptr),
        m_roDepthWStencilView(nullptr),
        m_roDepthROStencilView(nullptr),
        m_wDepthROStencilView(nullptr) {
      if ((m_desc.usage & GPU_VIEW_USAGE::kRANDOMWRITE) != 0) {
        m_uav = createUAV(texture,
                         m_desc.mostDetailMip,
                         m_desc.firstArraySlice,
                         m_desc.numArraySlices);
      }
      else if ((m_desc.usage & GPU_VIEW_USAGE::kRENDERTARGET) != 0) {
        m_rtv = createRTV(texture,
                         m_desc.mostDetailMip,
                         m_desc.firstArraySlice,
                         m_desc.numArraySlices);
      }
      else if ((m_desc.usage & GPU_VIEW_USAGE::kDEPTHSTENCIL) != 0) {
        m_wDepthWStencilView = createDSV(texture,
                                        m_desc.mostDetailMip,
                                        m_desc.firstArraySlice,
                                        m_desc.numArraySlices,
                                        false,
                                        false);

        m_roDepthWStencilView = createDSV(texture,
                                         m_desc.mostDetailMip,
                                         m_desc.firstArraySlice,
                                         m_desc.numArraySlices,
                                         true,
                                         false);

        m_roDepthROStencilView = createDSV(texture,
                                          m_desc.mostDetailMip,
                                          m_desc.firstArraySlice,
                                          m_desc.numArraySlices,
                                          true,
                                          true);

        m_wDepthROStencilView = createDSV(texture,
                                         m_desc.mostDetailMip,
                                         m_desc.firstArraySlice,
                                         m_desc.numArraySlices,
                                         false,
                                         true);
      }
      else {
        m_srv = createSRV(texture,
                         m_desc.mostDetailMip,
                         m_desc.numMips,
                         m_desc.firstArraySlice,
                         m_desc.numArraySlices);
      }
    }

    D3D11TextureView::~D3D11TextureView() {
      SAFE_RELEASE(m_srv);
      SAFE_RELEASE(m_uav);
      SAFE_RELEASE(m_rtv);
      SAFE_RELEASE(m_wDepthWStencilView);
      SAFE_RELEASE(m_wDepthROStencilView);
      SAFE_RELEASE(m_roDepthWStencilView);
      SAFE_RELEASE(m_roDepthROStencilView);
    }

    ID3D11DepthStencilView*
    D3D11TextureView::getDSV(bool readOnlyDepth, bool readOnlyStencil) const {
      if (readOnlyDepth) {
        if (readOnlyStencil) {
          return m_roDepthROStencilView;
        }
        else {
          return m_roDepthWStencilView;
        }
      }
      else {
        if (readOnlyStencil) {
          return m_wDepthROStencilView;
        }
        else {
          return m_wDepthWStencilView;
        }
      }
    }

    ID3D11ShaderResourceView*
    D3D11TextureView::createSRV(const D3D11Texture* texture,
                                uint32 mostDetailMip,
                                uint32 numMips,
                                uint32 firstArraySlice,
                                uint32 numArraySlices) {
      D3D11_SHADER_RESOURCE_VIEW_DESC desc;
      memset(&desc, 0, sizeof(desc));

      const TextureProperties& texProps = texture->getProperties();
      uint32 numFaces = texProps.getNumFaces();

      switch (texProps.getTextureType())
      {
        case TEX_TYPE_1D:
          if (1 >= numFaces) {
            desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE1D;
            desc.Texture1D.MipLevels = numMips;
            desc.Texture1D.MostDetailedMip = mostDetailMip;
          }
          else {
            desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE1DARRAY;
            desc.Texture1DArray.MipLevels = numMips;
            desc.Texture1DArray.MostDetailedMip = mostDetailMip;
            desc.Texture1DArray.FirstArraySlice = firstArraySlice;
            desc.Texture1DArray.ArraySize = numArraySlices;
          }
          break;
        case TEX_TYPE_2D:
          if (texProps.getNumSamples() > 1) {
            if (numFaces <= 1) {
              desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DMS;
            }
            else {
              desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DMSARRAY;
              desc.Texture2DMSArray.FirstArraySlice = firstArraySlice;
              desc.Texture2DMSArray.ArraySize = numArraySlices;
            }
          }
          else {
            if (1 >= numFaces) {
              desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
              desc.Texture2D.MipLevels = numMips;
              desc.Texture2D.MostDetailedMip = mostDetailMip;
            }
            else {
              desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
              desc.Texture2DArray.MipLevels = numMips;
              desc.Texture2DArray.MostDetailedMip = mostDetailMip;
              desc.Texture2DArray.FirstArraySlice = firstArraySlice;
              desc.Texture2DArray.ArraySize = numArraySlices;
            }
          }
          break;
        case TEX_TYPE_3D:
          desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE3D;
          desc.Texture3D.MipLevels = numMips;
          desc.Texture3D.MostDetailedMip = mostDetailMip;
          break;
        case TEX_TYPE_CUBE_MAP:
          if (numFaces % 6 == 0) {
            if (6 == numFaces) {
              desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
              desc.TextureCube.MipLevels = numMips;
              desc.TextureCube.MostDetailedMip = mostDetailMip;
            }
            else {
              desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBEARRAY;
              desc.TextureCubeArray.MipLevels = numMips;
              desc.TextureCubeArray.MostDetailedMip = mostDetailMip;
              desc.TextureCubeArray.First2DArrayFace = firstArraySlice;
              desc.TextureCubeArray.NumCubes = numArraySlices / 6;
            }
          }
          else {
            desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
            desc.Texture2DArray.MipLevels = numMips;
            desc.Texture2DArray.MostDetailedMip = mostDetailMip;
            desc.Texture2DArray.FirstArraySlice = firstArraySlice;
            desc.Texture2DArray.ArraySize = numArraySlices;
          }
          break;
        default:
          GE_EXCEPT(InvalidParametersException,
                    "Invalid texture type for this view type.");
      }

      desc.Format = texture->getColorFormat();

      ID3D11ShaderResourceView* srv = nullptr;

      auto d3d11rs = static_cast<D3D11RenderAPI*>(D3D11RenderAPI::instancePtr());

      D3D11Device& priDevice = d3d11rs->getPrimaryDevice();
      HRESULT hr = priDevice.getD3D11Device()->
                     CreateShaderResourceView(texture->getDX11Resource(), &desc, &srv);

      if (FAILED(hr) || priDevice.hasError()) {
        String msg = priDevice.getErrorDescription();
        GE_EXCEPT(RenderingAPIException,
                  "Cannot create ShaderResourceView: " + msg);
      }

      return srv;
    }

    ID3D11RenderTargetView*
    D3D11TextureView::createRTV(const D3D11Texture* texture,
                                uint32 mipSlice,
                                uint32 firstArraySlice,
                                uint32 numArraySlices) {
      D3D11_RENDER_TARGET_VIEW_DESC desc;
      memset(&desc, 0, sizeof(desc));

      const TextureProperties& texProps = texture->getProperties();
      uint32 numFaces = texProps.getNumFaces();

      switch (texProps.getTextureType())
      {
        case TEX_TYPE_1D:
          if (1 >= numFaces) {
            desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE1D;
            desc.Texture1D.MipSlice = mipSlice;
          }
          else {
            desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE1DARRAY;
            desc.Texture1DArray.MipSlice = mipSlice;
            desc.Texture1DArray.FirstArraySlice = firstArraySlice;
            desc.Texture1DArray.ArraySize = numArraySlices;
          }
          break;
        case TEX_TYPE_2D:
          if (texProps.getNumSamples() > 1) {
            if (1 >= numFaces) {
              desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DMS;
            }
            else {
              desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DMSARRAY;
              desc.Texture2DMSArray.FirstArraySlice = firstArraySlice;
              desc.Texture2DMSArray.ArraySize = numArraySlices;
            }
          }
          else {
            if (1 >= numFaces) {
              desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
              desc.Texture2D.MipSlice = mipSlice;
            }
            else {
              desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
              desc.Texture2DArray.MipSlice = mipSlice;
              desc.Texture2DArray.FirstArraySlice = firstArraySlice;
              desc.Texture2DArray.ArraySize = numArraySlices;
            }
          }
          break;
        case TEX_TYPE_3D:
          desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE3D;
          desc.Texture3D.MipSlice = mipSlice;
          desc.Texture3D.FirstWSlice = 0;
          desc.Texture3D.WSize = texProps.getDepth();
          break;
        case TEX_TYPE_CUBE_MAP:
          desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
          desc.Texture2DArray.FirstArraySlice = firstArraySlice;
          desc.Texture2DArray.ArraySize = numArraySlices;
          desc.Texture2DArray.MipSlice = mipSlice;
          break;
        default:
          GE_EXCEPT(InvalidParametersException,
                    "Invalid texture type for this view type.");
      }

      desc.Format = texture->getColorFormat();

      ID3D11RenderTargetView* rtv = nullptr;

      auto d3d11rs = static_cast<D3D11RenderAPI*>(D3D11RenderAPI::instancePtr());

      D3D11Device& priDevice = d3d11rs->getPrimaryDevice();
      HRESULT hr = priDevice.getD3D11Device()->
                     CreateRenderTargetView(texture->getDX11Resource(), &desc, &rtv);

      if (FAILED(hr) || priDevice.hasError()) {
        String msg = priDevice.getErrorDescription();
        GE_EXCEPT(RenderingAPIException,
                  "Cannot create RenderTargetView: " + msg);
      }

      return rtv;
    }

    ID3D11UnorderedAccessView*
    D3D11TextureView::createUAV(const D3D11Texture* texture,
                                uint32 mipSlice,
                                uint32 firstArraySlice,
                                uint32 numArraySlices) {
      D3D11_UNORDERED_ACCESS_VIEW_DESC desc;
      memset(&desc, 0, sizeof(desc));

      const TextureProperties& texProps = texture->getProperties();
      uint32 numFaces = texProps.getNumFaces();

      switch (texProps.getTextureType())
      {
        case TEX_TYPE_1D:
          if (1 >= numFaces) {
            desc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE1D;
            desc.Texture1D.MipSlice = mipSlice;
          }
          else {
            desc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE1DARRAY;
            desc.Texture1DArray.MipSlice = mipSlice;
            desc.Texture1DArray.FirstArraySlice = firstArraySlice;
            desc.Texture1DArray.ArraySize = numArraySlices;
          }
          break;
        case TEX_TYPE_2D:
          if (1 >= numFaces) {
            desc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
            desc.Texture2D.MipSlice = mipSlice;
          }
          else {
            desc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2DARRAY;
            desc.Texture2DArray.MipSlice = mipSlice;
            desc.Texture2DArray.FirstArraySlice = firstArraySlice;
            desc.Texture2DArray.ArraySize = numArraySlices;
          }
          break;
        case TEX_TYPE_3D:
          desc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE3D;
          desc.Texture3D.MipSlice = mipSlice;
          desc.Texture3D.FirstWSlice = 0;
          desc.Texture3D.WSize = texProps.getDepth();
          break;
        case TEX_TYPE_CUBE_MAP:
          desc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2DARRAY;
          desc.Texture2DArray.FirstArraySlice = firstArraySlice;
          desc.Texture2DArray.ArraySize = numArraySlices;
          desc.Texture2DArray.MipSlice = mipSlice;
          break;
        default:
          GE_EXCEPT(InvalidParametersException,
                    "Invalid texture type for this view type.");
      }

      desc.Format = texture->getDXGIFormat();

      ID3D11UnorderedAccessView* uav = nullptr;

      auto d3d11rs = static_cast<D3D11RenderAPI*>(D3D11RenderAPI::instancePtr());
      D3D11Device& priDevice = d3d11rs->getPrimaryDevice();
      HRESULT hr = priDevice.getD3D11Device()->
                     CreateUnorderedAccessView(texture->getDX11Resource(), &desc, &uav);

      if (FAILED(hr) || priDevice.hasError())
      {
        String msg = priDevice.getErrorDescription();
        GE_EXCEPT(RenderingAPIException,
                  "Cannot create UnorderedAccessView: " + msg);
      }

      return uav;
    }

    ID3D11DepthStencilView*
    D3D11TextureView::createDSV(const D3D11Texture* texture,
                                uint32 mipSlice,
                                uint32 firstArraySlice,
                                uint32 numArraySlices,
                                bool readOnlyDepth,
                                bool readOnlyStencil) {
      D3D11_DEPTH_STENCIL_VIEW_DESC desc;
      memset(&desc, 0, sizeof(desc));

      const TextureProperties& texProps = texture->getProperties();
      uint32 numFaces = texProps.getNumFaces();

      switch (texProps.getTextureType())
      {
        case TEX_TYPE_1D:
          if (1 >= numFaces) {
            desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE1D;
            desc.Texture1D.MipSlice = mipSlice;
          }
          else {
            desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE1DARRAY;
            desc.Texture1DArray.MipSlice = mipSlice;
            desc.Texture1DArray.FirstArraySlice = firstArraySlice;
            desc.Texture1DArray.ArraySize = numArraySlices;
          }
          break;
        case TEX_TYPE_2D:
          if (texProps.getNumSamples() > 1) {
            if (1 >= numFaces) {
              desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
            }
            else {
              desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMSARRAY;
              desc.Texture2DMSArray.FirstArraySlice = firstArraySlice;
              desc.Texture2DMSArray.ArraySize = numArraySlices;
            }
          }
          else {
            if (1 >= numFaces) {
              desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
              desc.Texture2D.MipSlice = mipSlice;
            }
            else {
              desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
              desc.Texture2DArray.MipSlice = mipSlice;
              desc.Texture2DArray.FirstArraySlice = firstArraySlice;
              desc.Texture2DArray.ArraySize = numArraySlices;
            }
          }
          break;
        case TEX_TYPE_3D:
          desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
          desc.Texture2DArray.FirstArraySlice = 0;
          desc.Texture2DArray.ArraySize = texProps.getDepth();
          desc.Texture2DArray.MipSlice = mipSlice;
          break;
        case TEX_TYPE_CUBE_MAP:
          desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
          desc.Texture2DArray.FirstArraySlice = firstArraySlice;
          desc.Texture2DArray.ArraySize = numArraySlices;
          desc.Texture2DArray.MipSlice = mipSlice;
          break;
        default:
          GE_EXCEPT(InvalidParametersException,
                    "Invalid texture type for this view type.");
      }

      desc.Format = texture->getDepthStencilFormat();

      if (readOnlyDepth) {
        desc.Flags = D3D11_DSV_READ_ONLY_DEPTH;
      }

      bool hasStencil = desc.Format == DXGI_FORMAT_D32_FLOAT_S8X24_UINT ||
                        desc.Format == DXGI_FORMAT_D24_UNORM_S8_UINT;

      if (readOnlyStencil && hasStencil) {
        desc.Flags |= D3D11_DSV_READ_ONLY_STENCIL;
      }

      ID3D11DepthStencilView* dsv = nullptr;

      auto d3d11rs = static_cast<D3D11RenderAPI*>(D3D11RenderAPI::instancePtr());
      D3D11Device& priDevice = d3d11rs->getPrimaryDevice();

      HRESULT hr = priDevice.getD3D11Device()->
                     CreateDepthStencilView(texture->getDX11Resource(), &desc, &dsv);

      if (FAILED(hr) || priDevice.hasError()) {
        String msg = priDevice.getErrorDescription();
        GE_EXCEPT(RenderingAPIException,
                  "Cannot create DepthStencilView: " + msg);
      }

      return dsv;
    }
  }
}
