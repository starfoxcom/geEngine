/*****************************************************************************/
/**
 * @file    gePixelVolume.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2018/05/11
 * @brief   Represents a 3D region of pixels used for referencing pixel data.
 *
 * Represents a 3D region of pixels used for referencing pixel data.
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
  struct GE_CORE_EXPORT GE_SCRIPT_EXPORT(pl:true, n:PixelVolume) PixelVolume
  {
    PixelVolume() : left(0), top(0), right(1), bottom(1), front(0), back(1) {}

    PixelVolume(uint32 left, uint32 top, uint32 right, uint32 bottom)
      : left(left),
        top(top),
        right(right),
        bottom(bottom),
        front(0),
        back(1) {
      GE_ASSERT(right >= left && bottom >= top && back >= front);
    }

    PixelVolume(uint32 left,
                uint32 top,
                uint32 front,
                uint32 right,
                uint32 bottom,
                uint32 back)
      : left(left),
        top(top),
        right(right),
        bottom(bottom),
        front(front),
        back(back) {
      GE_ASSERT(right >= left && bottom >= top && back >= front);
    }

    /**
     * @brief Return true if the other box is a part of this one.
     */
    bool
    contains(const PixelVolume &volume) const {
      return (volume.left >= left &&
              volume.top >= top &&
              volume.front >= front &&
              volume.right <= right &&
              volume.bottom <= bottom &&
              volume.back <= back);
    }

    uint32
    getWidth() const {
      return right - left;
    }

    uint32
    getHeight() const {
      return bottom - top;
    }

    uint32
    getDepth() const {
      return back - front;
    }

    uint32 left;
    uint32 top;
    uint32 right;
    uint32 bottom;
    uint32 front;
    uint32 back;
  };
}
