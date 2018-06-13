/*****************************************************************************/
/**
 * @file    geD3D11TextureView.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2018/06/12
 * @brief   DirectX implementation of a texture resource view.
 *
 * DirectX implementation of a texture resource view.
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

#include <geTextureView.h>

namespace geEngineSDK {
  namespace geCoreThread {
    class D3D11TextureView : public TextureView
    {
     public:
      ~D3D11TextureView();

      /**
       * @brief Returns a shader resource view.
       *        Caller must take care this texture view actually contains a
       *        shader resource view, otherwise it returns null.
       */
      ID3D11ShaderResourceView*
      getSRV() const {
        return m_srv;
      }

      /**
       * @brief Returns a render target view.
       *        Caller must take care this texture view actually contains a
       *        render target view, otherwise it returns null.
       */
      ID3D11RenderTargetView*
      getRTV() const {
        return m_rtv;
      }

      /**
       * @brief Returns a unordered access view.
       *        Caller must take care this texture view actually contains a
       *        unordered access view, otherwise it returns null.
       */
      ID3D11UnorderedAccessView*
      getUAV() const {
        return m_uav;
      }

      /**
       * @brief Returns a depth stencil view.
       *        Caller must take care this texture view actually contains a
       *        depth stencil view, otherwise it returns null.
       * @param[in] readOnlyDepth   Should the view only support read
       *            operations for the depth portion of the depth/stencil
       *            buffer(allows the bound texture to be also used as a shader
       *            resource view while bound as a depth stencil target).
       * @param[in] readOnlyStencil Should the view only support read
       *            operations for the stencil portion of the depth/stencil
       *            buffer(allows the bound texture to be also used as a shader
       *            resource view while bound as a depth stencil target).
       */
      ID3D11DepthStencilView*
      getDSV(bool readOnlyDepth, bool readOnlyStencil) const;

     protected:
      friend class D3D11Texture;

      D3D11TextureView(const D3D11Texture* texture,
                       const TEXTURE_VIEW_DESC& desc);

     private:

      /**
       * @brief Creates a shader resource view that allows the provided
       *        surfaces to be bound as normal shader resources.
       * @param[in] texture         Texture to create the resource view for.
       * @param[in] mostDetailMip   First mip level to create the resource view
       *                            for (0 - base level).
       * @param[in] numMips         Number of mip levels to create the view
       *                            for.
       * @param[in] firstArraySlice First array slice to create the view for.
       *                            This will be array index for 1D and 2D
       *                            array textures, texture slice index for 3D
       *                            textures, and face index for cube textures
       *                            (cube index * 6).
       * @param[in]	numArraySlices	Number of array slices to create the view
       *                            for. This will be number of array elements
       *                            for 1D and 2D array textures, number of
       *                            slices for 3D textures, and number of cubes
       *                            for cube textures.
       */
      ID3D11ShaderResourceView*
      createSRV(const D3D11Texture* texture,
                uint32 mostDetailMip,
                uint32 numMips,
                uint32 firstArraySlice,
                uint32 numArraySlices);

      /**
       * @brief Creates a shader resource view that allows the provided
       *        surfaces to be bound as render targets.
       * @param[in] texture         Texture to create the resource view for.
       * @param[in] mipSlice        Mip level to create the resource view for
       *                            (0 - base level).
       * @param[in] firstArraySlice First array slice to create the view for.
       *                            This will be array index for 1D and 2D
       *                            array textures, texture slice index for 3D
       *                            textures, and face index for cube textures
       *                            (cube index * 6).
       * @param[in] numArraySlices  Number of array slices to create the view
       *                            for. This will be number of array elements
       *                            for 1D and 2D array textures, number of
       *                            slices for 3D textures, and number of cubes
       *                            for cube textures.
       */
      ID3D11RenderTargetView*
      createRTV(const D3D11Texture* texture,
                uint32 mipSlice,
                uint32 firstArraySlice,
                uint32 numArraySlices);

      /**
       * @brief Creates a shader resource view that allows the provided
       *        surfaces to be bound as unordered access buffers.
       * @param[in] texture         Texture to create the resource view for.
       * @param[in] mipSlice        Mip level to create the resource view for
       *                            (0 - base level).
       * @param[in] firstArraySlice First array slice to create the view for.
       *                            This will be array index for 1D and 2D
       *                            array textures, texture slice index for 3D
       *                            textures, and face index for cube textures
       *                            (cube index * 6).
       * @param[in] numArraySlices  Number of array slices to create the view
       *                            for. This will be number of array elements
       *                            for 1D and 2D array textures, number of
       *                            slices for 3D textures, and number of cubes
       *                            for cube textures.
       */
      ID3D11UnorderedAccessView*
      createUAV(const D3D11Texture* texture,
                uint32 mipSlice,
                uint32 firstArraySlice,
                uint32 numArraySlices);

      /**
       * @brief Creates a shader resource view that allows the provided
       *        surfaces to be bound as depth stencil buffers.
       * @param[in] texture         Texture to create the resource view for.
       * @param[in] mipSlice        Mip level to create the resource view for
       *                            (0 - base level).
       * @param[in] firstArraySlice First array slice to create the view for.
       *                            This will be array index for 1D and 2D
       *                            array textures, texture slice index for 3D
       *                            textures, and face index for cube textures
       *                            (cube index * 6).
       * @param[in] numArraySlices  Number of array slices to create the view
       *                            for. This will be number of array elements
       *                            for 1D and 2D array textures, number of
       *                            slices for 3D textures, and number of cubes
       *                            for cube textures.
       * @param[in] readOnlyDepth   Should the view only support read
       *                            operations for the depth portion of the
       *                            depth/stencil buffer(allows the bound
       *                            texture to be also used as a shader
       *                            resource view while bound as a depth
       *                            stencil target).
       * @param[in] readOnlyStencil Should the view only support read
       *                            operations for the stencil portion of the
       *                            depth/stencil buffer(allows the bound
       *                            texture to be also used as a shader
       *                            resource view while bound as a depth
       *                            stencil target).
       */
      ID3D11DepthStencilView*
      createDSV(const D3D11Texture* texture,
                uint32 mipSlice,
                uint32 firstArraySlice,
                uint32 numArraySlices,
                bool readOnlyDepth,
                bool readOnlyStencil);

      ID3D11ShaderResourceView* m_srv;
      ID3D11RenderTargetView* m_rtv;
      ID3D11UnorderedAccessView* m_uav;
      ID3D11DepthStencilView*	m_wDepthWStencilView;
      ID3D11DepthStencilView*	m_roDepthWStencilView;
      ID3D11DepthStencilView*	m_roDepthROStencilView;
      ID3D11DepthStencilView*	m_wDepthROStencilView;
    };
  }
}
