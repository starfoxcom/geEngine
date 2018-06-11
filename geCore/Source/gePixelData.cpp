/*****************************************************************************/
/**
 * @file    gePixelData.cpp
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

/*****************************************************************************/
/**
 * Includes
 */
/*****************************************************************************/
#include "gePixelData.h"
#include "gePixelUtil.h"
#include "gePixelDataRTTI.h"

#include <geColor.h>
#include <geVector2.h>
#include <geMath.h>
#include <geDebug.h>

namespace geEngineSDK {
  PixelData::PixelData()
    : m_extents(0, 0, 0, 0),
      m_format(PixelFormat::kUNKNOWN),
      m_rowPitch(0),
      m_slicePitch(0)
  {}

  PixelData::PixelData(const PixelVolume& extents, PixelFormat pixelFormat)
    : m_extents(extents),
      m_format(pixelFormat) {
    PixelUtil::getPitch(extents.getWidth(),
                        extents.getHeight(),
                        extents.getDepth(),
                        pixelFormat,
                        m_rowPitch,
                        m_slicePitch);
  }

  PixelData::PixelData(uint32 width,
                       uint32 height,
                       uint32 depth,
                       PixelFormat pixelFormat)
    : m_extents(0, 0, 0, width, height, depth),
      m_format(pixelFormat) {
    PixelUtil::getPitch(width, height, depth, pixelFormat, m_rowPitch, m_slicePitch);
  }

  PixelData::PixelData(const PixelData& copy)
    : GPUResourceData(copy),
      m_format(copy.m_format),
      m_rowPitch(copy.m_rowPitch),
      m_slicePitch(copy.m_slicePitch),
      m_extents(copy.m_extents)
  {}

  PixelData&
  PixelData::operator=(const PixelData& rhs) {
    GPUResourceData::operator=(rhs);

    m_format = rhs.m_format;
    m_rowPitch = rhs.m_rowPitch;
    m_slicePitch = rhs.m_slicePitch;
    m_extents = rhs.m_extents;

    return *this;
  }

  uint32
  PixelData::getConsecutiveSize() const {
    return PixelUtil::getMemorySize(getWidth(),
                                    getHeight(),
                                    getDepth(),
                                    m_format);
  }

  uint32
  PixelData::getSize() const {
    return PixelUtil::getMemorySize(m_rowPitch,
                                    m_slicePitch / m_rowPitch,
                                    getDepth(),
                                    getFormat());
  }

  PixelData
  PixelData::getSubVolume(const PixelVolume& volume) const {
    if (PixelUtil::isCompressed(m_format)) {
      if (volume.left == getLeft() &&
          volume.top == getTop() &&
          volume.front == getFront() &&
          volume.right == getRight() &&
          volume.bottom == getBottom() &&
          volume.back == getBack()) {
        //Entire buffer is being queried
        return *this;
      }

      GE_EXCEPT(InvalidParametersException,
                "Cannot return subvolume of compressed PixelBuffer");
    }

    if (!m_extents.contains(volume)) {
      GE_EXCEPT(InvalidParametersException, "Bounds out of range");
    }

    const SIZE_T elemSize = PixelUtil::getNumElemBytes(m_format);
    PixelData rval(volume.getWidth(),
                   volume.getHeight(),
                   volume.getDepth(),
                   m_format);

    rval.setExternalBuffer((reinterpret_cast<uint8*>(getData()) ) +
                           ((volume.left - getLeft()) * elemSize) +
                           ((volume.top - getTop()) * m_rowPitch * elemSize) +
                           ((volume.front - getFront()) * m_slicePitch * elemSize));

    rval.m_format = m_format;
    PixelUtil::getPitch(volume.getWidth(),
                        volume.getHeight(),
                        volume.getDepth(),
                        m_format,
                        rval.m_rowPitch,
                        rval.m_slicePitch);
    return rval;
  }

  LinearColor
  PixelData::sampleColorAt(const Vector2& coords,
                           TEXTURE_FILTER::E filter) const {
    Vector2 pixelCoords = coords * Vector2(static_cast<float>(m_extents.getWidth()),
                                           static_cast<float>(m_extents.getHeight()));

    int32 maxExtentX = Math::max(0, static_cast<int32>(m_extents.getWidth()) - 1);
    int32 maxExtentY = Math::max(0, static_cast<int32>(m_extents.getHeight()) - 1);

    if (TEXTURE_FILTER::kBILINEAR == filter) {
      pixelCoords -= Vector2(0.5f, 0.5f);

      uint32 x = static_cast<uint32>(Math::clamp(Math::trunc(pixelCoords.x), 0, maxExtentX));
      uint32 y = static_cast<uint32>(Math::clamp(Math::trunc(pixelCoords.y), 0, maxExtentY));

      float fracX = pixelCoords.x - x;
      float fracY = pixelCoords.y - y;

      x = Math::clamp(x, 0U, static_cast<uint32>(maxExtentX));
      y = Math::clamp(y, 0U, static_cast<uint32>(maxExtentY));

      int32 x1 = Math::clamp(x + 1, 0U, static_cast<uint32>(maxExtentX));
      int32 y1 = Math::clamp(y + 1, 0U, static_cast<uint32>(maxExtentY));

      LinearColor color = LinearColor::Transparent;
      color += (1.0f - fracX) * (1.0f - fracY) * getColorAt(x, y);
      color += fracX * (1.0f - fracY) * getColorAt(x1, y);
      color += (1.0f - fracX) * fracY * getColorAt(x, y1);
      color += fracX * fracY * getColorAt(x1, y1);

      return color;
    }

    uint32 x = static_cast<uint32>(Math::clamp(Math::trunc(pixelCoords.x), 0, maxExtentX));
    uint32 y = static_cast<uint32>(Math::clamp(Math::trunc(pixelCoords.y), 0, maxExtentY));

    return getColorAt(x, y);
  }

  LinearColor
  PixelData::getColorAt(uint32 x, uint32 y, uint32 z) const {
    LinearColor cv;

    uint32 pixelSize = PixelUtil::getNumElemBytes(m_format);
    uint32 pixelOffset = pixelSize * (z * m_slicePitch + y * m_rowPitch + x);
    PixelUtil::unpackColor(&cv, m_format, getData() + pixelOffset);

    return cv;
  }

  void
  PixelData::setColorAt(const LinearColor& color, uint32 x, uint32 y, uint32 z) {
    uint32 pixelSize = PixelUtil::getNumElemBytes(m_format);
    uint32 pixelOffset = pixelSize * (z * m_slicePitch + y * m_rowPitch + x);
    PixelUtil::packColor(color, m_format, getData() + pixelOffset);
  }

  Vector<LinearColor>
  PixelData::getColors() const {
    uint32 depth = m_extents.getDepth();
    uint32 height = m_extents.getHeight();
    uint32 width = m_extents.getWidth();

    uint32 pixelSize = PixelUtil::getNumElemBytes(m_format);
    uint8* data = getData();

    Vector<LinearColor> colors(width * height * depth);
    for (uint32 z = 0; z < depth; ++z) {
      uint32 zArrayIdx = z * width * height;
      uint32 zDataIdx = z * m_slicePitch * pixelSize;

      for (uint32 y = 0; y < height; ++y) {
        uint32 yArrayIdx = y * width;
        uint32 yDataIdx = y * m_rowPitch * pixelSize;

        for (uint32 x = 0; x < width; ++x) {
          uint32 arrayIdx = x + yArrayIdx + zArrayIdx;
          uint32 dataIdx = x * pixelSize + yDataIdx + zDataIdx;

          uint8* dest = data + dataIdx;
          PixelUtil::unpackColor(&colors[arrayIdx], m_format, dest);
        }
      }
    }

    return colors;
  }

  template<class T>
  void PixelData::setColorsInternal(const T& colors, uint32 numElements) {
    uint32 depth = m_extents.getDepth();
    uint32 height = m_extents.getHeight();
    uint32 width = m_extents.getWidth();

    uint32 totalNumElements = width * height * depth;
    if (numElements != totalNumElements) {
      LOGERR("Unable to set colors, invalid array size.");
      return;
    }

    uint32 pixelSize = PixelUtil::getNumElemBytes(m_format);
    uint8* data = getData();

    for (uint32 z = 0; z < depth; ++z) {
      uint32 zArrayIdx = z * width * height;
      uint32 zDataIdx = z * m_slicePitch * pixelSize;

      for (uint32 y = 0; y < height; ++y) {
        uint32 yArrayIdx = y * width;
        uint32 yDataIdx = y * m_rowPitch * pixelSize;

        for (uint32 x = 0; x < width; ++x) {
          uint32 arrayIdx = x + yArrayIdx + zArrayIdx;
          uint32 dataIdx = x * pixelSize + yDataIdx + zDataIdx;

          uint8* dest = data + dataIdx;
          PixelUtil::packColor(colors[arrayIdx], m_format, dest);
        }
      }
    }
  }

  template
  GE_CORE_EXPORT void
  PixelData::setColorsInternal(LinearColor* const &, uint32);

  template
  GE_CORE_EXPORT void
  PixelData::setColorsInternal(const Vector<LinearColor>&, uint32);

  void
  PixelData::setColors(const Vector<LinearColor>& colors) {
    setColorsInternal(colors, static_cast<uint32>(colors.size()));
  }

  void
  PixelData::setColors(LinearColor* colors, uint32 numElements) {
    setColorsInternal(colors, numElements);
  }

  void
  PixelData::setColors(const LinearColor& color) {
    uint32 depth = m_extents.getDepth();
    uint32 height = m_extents.getHeight();
    uint32 width = m_extents.getWidth();

    uint32 pixelSize = PixelUtil::getNumElemBytes(m_format);
    uint32 packedColor[4];
    GE_ASSERT(sizeof(packedColor) >= pixelSize);

    PixelUtil::packColor(color, m_format, packedColor);

    uint8* data = getData();
    for (uint32 z = 0; z < depth; ++z) {
      uint32 zDataIdx = z * m_slicePitch * pixelSize;

      for (uint32 y = 0; y < height; ++y) {
        uint32 yDataIdx = y * m_rowPitch * pixelSize;

        for (uint32 x = 0; x < width; ++x) {
          uint32 dataIdx = x * pixelSize + yDataIdx + zDataIdx;

          uint8* dest = data + dataIdx;
          memcpy(dest, packedColor, pixelSize);
        }
      }
    }
  }

  float
  PixelData::getDepthAt(uint32 x, uint32 y, uint32 z) const {
    uint32 pixelSize = PixelUtil::getNumElemBytes(m_format);
    uint32 pixelOffset = pixelSize * (z * m_slicePitch + y * m_rowPitch + x);
    return PixelUtil::unpackDepth(m_format, getData() + pixelOffset);
  }

  Vector<float>
  PixelData::getDepths() const {
    uint32 depth = m_extents.getDepth();
    uint32 height = m_extents.getHeight();
    uint32 width = m_extents.getWidth();

    uint32 pixelSize = PixelUtil::getNumElemBytes(m_format);
    uint8* data = getData();

    Vector<float> depths(width * height * depth);
    for (uint32 z = 0; z < depth; ++z) {
      uint32 zArrayIdx = z * width * height;
      uint32 zDataIdx = z * m_slicePitch * pixelSize;

      for (uint32 y = 0; y < height; ++y) {
        uint32 yArrayIdx = y * width;
        uint32 yDataIdx = y * m_rowPitch * pixelSize;

        for (uint32 x = 0; x < width; ++x) {
          uint32 arrayIdx = x + yArrayIdx + zArrayIdx;
          uint32 dataIdx = x * pixelSize + yDataIdx + zDataIdx;

          uint8* dest = data + dataIdx;
          depths[arrayIdx] = PixelUtil::unpackDepth(m_format, dest);
        }
      }
    }

    return depths;
  }

  SPtr<PixelData>
  PixelData::create(const PixelVolume &extents, PixelFormat pixelFormat) {
    SPtr<PixelData>
      pixelData = ge_shared_ptr_new<PixelData>(extents, pixelFormat);
    pixelData->allocateInternalBuffer();
    return pixelData;
  }

  SPtr<PixelData>
  PixelData::create(uint32 width,
                    uint32 height,
                    uint32 depth,
                    PixelFormat pixelFormat) {
    SPtr<PixelData> pixelData = ge_shared_ptr_new<PixelData>(width,
                                                             height,
                                                             depth,
                                                             pixelFormat);
    pixelData->allocateInternalBuffer();

    return pixelData;
  }

  uint32
  PixelData::getInternalBufferSize() const {
    return getSize();
  }

  /***************************************************************************/
  /**
   * Serialization
   */
  /***************************************************************************/

  RTTITypeBase*
  PixelData::getRTTIStatic() {
    return PixelDataRTTI::instance();
  }

  RTTITypeBase*
  PixelData::getRTTI() const {
    return PixelData::getRTTIStatic();
  }
}
