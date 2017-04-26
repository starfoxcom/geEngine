/********************************************************************/
/**
 * @file   geRadian.cpp
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

/************************************************************************************************************************/
/* Includes                                                                     										*/
/************************************************************************************************************************/
#include "geRadian.h"
#include "geMath.h"

namespace geEngineSDK
{
	Radian::Radian(const Degree& d) : m_Radian(d.ValueRadians())
	{

	}

	Radian& Radian::operator= (const Degree& d)
	{
		m_Radian = d.ValueRadians();
		return *this;
	}

	Radian Radian::operator+ (const Degree& d) const
	{
		return Radian(m_Radian + d.ValueRadians());
	}

	Radian& Radian::operator+= (const Degree& d)
	{
		m_Radian += d.ValueRadians();
		return *this;
	}

	Radian Radian::operator- (const Degree& d) const
	{
		return Radian(m_Radian - d.ValueRadians());
	}

	Radian& Radian::operator-= (const Degree& d)
	{
		m_Radian -= d.ValueRadians();
		return *this;
	}

	inline float Radian::ValueDegrees() const
	{
		return m_Radian * Math::RAD2DEG;
	}

	inline void Radian::UnwindRadians()
	{
		m_Radian = Math::UnwindRadians(m_Radian);
	}
}
