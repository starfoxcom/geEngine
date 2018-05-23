/*****************************************************************************/
/**
 * @file    geTextureView.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2018/05/19
 * @brief   Texture views allow you to reference only a part of a texture.
 *
 * Texture views allow you to reference only a part of a texture. They may
 * reference one or multiple mip-levels on one or multiple texture array slices.
 * Selected mip level will apply to all slices.
 * They also allow you to re-purpose a texture (for example make a render target
 * a bindable texture).
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
#include "gePrerequisitesCore.h"

namespace geEngineSDK {
  namespace geCoreThread {
    /**
     * @brief Data describing a texture view.
     */
    struct GE_CORE_EXPORT TEXTURE_VIEW_DESC
    {
      /**
       * @brief First mip level of the parent texture the view binds
       *        (0 - base level). This applied to all array slices specified
       *        below.
       */
      uint32 mostDetailMip;

      /**
       * @brief Number of mip levels to bind to the view. This applied to all
       *        array slices specified below.
       */
      uint32 numMips;

      /**
       * @brief First array slice the view binds to. This will be array index
       *        for 1D and 2D array textures, texture slice index for 3D
       *        textures, and face index for cube textures(cube index * 6).
       */
      uint32 firstArraySlice;

      /**
       * @brief Number of array slices to bind tot he view. This will be number
       *        of array elements for 1D and 2D array textures, number of
       *        slices for 3D textures, and number of cubes for cube textures.
       */
      uint32 numArraySlices;

      /**
       * @brief Type of texture view.
       */
      GPU_VIEW_USAGE::E usage;
    };

    /**
     * @brief Texture views allow you to reference only a party of a texture.
     *        They may reference one or multiple mip-levels on one or multiple
     *        texture array slices. Selected mip level will apply to all
     *        slices.
     *        They also allow you to re-purpose a texture (for example make a
     *        render target a bindable texture).
     * @note  Core thread.
     */
    class GE_CORE_EXPORT TextureView
    {
     public:
      class HashFunction
      {
       public:
        size_t
        operator()(const TEXTURE_VIEW_DESC &key) const;
      };

      class EqualFunction
      {
       public:
        bool
        operator()(const TEXTURE_VIEW_DESC &a,
                   const TEXTURE_VIEW_DESC &b) const;
      };

      virtual ~TextureView() = default;

      /**
       * @brief Returns the most detailed mip level visible by the view.
       */
      uint32
      getMostDetailedMip() const {
        return m_desc.mostDetailMip;
      }

      /**
       * @brief Returns the number of mip levels in a single slice visible by the view.
       */
      uint32
      getNumMips() const {
        return m_desc.numMips;
      }

      /**
       * @brief Returns the first array slice index visible by this view.
       */
      uint32
      getFirstArraySlice() const {
        return m_desc.firstArraySlice;
      }

      /**
       * @brief Returns the number of array slices visible by this view.
       */
      uint32
      getNumArraySlices() const {
        return m_desc.numArraySlices;
      }

      /**
       * @brief Returns texture view usage. This determines where on the
       *        pipeline can be bind the view.
       */
      GPU_VIEW_USAGE::E
      getUsage() const {
        return m_desc.usage;
      }

      /**
       * @brief Returns the descriptor used for initializing the view.
       */
      const TEXTURE_VIEW_DESC&
      getDesc() const {
        return m_desc;
      }

     protected:
      TextureView(const TEXTURE_VIEW_DESC& _desc);

     protected:
      friend class Texture;

      TEXTURE_VIEW_DESC m_desc;
    };
  }
}
