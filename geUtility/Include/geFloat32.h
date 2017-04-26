/********************************************************************/
/**
* @file   geFloat32.h
* @author Samuel Prince (samuel.prince.quezada@gmail.com)
* @date   2014/07/17
* @brief  32 bits float with access to elements
*
* 32 bits float with access to individual elements
* 23 bits Mantissa , 8 bits Exponent, 1 bit Sign
*
* @bug	   No known bugs.
*/
/********************************************************************/
#pragma once

/************************************************************************************************************************/
/* Includes																												*/
/************************************************************************************************************************/
#include "gePrerequisitesUtil.h"

namespace geEngineSDK
{
	/************************************************************************************************************************/
	/**
	* @brief	32 bit float components
	*/
	/************************************************************************************************************************/
	class GE_UTILITY_EXPORT Float32
	{
	public:

		union
		{
			struct
			{
#if GE_ENDIAN == GE_ENDIAN_LITTLE
				uint32	Mantissa : 23;
				uint32	Exponent : 8;
				uint32	Sign : 1;
#else
				uint32	Sign : 1;
				uint32	Exponent : 8;
				uint32	Mantissa : 23;
#endif
			} Components;
			float	FloatValue;
		};

		/************************************************************************************************************************/
		/**
		* @brief	Constructor
		* @param	InValue value of the float.
		* @return	What does this function returns.
		*/
		/************************************************************************************************************************/
		FORCEINLINE Float32(float InValue = 0.0f) : FloatValue(InValue)
		{

		}
	};


}