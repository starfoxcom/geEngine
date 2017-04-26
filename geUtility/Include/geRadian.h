/********************************************************************/
/**
* @file   geRadian.h
* @author Samuel Prince (samuel.prince.quezada@gmail.com)
* @date   2015/04/04
* @brief  Wrapper class which indicates a given angle value is in
*		   Radians.
*
* Radian values are interchangeable with Degree values, and
* conversions will be done automatically between them.
*
* @bug	   No known bugs.
*/
/********************************************************************/
#pragma once

/************************************************************************************************************************/
/* Includes                                                                     										*/
/************************************************************************************************************************/
#include "gePrerequisitesUtil.h"
#include "geDegree.h"

namespace geEngineSDK
{
	/************************************************************************************************************************/
	/**
	*	@brief	Wrapper class which indicates a given angle value is in Radians.
	*	@note Radian values are interchangeable with Degree values, and conversions will be done automatically between them.
	*/
	/************************************************************************************************************************/
	class GE_UTILITY_EXPORT Radian
	{
	private:
		float m_Radian;

	public:
		explicit Radian(float r = 0.0f) : m_Radian(r){}
		Radian(const Degree& d);

		Radian& operator= (const float& f)	{ m_Radian = f; return *this; }
		Radian& operator= (const Radian& r)	{ m_Radian = r.m_Radian; return *this; }
		Radian& operator= (const Degree& d);

		inline float ValueDegrees() const;
		inline float ValueRadians() const { return m_Radian; }

		inline void UnwindRadians();		/** Given a heading which may be outside the +/- PI range, 'unwind' it back into that range. */

		const Radian& operator+ () const { return *this; }
		Radian operator+	(const Radian& r) const { return Radian(m_Radian + r.m_Radian); }
		Radian operator+	(const Degree& d) const;
		Radian& operator+=	(const Radian& r) { m_Radian += r.m_Radian; return *this; }
		Radian& operator+=	(const Degree& d);

		Radian operator-	() const { return Radian(-m_Radian); }
		Radian operator-	(const Radian& r) const { return Radian(m_Radian - r.m_Radian); }
		Radian operator-	(const Degree& d) const;
		Radian& operator-=	(const Radian& r) { m_Radian -= r.m_Radian; return *this; }
		Radian& operator-=	(const Degree& d);

		Radian operator*	(float f) const { return Radian(m_Radian * f); }
		Radian operator*	(const Radian& f) const { return Radian(m_Radian * f.m_Radian); }
		Radian& operator*=	(float f) { m_Radian *= f; return *this; }

		Radian operator/	(float f) const { return Radian(m_Radian / f); }
		Radian& operator/=	(float f) { m_Radian /= f; return *this; }

		bool operator<  (const Radian& r) const { return m_Radian <  r.m_Radian; }
		bool operator<= (const Radian& r) const { return m_Radian <= r.m_Radian; }
		bool operator== (const Radian& r) const { return m_Radian == r.m_Radian; }
		bool operator!= (const Radian& r) const { return m_Radian != r.m_Radian; }
		bool operator>= (const Radian& r) const { return m_Radian >= r.m_Radian; }
		bool operator>  (const Radian& r) const { return m_Radian >  r.m_Radian; }

		bool operator<  (const float& v) const { return m_Radian <  v; }
		bool operator<= (const float& v) const { return m_Radian <= v; }
		bool operator== (const float& v) const { return m_Radian == v; }
		bool operator!= (const float& v) const { return m_Radian != v; }
		bool operator>= (const float& v) const { return m_Radian >= v; }
		bool operator>  (const float& v) const { return m_Radian >  v; }

		friend Radian operator* (float lhs, const Radian& rhs)			{ return Radian(lhs * rhs.m_Radian); }
		friend Radian operator/ (float lhs, const Radian& rhs)			{ return Radian(lhs / rhs.m_Radian); }
		friend Radian operator+ (Radian& lhs, float rhs)				{ return Radian(lhs.m_Radian + rhs); }
		friend Radian operator+ (float lhs, const Radian& rhs)			{ return Radian(lhs + rhs.m_Radian); }
		friend Radian operator- (const Radian& lhs, float rhs)			{ return Radian(lhs.m_Radian - rhs); }
		friend Radian operator- (const float lhs, const Radian& rhs)	{ return Radian(lhs - rhs.m_Radian); }

		friend bool operator<  (float lhs, const Radian& rhs) { return lhs <  rhs.m_Radian; }
		friend bool operator<= (float lhs, const Radian& rhs) { return lhs <= rhs.m_Radian; }
		friend bool operator== (float lhs, const Radian& rhs) { return lhs == rhs.m_Radian; }
		friend bool operator!= (float lhs, const Radian& rhs) { return lhs != rhs.m_Radian; }
		friend bool operator>= (float lhs, const Radian& rhs) { return lhs >= rhs.m_Radian; }
		friend bool operator>  (float lhs, const Radian& rhs) { return lhs >  rhs.m_Radian; }
	};
}
