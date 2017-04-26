/********************************************************************/
/**
 * @file   geFloat16.h
 * @author Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date   2014/07/17
 * @brief  IEEE float 16 class
 *
 * Declaration of a class to use 16 bits floats with access to
 * elements represented by 10 mantissa bits, 5 exponent bits and
 * 1 sign bit.
 * 
 * E=0, M=0			== 0.0
 * E=0, M!=0		== Denormalized value (M/2^10)*2^-14
 * 0<E<31, M=any	== (1 + M / 2^10) * 2^(E-15)
 * E=32, M=0		== Infinite
 * E=31, M!=0		== NaN
 *
 * @bug	   No known bugs.
 */
 /********************************************************************/
#pragma once

/************************************************************************************************************************/
/* Includes																												*/
/************************************************************************************************************************/
#include "gePrerequisitesUtil.h"
#include "geMath.h"
#include "geFloat32.h"

namespace geEngineSDK
{
	/************************************************************************************************************************/
	/**
	*	@brief	Class to use 16 bits floats with elements access
	*	@note 10 mantissa bits, 5 exponent bits and 1 sign bit.
	*/
	/************************************************************************************************************************/
	class GE_UTILITY_EXPORT Float16
	{
	public:
		union
		{
			struct
			{
#if GE_ENDIAN == GE_ENDIAN_LITTLE
				uint16 Mantissa : 10;
				uint16 Exponent : 5;
				uint16 Sign : 1;
#else
				uint16 Sign : 1;
				uint16 Exponent : 5;
				uint16 Mantissa : 10;
#endif
			} Components;
			uint16 Encoded;
		};

		/** Default constructor */
		FORCEINLINE Float16() : Encoded(0)
		{
		
		}

		/** Copy constructor. */
		FORCEINLINE Float16(const Float16& FP16Value)
		{
			Encoded = FP16Value.Encoded;
		}

		/** Conversion constructor. Convert from Fp32 to Fp16. */
		FORCEINLINE Float16(float FP32Value)
		{
			Set(FP32Value);
		}

		/** Assignment operator. Convert from Fp32 to Fp16. */
		FORCEINLINE Float16& operator=(float FP32Value)
		{
			Set(FP32Value);
			return *this;
		}
		
		/** Assignment operator. Copy Fp16 value. */
		FORCEINLINE Float16& operator=(const Float16& FP16Value)
		{
			Encoded = FP16Value.Encoded;
			return *this;
		}

		/** Convert from Fp16 to Fp32. */
		FORCEINLINE operator float() const
		{
			return GetFloat();
		}
		
		/** Convert from Fp32 to Fp16. */
		FORCEINLINE void Set(float FP32Value)
		{
			Float32 FP32(FP32Value);

			Components.Sign = FP32.Components.Sign;	//Copy the sign bit

			//Check if this is a zero, de-normalized or too small value
			if(FP32.Components.Exponent <= 112)		//Exponent Too small? (0+127-15)
			{
				//Set it to 0
				Components.Exponent = 0;
				Components.Mantissa = 0;
			}
			else if(FP32.Components.Exponent >= 143)	//Exponent Too big? (31+127-15)
			{
				//Set it to 65504.0 (max value)
				Components.Exponent = 30;
				Components.Mantissa = 1023;
			}
			else
			{
				//Handle as normal number
				Components.Exponent = int32(FP32.Components.Exponent) - 127 + 15;
				Components.Mantissa = uint16(FP32.Components.Mantissa >> 13);
			}
		}

		/************************************************************************************************************************/
		/**
		* @brief	Convert from Fp32 to Fp16 without doing any checks if the Fp32 exponent is too large or too small. This is a
		*			faster alternative to Set() when you know the values within the single precision float don't need the checks.
		* @param	FP32Value Single precision float to be set as half precision.
		* @note		Make absolutely sure that you never pass in a single precision floating point value that may actually need
		*			the checks. If you are not 100% sure of that just use Set().
		*/
		/************************************************************************************************************************/
		FORCEINLINE void SetWithoutBoundsChecks(const float FP32Value)
		{
			const Float32 FP32(FP32Value);

			Components.Sign = FP32.Components.Sign;
			Components.Exponent = int32(FP32.Components.Exponent) - 127 + 15;
			Components.Mantissa = uint16(FP32.Components.Mantissa >> 13);
		}

		/** Convert from Fp16 to Fp32. */
		FORCEINLINE float GetFloat() const
		{
			Float32	Result;

			Result.Components.Sign = Components.Sign;
			if (Components.Exponent == 0)
			{
				uint32 Mantissa = Components.Mantissa;
				if (Mantissa == 0)
				{
					//Zero.
					Result.Components.Exponent = 0;
					Result.Components.Mantissa = 0;
				}
				else
				{
					//Denormal.
					uint32 MantissaShift = 10 - Math::Trunc(Math::Log2((float)Mantissa));
					Result.Components.Exponent = 127 - (15 - 1) - MantissaShift;
					Result.Components.Mantissa = Mantissa << (MantissaShift + 23 - 10);
				}
			}
			else if (Components.Exponent == 31)		// 2^5 - 1
			{
				//Infinity or NaN. Set to 65504.0
				Result.Components.Exponent = 142;
				Result.Components.Mantissa = 8380416;
			}
			else
			{
				//Normal number.
				Result.Components.Exponent = int32(Components.Exponent) - 15 + 127; // Stored exponents are biased by half their range.
				Result.Components.Mantissa = uint32(Components.Mantissa) << 13;
			}

			return Result.FloatValue;
		}
	};
}
