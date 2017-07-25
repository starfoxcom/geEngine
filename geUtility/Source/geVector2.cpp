/*****************************************************************************/
/**
 * @file    geVector2.cpp
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2015/04/14
 * @brief   A vector in 2-D space composed of components (X, Y).
 *
 * A vector in 2-D space composed of components (X, Y) with floating point
 * precision.
 *
 * @bug     No known bugs.
 */
/*****************************************************************************/

/*****************************************************************************/
/**
 * Includes
 */
/*****************************************************************************/
#include "geVector2.h"

namespace geEngineSDK {
  const Vector2 Vector2::ZERO = Vector2(0.f, 0.f);
  const Vector2 Vector2::UNIT = Vector2(1.f, 1.f);
  const Vector2 Vector2::UNIT_X = Vector2(1.f, 0.f);
  const Vector2 Vector2::UNIT_Y = Vector2(0.f, 1.f);
}
