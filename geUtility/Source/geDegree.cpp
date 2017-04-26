/********************************************************************/
/**
 * @file   geDegree.cpp
 * @author Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date   2015/04/04
 * @brief  Wrapper class which indicates a given angle value is in
 *		  Degrees.
 *
 * Degree values are interchangeable with Radian values, and
 * conversions will be done automatically between them.
 *
 * @bug    No known bugs.
 */
/********************************************************************/

/************************************************************************************************************************/
/* Includes                                                                     										*/
/************************************************************************************************************************/
#include "geDegree.h"
#include "geMath.h"

namespace geEngineSDK
{
	Degree::Degree(const Radian& r) : m_Degree(r.ValueDegrees())
	{

	}

	Degree& Degree::operator= (const Radian& r)
	{
		m_Degree = r.ValueDegrees(); return *this;
	}

	Degree Degree::operator+ (const Radian& r) const
	{
		return Degree(m_Degree + r.ValueDegrees());
	}

	Degree& Degree::operator+= (const Radian& r)
	{
		m_Degree += r.ValueDegrees(); return *this;
	}

	Degree Degree::operator- (const Radian& r) const
	{
		return Degree(m_Degree - r.ValueDegrees());
	}

	Degree& Degree::operator-= (const Radian& r)
	{
		m_Degree -= r.ValueDegrees(); return *this;
	}

	inline float Degree::ValueRadians() const
	{
		return m_Degree * Math::DEG2RAD;
	}

	inline void Degree::UnwindDegrees()
	{
		m_Degree = Math::UnwindDegrees(m_Degree);
	}
}
