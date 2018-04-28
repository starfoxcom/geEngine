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
#pragma once

/*****************************************************************************/
/**
 * Includes
 */
/*****************************************************************************/
#include "gePrerequisitesCore.h"

#include <geMatrix4.h>
#include <geVector3.h>
#include <geRotator.h>
#include <geRTTIType.h>

namespace geEngineSDK {
  class GE_CORE_EXPORT Transform : public IReflectable
  {
   public:
    Transform();
    Transform(const Vector3& position, const Rotator& rotation, const Vector3& scale);

    /**
     * @brief Sets the local position of the object.
     */
    void
    setPosition(const Vector3& position) { m_position = position; }

    /**
     * @brief Gets the local position of the object.
     */
    const Vector3&
    getPosition() const {
      return m_position;
    }

    /**
     * @brief Shorthand for getPosition().
     */
    const Vector3&
    pos() const {
      return m_position;
    }

    /**
     * @brief Sets the local rotation of the object.
     */
    void
    setRotation(const Quaternion& rotation) {
      m_rotation = rotation;
    }

    /**
     * @brief Gets the local rotation of the object.
     */
    const Quaternion&
    getRotation() const {
      return m_rotation;
    }

    /**
     * @brief Shorthand for getRotation().
     */
    const Quaternion&
    rot() const {
      return m_rotation;
    }

    /**
     * @brief Sets the local scale of the object.
     */
    void
    setScale(const Vector3& scale) {
      m_scale = scale;
    }

    /**
     * @brief Gets the local scale of the object.
     */
    const Vector3&
    getScale() const {
      return m_scale;
    }

    /**
     * @brief Shorthand for getScale().
     */
    const Vector3&
    scl() const {
      return m_scale;
    }

    /**
     * @brief Converts the provided world position to a space relative to the
     *        provided parent, and sets it as the current transform's position.
     */
    void
    setWorldPosition(const Vector3& position, const Transform& parent);

    /**
     * @brief Converts the provided world rotation to a space relative to the
     *        provided parent, and sets it as the current transform's rotation.
     */
    void
    setWorldRotation(const Quaternion& rotation, const Transform& parent);

    /**
     * @brief Converts the provided world scale to a space relative to the
     *        provided parent, and sets it as the current transform's scale.
     */
    void
    setWorldScale(const Vector3& scale, const Transform& parent);

    /**
     * @brief Builds the transform matrix from current translation, rotation
     *        and scale properties.
     */
    Matrix4
    getMatrix() const;

    /**
     * @brief Builds the inverse transform matrix from current translation,
     *        rotation and scale properties.
     */
    Matrix4
    getInvMatrix() const;

    /**
     * @brief Makes the current transform relative to the provided transform.
     *        In another words, converts from a world coordinate system to one
     *        local to the provided transform.
     */
    void
    makeLocal(const Transform& parent);

    /**
     * @brief Makes the current transform absolute. In another words, converts
     *        from a local coordinate system relative to the provided
     *        transform, to a world coordinate system.
     */
    void
    makeWorld(const Transform& parent);

    /**
     * @brief Orients the object so it is looking at the provided @p location
     *        (world space) where @p up is used for determining the location of
     *        the object's Y axis.
     */
    void
    lookAt(const Vector3& location, const Vector3& up = Vector3::UP);

    /**
     * @brief Moves the object's position by the vector offset provided along
     *        world axes.
     */
    void
    move(const Vector3& vec);

    /**
     * @brief Moves the object's position by the vector offset provided along
     *        it's own axes (relative to orientation).
     */
    void
    moveRelative(const Vector3& vec);

    /**
     * @brief Gets the forward axis of the object.
     * @return  Forward axis of the object.
     */
    Vector3
    getForward() const {
      return getRotation().getForwardVector();
    }

    /**
     * @brief Gets the up axis of the object.
     * @return  Up axis of the object.
     */
    Vector3
    getUp() const {
      return getRotation().getUpVector();
    }

    /**
     * @brief Gets the right axis of the object.
     * @return  Right axis of the object.
     */
    Vector3
    getRight() const {
      return getRotation().getRightVector();
    }

    /**
     * @brief Rotates the game object so it's forward axis faces the provided
     *        direction.
     * @param[in] forwardDir  The forward direction to face.
     * @note  Local forward axis is considered to be negative Z.
     */
    void
    setForward(const Vector3& forwardDir);

    /**
     * @brief Rotate the object around an arbitrary axis.
     */
    void
    rotate(const Vector3& axis, const Radian& angle);

    /**
     * @brief Rotate the object around an arbitrary axis using a Quaternion.
     */
    void
    rotate(const Quaternion& q);

    /**
     * @brief Rotates around local front axis.
     * @param[in] angle Angle to rotate by.
     */
    void
    roll(const Radian& angle);

    /**
     * @brief Rotates around up axis.
     * @param[in] angle Angle to rotate by.
     */
    void
    yaw(const Radian& angle);

    /**
     * @brief Rotates around X axis
     * @param[in] angle Angle to rotate by.
     */
    void
    pitch(const Radian& angle);
   private:
    Vector3 m_position;
    Rotator m_rotation;
    Vector3 m_scale;

    /*************************************************************************/
    /**
     * Serialization
     */
    /*************************************************************************/
   public:
    friend class TransformRTTI;
    
    static RTTITypeBase*
    getRTTIStatic();

    RTTITypeBase*
    getRTTI() const override;
  };
}
