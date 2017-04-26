/********************************************************************/
/**
 * @file   geDegree.h
 * @author Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date   2015/04/04
 * @brief  Wrapper class which indicates a given angle value is in
 *		  Degrees.
 *
 * Degree values are interchangeable with Radian values, and
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
#include "geRadian.h"

namespace geEngineSDK
{
	/************************************************************************************************************************/
	/**
	*	@brief	Wrapper class which indicates a given angle value is in Degrees.
	*	@note	Degree values are interchangeable with Radian values, and conversions will be done automatically between them.
	*/
	/************************************************************************************************************************/
	class GE_UTILITY_EXPORT Degree
	{
	private:
		float m_Degree;

	public:
		explicit Degree(float d = 0.0f) : m_Degree(d) {}
		Degree(const Radian& r);

		Degree& operator= (const float& f) { m_Degree = f; return *this; }
		Degree& operator= (const Degree& d) { m_Degree = d.m_Degree; return *this; }
		Degree& operator= (const Radian& r);

		inline float ValueDegrees() const { return m_Degree; }
		inline float ValueRadians() const;

		void UnwindDegrees();		/** Utility to ensure angle is between +/- 180 degrees by unwinding. */

		const Degree& operator+ () const { return *this; }
		Degree operator+	(const Degree& d) const { return Degree(m_Degree + d.m_Degree); }
		Degree operator+	(const Radian& r) const;
		Degree& operator+=	(const Degree& d) { m_Degree += d.m_Degree; return *this; }
		Degree& operator+=	(const Radian& r);

		Degree operator-	() const { return Degree(-m_Degree); }
		Degree operator-	(const Degree& d) const { return Degree(m_Degree - d.m_Degree); }
		Degree operator-	(const Radian& r) const;
		Degree& operator-=	(const Degree& d) { m_Degree -= d.m_Degree; return *this; }
		Degree& operator-=	(const Radian& r);

		Degree operator*	(float f) const { return Degree(m_Degree * f); }
		Degree operator*	(const Degree& f) const { return Degree(m_Degree * f.m_Degree); }
		Degree& operator*=	(float f) { m_Degree *= f; return *this; }

		Degree operator/	(float f) const { return Degree(m_Degree / f); }
		Degree& operator/=	(float f) { m_Degree /= f; return *this; }

		bool operator<  (const Degree& d) const { return m_Degree <  d.m_Degree; }
		bool operator<= (const Degree& d) const { return m_Degree <= d.m_Degree; }
		bool operator== (const Degree& d) const { return m_Degree == d.m_Degree; }
		bool operator!= (const Degree& d) const { return m_Degree != d.m_Degree; }
		bool operator>= (const Degree& d) const { return m_Degree >= d.m_Degree; }
		bool operator>	(const Degree& d) const { return m_Degree >  d.m_Degree; }

		bool operator<  (const float& v) const { return m_Degree <  v; }
		bool operator<= (const float& v) const { return m_Degree <= v; }
		bool operator== (const float& v) const { return m_Degree == v; }
		bool operator!= (const float& v) const { return m_Degree != v; }
		bool operator>= (const float& v) const { return m_Degree >= v; }
		bool operator>  (const float& v) const { return m_Degree >  v; }

		friend Degree operator* (float lhs, const Degree& rhs)			{ return Degree(lhs * rhs.m_Degree); }
		friend Degree operator/ (float lhs, const Degree& rhs)			{ return Degree(lhs / rhs.m_Degree); }
		friend Degree operator+ (Degree& lhs, float rhs)				{ return Degree(lhs.m_Degree + rhs); }
		friend Degree operator+ (float lhs, const Degree& rhs)			{ return Degree(lhs + rhs.m_Degree); }
		friend Degree operator- (const Degree& lhs, float rhs)			{ return Degree(lhs.m_Degree - rhs); }
		friend Degree operator- (const float lhs, const Degree& rhs)	{ return Degree(lhs - rhs.m_Degree); }

		friend bool operator<  (float lhs, const Degree& rhs) { return lhs <  rhs.m_Degree; }
		friend bool operator<= (float lhs, const Degree& rhs) { return lhs <= rhs.m_Degree; }
		friend bool operator== (float lhs, const Degree& rhs) { return lhs == rhs.m_Degree; }
		friend bool operator!= (float lhs, const Degree& rhs) { return lhs != rhs.m_Degree; }
		friend bool operator>= (float lhs, const Degree& rhs) { return lhs >= rhs.m_Degree; }
		friend bool operator>  (float lhs, const Degree& rhs) { return lhs >  rhs.m_Degree; }
	};
}
