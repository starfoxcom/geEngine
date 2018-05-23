/*****************************************************************************/
/**
 * @file    gePixelData.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2018/05/12
 * @brief   A buffer describing a volume, image or line of pixels in memory.
 *
 * A buffer describing a volume (3D), image (2D) or line (1D) of pixels in
 * memory. Pixels are stored as a succession of "depth" slices, each containing
 * "height" rows of "width" pixels.
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
#include "gePixelVolume.h"
#include "geGPUResourceData.h"

#include <geIReflectable.h>

namespace geEngineSDK {
  /**
   * Pixel formats usable by images, textures and render surfaces.
   */
  enum GE_SCRIPT_EXPORT(m:Rendering) PixelFormat
  {
    /**
     * Unknown pixel format.
     */
    kUNKNOWN      GE_SCRIPT_EXPORT(ex:true) = 0,

    /**
     * 8-bit 1-channel pixel format, unsigned normalized.
     */
    kR8           GE_SCRIPT_EXPORT(n:R8) = 1,

    /**
     * 8-bit 2-channel pixel format, unsigned normalized.
     */
    kRG8          GE_SCRIPT_EXPORT(n:RG8) = 2,

    /**
     * 8-bit 3-channel pixel format, unsigned normalized.
     */
    kRGB8         GE_SCRIPT_EXPORT(n:RGB8) = 3,

    /**
     * 8-bit 3-channel pixel format, unsigned normalized.
     */
    kBGR8         GE_SCRIPT_EXPORT(n:BGR8) = 4,

    /**
     * 8-bit 4-channel pixel format, unsigned normalized.
     */
    kBGRA8        GE_SCRIPT_EXPORT(n:BGRA8) = 7,

    /**
     * 8-bit 4-channel pixel format, unsigned normalized.
     */
    kRGBA8        GE_SCRIPT_EXPORT(n:RGBA8) = 8,

    /**
     * DXT1/BC1 format containing opaque RGB or 1-bit alpha RGB.
     * 4 bits per pixel.
     */
    kBC1          GE_SCRIPT_EXPORT(n:BC1) = 13,

    /**
     * DXT3/BC2 format containing RGB with premultiplied alpha.
     * 4 bits per pixel.
     */
    kBC1a         GE_SCRIPT_EXPORT(ex:true) = 14,

    /**
     * DXT3/BC2 format containing RGB with explicit alpha. 8 bits per pixel.
     */
    kBC2          GE_SCRIPT_EXPORT(n:BC2) = 15,

    /**
     * DXT5/BC2 format containing RGB with explicit alpha. 8 bits per pixel.
     * Better alpha gradients than BC2.
     */
    kBC3          GE_SCRIPT_EXPORT(n:BC3) = 16,

    /**
     * One channel compressed format. 4 bits per pixel.
     */
    kBC4          GE_SCRIPT_EXPORT(n:BC4) = 17,

    /**
     * Two channel compressed format. 8 bits per pixel.
     */
    kBC5          GE_SCRIPT_EXPORT(n:BC5) = 18,

    /**
     * Format storing RGB in half (16bit) floating point format usable for HDR.
     * 8 bits per pixel.
     */
    kBC6H         GE_SCRIPT_EXPORT(n:BC6H) = 19,

    /**
     * Format storing RGB with optional alpha channel. Similar to BC1/BC2/BC3
     * formats but with higher quality and higher decompress overhead.
     * 8 bits per pixel.
     */
    kBC7          GE_SCRIPT_EXPORT(n:BC7) = 20,

    /**
     * 16-bit 1-channel pixel format, signed float.
     */
    kR16F         GE_SCRIPT_EXPORT(n:R16F) = 21,

    /**
     * 16-bit 2-channel pixel format, signed float.
     */
    kRG16F        GE_SCRIPT_EXPORT(n:RG16F) = 22,

    /**
     * 16-bit 4-channel pixel format, signed float.
     */
    kRGBA16F      GE_SCRIPT_EXPORT(n:RGBA16F) = 24,

    /**
     * 32-bit 1-channel pixel format, signed float.
     */
    kR32F         GE_SCRIPT_EXPORT(n:R32F) = 25,

    /**
     * 32-bit 2-channel pixel format, signed float.
     */
    kRG32F        GE_SCRIPT_EXPORT(n:RG32F) = 26,

    /**
     * 32-bit 3-channel pixel format, signed float.
     */
    kRGB32F       GE_SCRIPT_EXPORT(n:RGB32F) = 27,

    /**
     * 32-bit 4-channel pixel format, signed float.
     */
    kRGBA32F      GE_SCRIPT_EXPORT(n:RGBA32F) = 28,

    /**
     * Depth stencil format, 32bit depth, 8bit stencil + 24 unused.
     * Depth stored as signed float.
     */
    kD32_S8X24    GE_SCRIPT_EXPORT(n:D32_S8X24) = 29,

    /**
     * Depth stencil format, 24bit depth + 8bit stencil.
     * Depth stored as unsigned normalized.
     */
    kD24S8        GE_SCRIPT_EXPORT(n:D24S8) = 30,

    /**
     * Depth format, 32bits. Signed float.
     */
    kD32          GE_SCRIPT_EXPORT(n:D32) = 31,

    /**
     * Depth format, 16bits. Unsigned normalized.
     */
    kD16          GE_SCRIPT_EXPORT(n:D16) = 32,

    /**
     * Packed unsigned float format, 11 bits for red, 11 bits for green,
     * 10 bits for blue.
     */
    kRG11B10F     GE_SCRIPT_EXPORT(ex:true) = 33,

    /**
     * Packed unsigned normalized format, 10 bits for red, 10 bits for green,
     * 10 bits for blue, and two bits for alpha.
     */
    kRGB10A2      GE_SCRIPT_EXPORT(ex:true) = 34,

    /**
     * 8-bit 1-channel pixel format, signed integer.
     */
    kR8I          GE_SCRIPT_EXPORT(n:R8I) = 35,

    /**
     * 8-bit 2-channel pixel format, signed integer.
     */
    kRG8I         GE_SCRIPT_EXPORT(n:RG8I) = 36,

    /**
     * 8-bit 4-channel pixel format, signed integer.
     */
    kRGBA8I       GE_SCRIPT_EXPORT(n:RGBA8I) = 37,

    /**
     * 8-bit 1-channel pixel format, unsigned integer.
     */
    kR8U          GE_SCRIPT_EXPORT(n:R8U) = 38,

    /**
     * 8-bit 2-channel pixel format, unsigned integer.
     */
    kRG8U         GE_SCRIPT_EXPORT(n:RG8U) = 39,

    /**
     * 8-bit 4-channel pixel format, unsigned integer.
     */
    kRGBA8U       GE_SCRIPT_EXPORT(n:RGBA8U) = 40,

    /**
     * 8-bit 1-channel pixel format, signed normalized.
     */
    kR8S          GE_SCRIPT_EXPORT(n:R8S) = 41,

    /**
     * 8-bit 2-channel pixel format, signed normalized.
     */
    kRG8S         GE_SCRIPT_EXPORT(n:RG8S) = 42,

    /**
     * 8-bit 4-channel pixel format, signed normalized.
     */
    kRGBA8S       GE_SCRIPT_EXPORT(n:RGBA8S) = 43,

    /**
     * 16-bit 1-channel pixel format, signed integer.
     */
    kR16I         GE_SCRIPT_EXPORT(n:R16I) = 44,

    /**
     * 16-bit 2-channel pixel format, signed integer.
     */
    kRG16I        GE_SCRIPT_EXPORT(n:RG16I) = 45,

    /**
     * 16-bit 4-channel pixel format, signed integer.
     */
    kRGBA16I      GE_SCRIPT_EXPORT(n:RGBA16I) = 46,

    /**
     * 16-bit 1-channel pixel format, unsigned integer.
     */
    kR16U         GE_SCRIPT_EXPORT(n:R16U) = 47,

    /**
     * 16-bit 2-channel pixel format, unsigned integer.
     */
    kRG16U        GE_SCRIPT_EXPORT(n:RG16U) = 48,

    /**
     * 16-bit 4-channel pixel format, unsigned integer.
     */
    kRGBA16U      GE_SCRIPT_EXPORT(n:RGBA16U) = 49,

    /**
     * 32-bit 1-channel pixel format, signed integer.
     */
    kR32I         GE_SCRIPT_EXPORT(n:R32I) = 50,

    /**
     * 32-bit 2-channel pixel format, signed integer.
     */
    kRG32I        GE_SCRIPT_EXPORT(n:RG32I) = 51,

    /**
     * 32-bit 3-channel pixel format, signed integer.
     */
    kRGB32I       GE_SCRIPT_EXPORT(n:RGB32I) = 52,

    /**
     * 32-bit 4-channel pixel format, signed integer.
     */
    kRGBA32I      GE_SCRIPT_EXPORT(n:RGBA32I) = 53,

    /**
     * 32-bit 1-channel pixel format, unsigned integer.
     */
    kR32U         GE_SCRIPT_EXPORT(n:R32U) = 54,

    /**
     * 32-bit 2-channel pixel format, unsigned integer.
     */
    kRG32U        GE_SCRIPT_EXPORT(n:RG32U) = 55,

    /**
     * 32-bit 3-channel pixel format, unsigned integer.
     */
    kRGB32U       GE_SCRIPT_EXPORT(n:RGB32U) = 56,

    /**
     * 32-bit 4-channel pixel format, unsigned integer.
     */
    kRGBA32U      GE_SCRIPT_EXPORT(n:RGBA32U) = 57,

    /**
     * 16-bit 1-channel pixel format, signed normalized.
     */
    kR16S         GE_SCRIPT_EXPORT(n:R16S) = 58,

    /**
     * 16-bit 2-channel pixel format, signed normalized.
     */
    kRG16S        GE_SCRIPT_EXPORT(n:RG16S) = 59,

    /**
     * 16-bit 4-channel pixel format, signed normalized.
     */
    kRGBA16S      GE_SCRIPT_EXPORT(n:RGBA16S) = 60,

    /**
     * 16-bit 1-channel pixel format, unsigned normalized.
     */
    kR16          GE_SCRIPT_EXPORT(n:R16) = 61,

    /**
     * 16-bit 2-channel pixel format, unsigned normalized.
     */
    kRG16         GE_SCRIPT_EXPORT(n:RG16) = 62,

    /**
     * 16-bit 3-channel pixel format, unsigned normalized.
     */
    kRGB16        GE_SCRIPT_EXPORT(n:RGB16) = 63,

    /**
     * 16-bit 4-channel pixel format, unsigned normalized.
     */
    kRGBA16       GE_SCRIPT_EXPORT(n:RGBA16) = 64,

    /**
     * Number of pixel formats currently defined.
     */
    kCOUNT        GE_SCRIPT_EXPORT(ex:true)
  };

  /**
   * @brief Flags defining some properties of pixel formats.
   */
  namespace PIXEL_FORMAT_FLAGS {
    enum E {
      /**
       * This format has an alpha channel.
       */
      kHASALPHA = 0x1,

      /**
       * This format is compressed. This invalidates the values in elemBytes,
       * elemBits and the bit counts as these might not be fixed in a
       * compressed format.
       */
      kCOMPRESSED = 0x2,

      /**
       * This is a floating point format.
       */
      kFLOAT = 0x4,

      /**
       * This is a depth format (for depth textures).
       */
      kDEPTH = 0x8,

      /**
       * This format stores data internally as integers.
       */
      kINTEGER = 0x10,

      /**
       * Format contains signed data. Absence of this flag implies unsigned
       * data.
       */
      kSIGNED = 0x20,

      /**
       * Format contains normalized data.
       * This will be [0, 1] for unsigned, and [-1,1] for signed formats.
       */
      kNORMALIZED = 0x40
    };
  }

  /**
   * @brief Types used for individual components of a pixel.
   */
  namespace PIXEL_COMPONENT_TYPE {
    enum E {
      /**
       * 8-bit integer per component
       */
      kBYTE = 0,

      /**
       * 16-bit integer per component.
       */
      kSHORT = 1,

      /**
       * 32-bit integer per component.
       */
      kINT = 2,

      /**
       * 16 bit float per component
       */
      kFLOAT16 = 3,

      /**
       * 32 bit float per component
       */
      kFLOAT32 = 4,

      /**
       * 11 bits for first two components, 10 for third component.
       */
      kPACKED_R11G11B10 = 5,

      /**
       * 10 bits for first three components, 2 bits for last component
       */
      kPACKED_R10G10B10A2 = 6,

      /**
       * Number of pixel types
       */
      kCOUNT
    };
  }

  /**
   * @brief Determines how are texture pixels filtered during sampling.
   */
  namespace TEXTURE_FILTER {
    enum E {
      /**
       * Pixel nearest to the sampled location is chosen.
       */
      kNEAREST,

      /**
       * Four pixels nearest to the sampled location are interpolated to yield
       * the sampled color.
       */
      kBILINEAR
    };
  }

  /**
   * @brief A list of cubemap faces.
   */
  namespace CUBEMAP_FACE {
    enum E {
      kPositiveX,
      kNegativeX,
      kPositiveY,
      kNegativeY,
      kPositiveZ,
      kNegativeZ
    };
  }

  /**
   * @brief A buffer describing a volume (3D), image (2D) or line (1D) of
   *        pixels in memory. Pixels are stored as a succession of "depth"
   *        slices, each containing "height" rows of "width" pixels.
   * @note  If using the constructor instead of create() you must call
   *        GPUResourceData::allocateInternalBuffer or set the buffer in some
   *        other way before reading / writing from this object, as by the
   *        default there is no buffer allocated.
   * @see GPUResourceData
   */
  class GE_CORE_EXPORT GE_SCRIPT_EXPORT() PixelData final
    : public GPUResourceData
  {
   public:
    PixelData();
    ~PixelData() = default;

    /**
     * @brief Constructs a new object with an internal buffer capable of
     *        holding "extents" volume of pixels, where each pixel is of the
     *        specified pixel format. Extent offsets are also stored, but are
     *        not used internally.
     */
    PixelData(const PixelVolume& extents, PixelFormat pixelFormat);

    /**
     * @brief Constructs a new object with an internal buffer capable of
     *        holding volume of pixels described by	provided width, height and
     *        depth, where each pixel is of the specified pixel format.
     */
    PixelData(uint32 width,
              uint32 height,
              uint32 depth,
              PixelFormat pixelFormat);

    PixelData(const PixelData& copy);
    PixelData& operator=(const PixelData& rhs);

    /**
     * @brief Returns the number of pixels that offsets one row from another.
     *        This can be "width", but doesn't have to be as some buffers
     *        require padding.
     */
    GE_SCRIPT_EXPORT(n:RawRowPitch, pr:getter)
    uint32
    getRowPitch() const {
      return m_rowPitch;
    }

    /**
     * @brief Returns the number of pixels that offsets one depth slice from
     *        another. This can be "width * height", but doesn't have to be as
     *        some buffers require padding.
     */
    GE_SCRIPT_EXPORT(n:RawSlicePitch, pr:getter)
    uint32
    getSlicePitch() const {
      return m_slicePitch;
    }

    /**
     * @brief Sets the pitch (in pixels) that determines offset between rows of
     *        the pixel buffer. Call this before allocating the buffer.
     */
    void
    setRowPitch(uint32 rowPitch) {
      m_rowPitch = rowPitch;
    }

    /**
     * @brief Sets the pitch (in pixels) that determines offset between depth
     *        slices of the pixel buffer. Call this before allocating the
     *        buffer.
     */
    void
    setSlicePitch(uint32 slicePitch) {
      m_slicePitch = slicePitch;
    }

    /**
     * @brief Returns the number of extra pixels in a row (non-zero only if
     *        rows are not consecutive (row pitch is larger than width)).
     */
    uint32
    getRowSkip() const {
      return m_rowPitch - getWidth();
    }

    /**
     * @brief Returns the number of extra pixels in a depth slice (non-zero
     *        only if slices aren't consecutive (slice pitch is larger than
     *        width*height).
     */
    uint32
    getSliceSkip() const {
      return m_slicePitch - (getHeight() * m_rowPitch);
    }

    /**
     * @brief Returns the pixel format used by the internal buffer for storing
     *        the pixels.
     */
    GE_SCRIPT_EXPORT(n:Format, pr:getter)
    PixelFormat
    getFormat() const {
      return m_format;
    }

    /**
     * @brief Returns width of the buffer in pixels.
     */
    uint32
    getWidth() const {
      return m_extents.getWidth();
    }

    /**
     * @brief Returns height of the buffer in pixels.
     */
    uint32
    getHeight() const {
      return m_extents.getHeight();
    }

    /**
     * @brief Returns depth of the buffer in pixels.
     */
    uint32
    getDepth() const {
      return m_extents.getDepth();
    }

    /**
     * @brief Returns left-most start of the pixel volume. This value is not
     *        used internally in any way. It is just passed through from the
     *        constructor.
     */
    uint32
    getLeft() const {
      return m_extents.left;
    }

    /**
     * @brief Returns right-most end of the pixel volume. This value is not
     *        used internally in any way. It is just passed through from the
     *        constructor.
     */
    uint32
    getRight() const {
      return m_extents.right;
    }

    /**
     * @brief Returns top-most start of the pixel volume. This value is not
     *        used internally in any way. It is just passed through from the
     *        constructor.
     */
    uint32
    getTop() const {
      return m_extents.top;
    }

    /**
     * @brief Returns bottom-most end of the pixel volume. This value is not
     *        used internally in any way. It is just passed through from the
     *        constructor.
     */
    uint32
    getBottom() const {
      return m_extents.bottom;
    }

    /**
     * @brief Returns front-most start of the pixel volume. This value is not
     *        used internally in any way. It is just passed through from the
     *        constructor.
     */
    uint32
    getFront() const {
      return m_extents.front;
    }

    /**
     * @brief Returns back-most end of the pixel volume. This value is not used
     *        internally in any way. It is just passed through from the
     *        constructor.
     */
    uint32
    getBack() const {
      return m_extents.back;
    }

    /**
     * @brief Returns extents of the pixel volume this object is capable of
     *        holding.
     */
    GE_SCRIPT_EXPORT(n:Extents, pr:getter)
    PixelVolume
    getExtents() const {
      return m_extents;
    }

    /**
     * @brief Return whether this buffer is laid out consecutive in memory
     *        (meaning the pitches are equal to the dimensions).
     */
    GE_SCRIPT_EXPORT(n:RawIsConsecutive, pr:getter)
    bool
    isConsecutive() const {
      return m_rowPitch == getWidth() && m_slicePitch == getWidth()*getHeight();
    }

    /**
     * @brief Return the size (in bytes) this image would take if it was laid
     *        out consecutive in memory.
     */
    uint32
    getConsecutiveSize() const;

    /**
     * @brief Return the size (in bytes) of the buffer this image requires.
     */
    GE_SCRIPT_EXPORT(n:RawSize, pr:getter)
    uint32
    getSize() const;

    /**
     * @brief Returns pixel data containing a sub-volume of this object.
     *        Returned data will not have its own buffer, but will instead
     *        point to this one. It is up to the caller to ensure this object
     *        outlives any sub-volume objects.
     */
    PixelData
    getSubVolume(const PixelVolume& volume) const;

    /**
     * @brief Samples a color at the specified coordinates using a specific
     *        filter.
     * @param[in] coords  Coordinates to sample the color at. They start at top
     *            left corner (0, 0), and are in range [0, 1].
     * @param[in] filter  Filtering mode to use when sampling the color.
     * @return Sampled color.
     */
    LinearColor
    sampleColorAt(const Vector2& coords,
                  TEXTURE_FILTER::E filter = TEXTURE_FILTER::kBILINEAR) const;

    /**
     * @brief Returns pixel color at the specified coordinates.
     */
    LinearColor
    getColorAt(uint32 x, uint32 y, uint32 z = 0) const;

    /**
     * @brief Sets the pixel color at the specified coordinates.
     */
    void
    setColorAt(const LinearColor& color, uint32 x, uint32 y, uint32 z = 0);

    /**
     * @brief Converts all the internal data into an array of colors. Array is
     *        mapped as such: arrayIdx = x + y * width + z * width * height.
     */
    Vector<LinearColor>
    getColors() const;

    /**
     * @brief Initializes the internal buffer with the provided set of colors.
     *        The array should be of width * height * depth size and mapped as
     *        such: arrayIdx = x + y * width + z * width * height.
     */
    void
    setColors(const Vector<LinearColor>& colors);

    /**
     * @brief Initializes the internal buffer with the provided set of colors.
     *        The array should be of width * height * depth size and mapped as
     *        such: arrayIdx = x + y * width + z * width * height.
     */
    void
    setColors(LinearColor* colors, uint32 numElements);

    /**
     * @brief Initializes all the pixels with a single color.
     */
    void
    setColors(const LinearColor& color);

    /**
     * @brief Interprets pixel data as depth information as retrieved from the
     *        GPU's depth buffer. Converts the device specific depth value to
     *        range [0, 1] and returns it.
     */
    float
    getDepthAt(uint32 x, uint32 y, uint32 z = 0) const;

    /**
     * @brief Converts all the internal data into an array of floats as if each
     *        individual pixel is retrieved with getDepthAt(). Array is mapped
     *        as such: arrayIdx = x + y * width + z * width * height.
     */
    Vector<float>
    getDepths() const;

    /**
     * @brief Constructs a new object with an internal buffer capable of
     *        holding "extents" volume of pixels, where each pixel is of the
     *        specified pixel format. Extent offsets are also stored, but are
     *        not used internally.
     */
    static SPtr<PixelData>
    create(const PixelVolume& extents, PixelFormat pixelFormat);

    /**
     * @brief Constructs a new object with an internal buffer capable of
     *        holding volume of pixels described by provided width, height and
     *        depth, where each pixel is of the specified pixel format.
     */
    static SPtr<PixelData>
    create(uint32 width, uint32 height, uint32 depth, PixelFormat pixelFormat);

   private:
    /**
     * @brief Initializes the internal buffer with the provided set of colors.
     *        The array should be of width * height * depth size and mapped as
     *        such: arrayIdx = x + y * width + z * width * height.
     * @note  A generic method that is reused in other more specific
     *        setColors() calls.
     */
    template<class T>
    void
    setColorsInternal(const T& colors, uint32 numElements);

    /**
     * @brief Returns the needed size of the internal buffer, in bytes.
     */
    uint32
    getInternalBufferSize() const override;

   private:
    PixelVolume m_extents;
    PixelFormat m_format;
    uint32 m_rowPitch;
    uint32 m_slicePitch;

    /*************************************************************************/
    /**
     * Serialization
     */
    /*************************************************************************/
   public:
    friend class PixelDataRTTI;

    static RTTITypeBase*
    getRTTIStatic();

    RTTITypeBase*
    getRTTI() const override;
  };
}
