/*****************************************************************************/
/**
 * @file    geVertexDeclaration.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2018/05/22
 * @brief   Describes a single vertex element in a vertex declaration.
 *
 * Describes a single vertex element in a vertex declaration.
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
  /**
   * @brief Types used to identify base types of vertex element contents.
   */
  namespace VERTEX_ELEMENT_TYPE {
    enum E {
      kFLOAT1 = 0, /**< 1D floating point value */
      kFLOAT2 = 1, /**< 2D floating point value */
      kFLOAT3 = 2, /**< 3D floating point value */
      kFLOAT4 = 3, /**< 4D floating point value */
      kCOLOR = 4, /**< Color encoded in 32-bits (8-bits per channel). */
      kSHORT1 = 5, /**< 1D 16-bit signed integer value */
      kSHORT2 = 6, /**< 2D 16-bit signed integer value */
      kSHORT4 = 8, /**< 4D 16-bit signed integer value */
      kUBYTE4 = 9, /**< 4D 8-bit unsigned integer value */
      kCOLOR_ARGB = 10, /**< Color encoded in 32-bits (8-bits per channel) in ARGB order) */
      kCOLOR_ABGR = 11, /**< Color encoded in 32-bits (8-bits per channel) in ABGR order) */
      kUINT4 = 12, /**< 4D 32-bit unsigned integer value */
      kINT4 = 13,  /**< 4D 32-bit signed integer value */
      kUSHORT1 = 14, /**< 1D 16-bit unsigned integer value */
      kUSHORT2 = 15, /**< 2D 16-bit unsigned integer value */
      kUSHORT4 = 17, /**< 4D 16-bit unsigned integer value */
      kINT1 = 18,  /**< 1D 32-bit signed integer value */
      kINT2 = 19,  /**< 2D 32-bit signed integer value */
      kINT3 = 20,  /**< 3D 32-bit signed integer value */
      kUINT1 = 21,  /**< 1D 32-bit signed integer value */
      kUINT2 = 22,  /**< 2D 32-bit signed integer value */
      kUINT3 = 23,  /**< 3D 32-bit signed integer value */
      kUBYTE4_NORM = 24, /**< 4D 8-bit unsigned integer interpreted as a normalized value in [0, 1] range. */
      kCOUNT, // Keep at end before VET_UNKNOWN
      kUNKNOWN = 0xffff
    };
  }
}
