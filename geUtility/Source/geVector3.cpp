/*****************************************************************************/
/**
 * @file    geVector3.cpp
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2017/06/28
 * @brief   A vector in 3-D space composed of components (X, Y, Z).
 *
 * A vector in 3-D space composed of components (X, Y, Z) with floating point
 * precision
 *
 * @bug     No known bugs.
 */
/*****************************************************************************/

/*****************************************************************************/
/**
 * Includes
 */
/*****************************************************************************/
#include "geVector3.h"

namespace geEngineSDK {
  const Vector3 Vector3::ZERO     = Vector3(0.f, 0.f, 0.f);
  const Vector3 Vector3::UNIT     = Vector3(1.f, 1.f, 1.f);
  const Vector3 Vector3::UP       = Vector3(0.f, 1.f, 0.f);
  const Vector3 Vector3::FORWARD  = Vector3(1.f, 0.f, 0.f);
}
