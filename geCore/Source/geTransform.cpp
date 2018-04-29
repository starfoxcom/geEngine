/*****************************************************************************/
/**
 * @file    geTransform.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2018/04/28
 * @brief   Contains information about 3D object's position, rotation and scale
 *
 * Contains information about 3D object's position, rotation and scale, and
 * provides methods to manipulate it. 
 *
 * @bug     No known bugs.
 */
/*****************************************************************************/

/*****************************************************************************/
/**
 * Includes
 */
/*****************************************************************************/
#include "geTransform.h"

namespace geEngineSDK {
  Transform::Transform()
    : m_position(Vector3::ZERO),
      m_rotation(Quaternion::IDENTITY),
      m_scale(Vector3::UNIT)
  {}

  Transform::Transform(const Vector3& position,
                       const Rotator& rotation,
                       const Vector3& scale)
    : m_position(position),
      m_rotation(rotation),
      m_scale(scale)
  {}

  Matrix4
  Transform::getMatrix() const {
    return ScaleRotationTranslationMatrix(m_position, m_rotation, m_scale);
  }

  Matrix4
  Transform::getInvMatrix() const {
    return ScaleRotationTranslationMatrix(m_position, m_rotation, m_scale).inverse();
  }
}
