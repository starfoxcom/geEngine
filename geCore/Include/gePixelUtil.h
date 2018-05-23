/*****************************************************************************/
/**
 * @file    gePixelUtil.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2018/05/12
 * @brief   Utility methods for converting and managing pixel data and formats.
 *
 * Utility methods for converting and managing pixel data and formats.
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
#include "gePixelData.h"

namespace geEngineSDK {
  /**
   * @brief Types of texture compression quality.
   */
  namespace COMPRESSION_QUALITY {
    enum E {
      kFastest,
      kNormal,
      kProduction,
      kHighest
    };
  }

  /**
   * @brief Mode of the alpha channel in a texture.
   */
  namespace ALPHA_MODE {
    enum E {
      /**
       * Texture has no alpha values.
       */
      kNone,
      
      /**
       * Alpha is in the separate transparency channel.
       */
      kTransparency,

      /**
       * Alpha values have been premultiplied with the color values.
       */
      kPremultiplied
    };
  }

  /**
   * @brief Wrap mode to use when generating mip maps.
   */
  namespace MIPMAP_WRAP_MODE {
    enum E {
      kMirror,
      kRepeat,
      kClamp
    };
  }

  /**
   * @brief Filter to use when generating mip maps.
   */
  namespace MIPMAP_FILTER {
    enum E {
      kBox,
      kTriangle,
      kKaiser
    };
  }

  /**
   * @brief Determines on which axes to mirror an image.
   */
  namespace MIRROR_MODE_BITS {
    enum E {
      kX = 1 << 0,
      kY = 1 << 1,
      kZ = 1 << 2
    };
  }

  using MirrorMode = Flags<MIRROR_MODE_BITS::E>;
  GE_FLAGS_OPERATORS(MIRROR_MODE_BITS::E);

  /**
   * @brief Options used to control texture compression.
   */
  struct CompressionOptions
  {
    /**
     * Format to compress to. Must be a format containing compressed data.
     */
    PixelFormat format = PixelFormat::kBC1;

    /**
     * Controls how to (and if) to compress the alpha channel.
     */
    ALPHA_MODE::E alphaMode = ALPHA_MODE::kNone;

    /**
     * Determines does the input data represent a normal map.
     */
    bool isNormalMap = false;

    /**
     * Determines has the input data been gamma corrected.
     */
    bool isSRGB = false;

    /**
     * Compressed image quality. Better compression might take longer to
     * execute but will generate better results.
     */
    COMPRESSION_QUALITY::E quality = COMPRESSION_QUALITY::kNormal;
  };

  /**
   * @brief Options used to control texture mip map generation.
   */
  struct MipMapGenOptions
  {
    /**
     * Filter to use when downsampling input data.
     */
    MIPMAP_FILTER::E filter = MIPMAP_FILTER::kBox;

    /**
     * Determines how to downsample pixels on borders.
     */
    MIPMAP_WRAP_MODE::E wrapMode = MIPMAP_WRAP_MODE::kMirror;

    /**
     * Determines does the input data represent a normal map.
     */
    bool isNormalMap = false;

    /**
     * Should the downsampled values be re-normalized. Only relevant for mip-maps representing normal maps.
     */
    bool normalizeMipmaps = false;

    /**
     * Determines has the input data been gamma corrected.
     */
    bool isSRGB = false;
  };

  /**
   * @brief Filtering types to use when scaling images.
   */
  namespace FILTER {
    enum E {
      /**
       * No filtering is performed and nearest existing value is used.
       */
      kNEAREST,

      /**
       * Box filter is applied, averaging nearby pixels.
       */
      kLINEAR
    };
  }

  class GE_CORE_EXPORT PixelUtil
  {
   public:
    /**
     * @brief Returns the size of a single pixel of the provided pixel format,
     *        in bytes.
     */
    static uint32
    getNumElemBytes(PixelFormat format);

    /**
     * @brief Returns the size of a single pixel of the provided pixel format,
     *        in bits.
     */
    static uint32
    getNumElemBits(PixelFormat format);

    /**
     * @brief Returns the size of the memory region required to hold pixels of
     *        the provided size and format.
     */
    static uint32
    getMemorySize(uint32 width,
                  uint32 height,
                  uint32 depth,
                  PixelFormat format);

    /**
     * @brief Calculates the size of a mip level of a texture with the provided
     *        size.
     */
    static void
    getSizeForMipLevel(uint32 width,
                       uint32 height,
                       uint32 depth,
                       uint32 mipLevel,
                       uint32& mipWidth,
                       uint32& mipHeight,
                       uint32& mipDepth);

    /**
     * @brief Calculates row and depth pitch for a texture surface of the
     *        specified size and format. For most this will be equal to their
     *        width & height, respectively. But some texture formats
     *        (especially compressed ones) might require extra padding.
     */
    static void
    getPitch(uint32 width,
             uint32 height,
             uint32 depth,
             PixelFormat format,
             uint32& rowPitch,
             uint32& depthPitch);

    /**
     * @brief Returns property flags for this pixel format.
     * @see   PixelFormatFlags
     */
    static uint32
    getFlags(PixelFormat format);

    /**
     * @brief Checks if the provided pixel format has an alpha channel.
     */
    static bool
    hasAlpha(PixelFormat format);

    /**
     * @brief Checks is the provided pixel format a floating point format.
     */
    static bool
    isFloatingPoint(PixelFormat format);

    /**
     * @brief Checks is the provided pixel format compressed.
     */
    static bool
    isCompressed(PixelFormat format);

    /**
     * @brief Checks is the provided pixel format is a depth / stencil buffer
     *        format.
     */
    static bool
    isDepth(PixelFormat format);

    /**
     * @brief Checks does the provided format store data in normalized range.
     */
    static bool
    isNormalized(PixelFormat format);

    /**
     * @brief Checks is the provided format valid for the texture type and
     *        usage.
     * @param[in, out]  format  Format to check. If format is not valid the
                        method will update this with the closest relevant
                        format.
     * @param[in] texType Type of the texture the format will be used for.
     * @param[in] usage A set of TextureUsage flags that define how will a
     *            texture be used.
     * @return  True if the format is valid, false if not.
     * @note  This method checks only for obvious format mismatches:
     *    - Using depth format for anything but a depth-stencil buffer
     *    - Using anything but a depth format for a depth-stencil-buffer
     *    - Using compressed format for anything but normal textures
     *    - Using compressed format for 1D textures
     *    Caller should still check for platform-specific unsupported formats.
     */
    static bool
    checkFormat(PixelFormat& format, TextureType texType, int32 usage);

    /**
     * @brief Checks are the provided dimensions valid for the specified pixel
     *        format. Some formats (like BC) require width / height to be
     *        multiples of 4 and some formats don't allow depth larger than 1.
     */
    static bool
    isValidExtent(uint32 width,
                  uint32 height,
                  uint32 depth,
                  PixelFormat format);

    /**
     * @brief Returns the number of bits per each element in the provided pixel
     *        format. This will return all zero for compressed and
     *        depth/stencil formats.
     */
    static void
    getBitDepths(PixelFormat format, int32(&rgba)[4]);

    /**
     * @brief Returns bit masks that determine in what bit range is each
     *        channel stored.
     * @note  For example if your color is stored in an uint32 and you want to
     *        extract the red channel you should AND the color uint32 with the
     *        bit-mask for the red channel and then right shift it by the red
     *        channel bit shift amount.
     */
    static void
    getBitMasks(PixelFormat format, uint32(&rgba)[4]);

    /**
     * @brief Returns number of bits you need to shift a pixel element in order
     *        to move it to the start of the data type.
     * @note  For example if your color is stored in an uint32 and you want to
     *        extract the red channel you should AND the color uint32 with the
     *        bit-mask for the red channel and then right shift it by the red
     *        channel bit shift amount.
     */
    static void
    getBitShifts(PixelFormat format, uint8(&rgba)[4]);

    /**
     * @brief Returns the name of the pixel format.
     */
    static String
    getFormatName(PixelFormat srcformat);

    /**
     * @brief Returns true if the pixel data in the format can be directly
     *        accessed and read. This is generally not true for compressed
     *        formats.
     */
    static bool
    isAccessible(PixelFormat srcformat);

    /**
     * @brief Returns the type of an individual pixel element in the provided
     *        format.
     */
    static PIXEL_COMPONENT_TYPE::E
    getElementType(PixelFormat format);

    /**
     * @brief Returns the number of pixel elements in the provided format.
     */
    static uint32
    getNumElements(PixelFormat format);

    /**
     * @brief Returns the maximum number of mip maps that can be generated
     *        until we reach the minimum size possible. This does not count
     *        the base level.
     */
    static uint32
    getMaxMipmaps(uint32 width,
                  uint32 height,
                  uint32 depth,
                  PixelFormat format);

    /**
     * @brief Writes the color to the provided memory location.
     */
    static void
    packColor(const LinearColor& color, PixelFormat format, void* dest);

    /**
     * @brief Writes the color to the provided memory location. If the
     *        destination	format is floating point, the byte values will be
     *        converted into [0.0, 1.0] range.
     */
    static void packColor(uint8 r,
                          uint8 g,
                          uint8 b,
                          uint8 a,
                          PixelFormat format,
                          void* dest);

    /**
     * @brief Writes the color to the provided memory location. If the
     *        destination format in non-floating point, the float values will
     *        be assumed to be in [0.0, 1.0] which	will be converted to
     *        integer range. ([0, 255] in the case of bytes)
     */
    static void
    packColor(float r,
              float g,
              float b,
              float a,
              const PixelFormat format,
              void* dest);

    /**
     * @brief Reads the color from the provided memory location and stores it
     *        into the provided color object.
     */
    static void
    unpackColor(LinearColor* color, PixelFormat format, const void* src);

    /**
     * @brief Reads the color from the provided memory location and stores it
     *        into the provided color elements, as bytes clamped to [0, 255]
     *        range.
     */
    static void
    unpackColor(uint8* r,
                uint8* g,
                uint8* b,
                uint8* a,
                PixelFormat format,
                const void* src);

    /**
     * @brief Reads the color from the provided memory location and stores it
     *        into the provided color elements. If the format is not natively
     *        floating point a conversion is done in such a way that returned
     *        values range [0.0, 1.0].
     */
    static void unpackColor(float* r,
                            float* g,
                            float* b,
                            float* a,
                            PixelFormat format,
                            const void* src);

    /**
     * @brief Writes a depth value to the provided memory location.
     *        Depth should be in range [0, 1].
     */
    static void
    packDepth(float depth, const PixelFormat format, void* dest);

    /**
     * @brief Reads the depth from the provided memory location. Value ranges in [0, 1].
     */
    static float
    unpackDepth(PixelFormat format, void* src);

    /**
     * @brief Converts pixels from one format to another. Provided pixel data
     *        objects must have previously allocated buffers of adequate size
     *        and their sizes must match.
     */
    static void
    bulkPixelConversion(const PixelData& src, PixelData& dst);

    /**
     * @brief Flips the order of components in each individual pixel.
     *        For example RGBA -> ABGR.
     */
    static void
    flipComponentOrder(PixelData& data);

    /**
     * @brief Compresses the provided data using the specified compression
     *        options.
     */
    static void
    compress(const PixelData& src,
             PixelData& dst,
             const CompressionOptions& options);

    /**
     * @brief Generates mip-maps from the provided source data using the
     *        specified compression options. Returned list includes the base
     *        level.
     * @return  A list of calculated mip-map data. First entry is the largest
     *          mip and other follow in order from largest to smallest.
     */
    static Vector<SPtr<PixelData>>
    genMipmaps(const PixelData& src, const MipMapGenOptions& options);

    /**
     * @brief Scales pixel data in the source buffer and stores the scaled data
     *        in the destination buffer. Provided pixel data objects must have
     *        previously allocated buffers of adequate size. You may also
     *        provide a filtering method to use when scaling.
     */
    static void
    scale(const PixelData& src,
          PixelData& dst,
          FILTER::E filter = FILTER::kLINEAR);

    /**
     * @brief Mirrors the contents of the provided object along the X, Y and /
     *        or Z axes.
     */
    static void
    mirror(PixelData& pixelData, MirrorMode mode);

    /**
     * @brief Copies the contents of the @p src buffer into the @p dst buffer.
     *        The size of the copied contents is determined by the size of the
     *        @p dst buffer. First pixel copied from @p src is determined by
     *        offset provided in @p offsetX, @p offsetY and @p offsetZ
     *        parameters.
     */
    static void
    copy(const PixelData& src,
         PixelData& dst,
         uint32 offsetX = 0,
         uint32 offsetY = 0,
         uint32 offsetZ = 0);

    /**
     * @brief Applies gamma correction to the pixels in the provided buffer.
     * @param[in] buffer  Pointer to the buffer containing the pixels.
     * @param[in] gamma   Gamma value to apply.
     * @param[in] size    Size of the buffer in bytes.
     * @param[in] bpp     Number of bits per pixel of the pixels in the buffer.
     */
    static void
    applyGamma(uint8* buffer, float gamma, uint32 size, uint8 bpp);
  };
}
