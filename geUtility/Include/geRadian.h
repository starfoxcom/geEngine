/*****************************************************************************/
/**
 * @file    geRadian.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2015/04/04
 * @brief   Wrapper class which indicates a given angle value is in Radians.
 *
 * Radian values are interchangeable with Degree values, and conversions will
 * be done automatically between them.
 *
 * @bug	    No known bugs.
/*****************************************************************************/
#pragma once

/*****************************************************************************/
/**
 * Includes
 */
/*****************************************************************************/
#include "gePlatformDefines.h"
#include "geRTTIPrerequisites.h"

namespace geEngineSDK {
  class Degree;

  /**
   * @brief Wrapper class which indicates a given angle value is in Radians.
   * @note  Radian values are interchangeable with Degree values, and
   *        conversions will be done automatically between them.
   */
  class GE_UTILITY_EXPORT Radian
  {
   public:
    explicit Radian(float r = 0.0f) : m_radian(r) {}
    Radian(const Degree& d);

    Radian&
    operator=(const float& f) {
      m_radian = f;
      return *this;
    }

    Radian&
    operator=(const Radian& r) {
      m_radian = r.m_radian;
      return *this;
    }
    
    Radian&
    operator=(const Degree& d);

    FORCEINLINE float
    valueDegrees() const;

    FORCEINLINE float
    valueRadians() const {
      return m_radian;
    }

    /**
    * @brief Utility to ensure angle is between +/- PI range by unwinding.
    */
    void
    unwindRadians();

    const Radian&
    operator+() const {
      return *this;
    }

    Radian
    operator+(const Radian& r) const {
      return Radian(m_radian + r.m_radian);
    }

    Radian
    operator+(const Degree& d) const;

    Radian&
    operator+=(const Radian& r) {
      m_radian += r.m_radian;
      return *this;
    }
    
    Radian&
    operator+=(const Degree& d);

    Radian
    operator-() const {
      return Radian(-m_radian);
    }
    
    Radian
    operator-(const Radian& r) const {
      return Radian(m_radian - r.m_radian);
    }
    
    Radian
    operator-(const Degree& d) const;

    Radian&
    operator-=(const Radian& r) {
      m_radian -= r.m_radian;
      return *this;
    }
    
    Radian&
    operator-=(const Degree& d);

    Radian
    operator*(float f) const {
      return Radian(m_radian * f);
    }
    
    Radian
    operator*(const Radian& f) const {
      return Radian(m_radian * f.m_radian);
    }
    
    Radian&
    operator*=(float f) {
      m_radian *= f;
      return *this;
    }

    Radian
    operator/(float f) const {
      return Radian(m_radian / f);
    }
    
    Radian&
    operator/=(float f) {
      m_radian /= f;
      return *this;
    }

    bool
    operator<(const Radian& r) const {
      return m_radian <  r.m_radian;
    }
    
    bool
    operator<=(const Radian& r) const {
      return m_radian <= r.m_radian;
    }
    
    bool
    operator==(const Radian& r) const {
      return m_radian == r.m_radian;
    }
    
    bool
    operator!=(const Radian& r) const {
      return m_radian != r.m_radian;
    }
    
    bool
    operator>=(const Radian& r) const {
      return m_radian >= r.m_radian;
    }
    
    bool
    operator>(const Radian& r) const {
      return m_radian >  r.m_radian;
    }

    bool
    operator<(const float& v) const {
      return m_radian <  v;
    }
    
    bool
    operator<=(const float& v) const {
      return m_radian <= v;
    }
    
    bool
    operator==(const float& v) const {
      return m_radian == v;
    }
    
    bool
    operator!=(const float& v) const {
      return m_radian != v;
    }
    
    bool
    operator>=(const float& v) const {
      return m_radian >= v;
    }
    
    bool operator>(const float& v) const {
      return m_radian >  v;
    }

    friend Radian
    operator*(float lhs, const Radian& rhs) {
      return Radian(lhs * rhs.m_radian);
    }
    
    friend Radian
    operator/(float lhs, const Radian& rhs) {
      return Radian(lhs / rhs.m_radian);
    }
    
    friend Radian
    operator+(Radian& lhs, float rhs) {
      return Radian(lhs.m_radian + rhs);
    }
    
    friend Radian
    operator+(float lhs, const Radian& rhs) {
      return Radian(lhs + rhs.m_radian);
    }
    
    friend Radian
    operator-(const Radian& lhs, float rhs) {
      return Radian(lhs.m_radian - rhs);
    }
    
    friend Radian
    operator-(const float lhs, const Radian& rhs) {
      return Radian(lhs - rhs.m_radian);
    }

    friend bool
    operator<(float lhs, const Radian& rhs) {
      return lhs < rhs.m_radian;
    }

    friend bool
    operator<=(float lhs, const Radian& rhs) {
      return lhs <= rhs.m_radian;
    }

    friend bool
    operator==(float lhs, const Radian& rhs) {
      return lhs == rhs.m_radian;
    }

    friend bool
    operator!=(float lhs, const Radian& rhs) {
      return lhs != rhs.m_radian;
    }

    friend bool
    operator>=(float lhs, const Radian& rhs) {
      return lhs >= rhs.m_radian;
    }

    friend bool
    operator>(float lhs, const Radian& rhs) {
      return lhs > rhs.m_radian;
    }

   private:
    float m_radian;
  };

  GE_ALLOW_MEMCPY_SERIALIZATION(Radian);
}
