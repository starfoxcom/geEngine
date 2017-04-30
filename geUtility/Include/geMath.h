/********************************************************************/
/**
* @file   geMath.h
* @author Samuel Prince (samuel.prince.quezada@gmail.com)
* @date   2015/04/04
* @brief  Utility class providing common scalar math operations.
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
#include "geRadian.h"

namespace geEngineSDK
{
	/************************************************************************************************************************/
	/**
	* @brief	Abs specialization template (this is done here because of standard compliant)
	*/
	/************************************************************************************************************************/
	template <class T> FORCEINLINE T Abs(const T A)	{ return (A >= (T)0) ? A : -A; }
	template <> FORCEINLINE float Abs<float>(const float A)
	{
		return (float)std::fabs(A);
	}

	/************************************************************************************************************************/
	/**
	* @brief	Utility class providing common scalar math operations.
	*/
	/************************************************************************************************************************/
	class GE_UTILITY_EXPORT Math
	{
	public:
		static const float PI;
		static const float INV_PI;		
		static const float HALF_PI;
		static const float TWO_PI;

		static const float EULERS_NUMBER;

		static const uint8	MIN_UINT8;
		static const uint16 MIN_UINT16;
		static const uint32 MIN_UINT32;
		static const int8	MIN_INT8;
		static const int16	MIN_INT16;
		static const int32	MIN_INT32;
		static const float	MIN_FLOAT;

		static const uint8	MAX_UINT8;
		static const uint16 MAX_UINT16;
		static const uint32 MAX_UINT32;
		static const int8	MAX_INT8;
		static const int16	MAX_INT16;
		static const int32	MAX_INT32;
		static const float	MAX_FLOAT;

		static const float POS_INFINITY;
		static const float NEG_INFINITY;

		static const float SMALL_NUMBER;
		static const float FLOAT_SMALL_NUMBER;
		static const float KINDA_SMALL_NUMBER;
		static const float BIG_NUMBER;

		static const float DEG2RAD;
		static const float RAD2DEG;
		static const float LOG2;

		static const float DELTA;
		static const float FLOAT_EPSILON;
		static const double DOUBLE_EPSILON;

		/**
		* Lengths of normalized vectors (These are half their maximum values
		* to assure that dot products with normalized vectors don't overflow).
		*/
		static const float FLOAT_NORMAL_THRESH;

		//
		// Magic numbers for numerical precision.
		//
		static const float THRESH_POINT_ON_PLANE;			/** Thickness of plane for front/back/inside test */
		static const float THRESH_POINT_ON_SIDE;			/** Thickness of polygon side's side-plane for point-inside/outside/on side test */
		static const float THRESH_POINTS_ARE_SAME;			/** Two points are same if within this distance */
		static const float THRESH_POINTS_ARE_NEAR;			/** Two points are near if within this distance and can be combined if imprecise math is ok */
		static const float THRESH_NORMALS_ARE_SAME;			/** Two normal points are same if within this distance */
		
		/* Making this too large results in incorrect CSG classification and disaster */
		static const float THRESH_VECTORS_ARE_NEAR;			/** Two vectors are near if within this distance and can be combined if imprecise math is ok */
		
		/* Making this too large results in lighting problems due to inaccurate texture coordinates */
		static const float THRESH_SPLIT_POLY_WITH_PLANE;	/** A plane splits a polygon in half */
		static const float THRESH_SPLIT_POLY_PRECISELY;		/** A plane exactly splits a polygon */
		static const float THRESH_ZERO_NORM_SQUARED;		/** Size of a unit normal that is considered "zero", squared */
		static const float THRESH_NORMALS_ARE_PARALLEL;		/** Two unit vectors are parallel if abs(A dot B) is greater than or equal to this. This is roughly cosine(1.0 degrees). */
		static const float THRESH_NORMALS_ARE_ORTHOGONAL;	/** Two unit vectors are orthogonal (perpendicular) if abs(A dot B) is less than or equal this. This is roughly cosine(89.0 degrees). */

		static const float THRESH_VECTOR_NORMALIZED;		/** Allowed error for a normalized vector (against squared magnitude) */
		static const float THRESH_QUAT_NORMALIZED;			/** Allowed error for a normalized quaternion (against squared magnitude) */


		static FORCEINLINE int32 Trunc(float F) { return (int32)F; }
		static FORCEINLINE float TruncFloat(float F) { return (float)Trunc(F); }

		static FORCEINLINE int32 Floor(float F) { return Trunc((float)std::floor(F)); }
		static FORCEINLINE int32 Round(float F) { return Floor(F + 0.5f); }
		static FORCEINLINE int32 Ceil(float F)	{ return Trunc((float)std::ceil(F)); }

		static FORCEINLINE float Fractional(float Value) { return Value - TruncFloat(Value); }
		static FORCEINLINE float Fmod(float X, float Y) { return(float)std::fmod(X, Y); }
		
		static FORCEINLINE float Pow(float Base, float Exponent) { return (float)std::pow(Base, Exponent); }
		static FORCEINLINE float Exp(float Value)	{ return (float)std::exp(Value); }

		static FORCEINLINE float Loge(float Value)	{ return (float)std::log(Value); }
		static FORCEINLINE float Log2(float Value)	{ return (float)(std::log(Value) / LOG2); }
		static FORCEINLINE float LogX(float Base, float Value) { return Loge(Value) / Loge(Base); }
		
		static FORCEINLINE float  Sqrt(float Value)			{ return (float)std::sqrt(Value); }
		static FORCEINLINE Radian Sqrt(const Radian& Value) { return Radian(Sqrt(Value.ValueRadians())); }
		static FORCEINLINE Degree Sqrt(const Degree& Value) { return Degree(Sqrt(Value.ValueDegrees())); }

		static FORCEINLINE float InvSqrt(float F) { return 1.0f / (float)std::sqrt(F); }
		static FORCEINLINE float InvSqrtEst(float F){ return InvSqrt(F); }

		static FORCEINLINE bool IsNaN(float A) { return ((*(uint32*)&A) & 0x7FFFFFFF) > 0x7F800000; }
		static FORCEINLINE bool IsFinite(float A) { return ((*(uint32*)&A) & 0x7F800000) != 0x7F800000; }
		static FORCEINLINE bool IsNegativeFloat(const float& F1) { return ((*(uint32*)&F1) >= (uint32)0x80000000); }

		static FORCEINLINE float Cos(float Value) { return (float)std::cos(Value); }
		static FORCEINLINE float Sin(float Value) { return (float)std::sin(Value); }
		static FORCEINLINE float Tan(float Value) { return (float)std::tan(Value); }

		static FORCEINLINE float Cos(const Radian& Value) { return (float)std::cos(Value.ValueRadians()); }
		static FORCEINLINE float Sin(const Radian& Value) { return (float)std::sin(Value.ValueRadians()); }
		static FORCEINLINE float Tan(const Radian& Value) { return (float)std::tan(Value.ValueRadians()); }

		static Radian Acos(float Value);
		static Radian Asin(float Value);
		static FORCEINLINE Radian Atan(float Value) { return Radian(std::atan(Value)); }
		static FORCEINLINE Radian Atan2(float Y, float X) { return Radian(std::atan2(Y, X)); }

		/************************************************************************************************************************/
		/**
		 * @brief	Generic Lineal Interpolation function
		 * @param	x0 Starting value
		 * @param	x1 Ending value
		 * @param	Alpha Time coefficient in the range of [0..1]
		 * @return	Value between x0 and x1 related to alpha
		 */
		 /************************************************************************************************************************/
		template< class T, class U >
		static FORCEINLINE T Lerp(const T& A, const T& B, const U& Alpha)
		{
			return (T)(A + Alpha * (B - A));
		}

		/************************************************************************************************************************/
		/**
		* @brief	Divide two integers and rounds the result
		*/
		/************************************************************************************************************************/
		static FORCEINLINE uint32 DivideAndRoundUp(uint32 Dividend, uint32 Divisor)
		{
			return (Dividend + Divisor - 1) / Divisor;
		}

		/************************************************************************************************************************/
		/**
		* @brief	Returns the Square of a value
		*/
		/************************************************************************************************************************/
		template< class T >
		static FORCEINLINE T Square(const T A)
		{
			return A*A;
		}

		/************************************************************************************************************************/
		/**
		* @brief	Returns the lowest between two values
		*/
		/************************************************************************************************************************/
		template< class T >
		static FORCEINLINE T Min(const T A, const T B)
		{
			return (A <= B) ? A : B;
		}

		/************************************************************************************************************************/
		/**
		* @brief	Returns the highest between two values
		*/
		/************************************************************************************************************************/
		template< class T >
		static FORCEINLINE T Max(const T A, const T B)
		{
			return (A >= B) ? A : B;
		}

		/************************************************************************************************************************/
		/**
		* @brief	Returns the lowest between three values
		*/
		/************************************************************************************************************************/
		template< class T >
		static FORCEINLINE T Min3(const T A, const T B, const T C)
		{
			return Min(Min(A, B), C);
		}

		/************************************************************************************************************************/
		/**
		* @brief	Returns the highest between three values
		*/
		/************************************************************************************************************************/
		template< class T >
		static FORCEINLINE T Max3(const T A, const T B, const T C)
		{
			return Max(Max(A, B), C);
		}

		/************************************************************************************************************************/
		/**
		* @brief	Clamp a value within an inclusive range.
		*/
		/************************************************************************************************************************/
		template< class T >
		static FORCEINLINE T Clamp(const T X, const T Min, const T Max)
		{
			return X < Min ? Min : X < Max ? X : Max;
		}

		/************************************************************************************************************************/
		/**
		* @brief	Clamp a value within an inclusive range [0..1].
		*/
		/************************************************************************************************************************/
		template <typename T>
		static T Clamp01(T val)
		{
			return Max(Min(val, (T)1), (T)0);
		}

		/************************************************************************************************************************/
		/**
		* @brief	Returns the Absolute of a value
		*/
		/************************************************************************************************************************/
		template< class T >
		static FORCEINLINE T Abs(const T A)
		{
			return geEngineSDK::Abs<T>(A);
		}

		/************************************************************************************************************************/
		/**
		* @copydoc	Math::Abs
		* @note		Specialization for Degree data
		*/
		/************************************************************************************************************************/
		static FORCEINLINE Degree Abs(const Degree& Value) { return Degree(std::fabs(Value.ValueDegrees())); }

		/************************************************************************************************************************/
		/**
		* @copydoc	Math::Abs
		* @note		Specialization for Radian data
		*/
		/************************************************************************************************************************/
		static FORCEINLINE Radian Abs(const Radian& Value) { return Radian(std::fabs(Value.ValueRadians())); }

		/************************************************************************************************************************/
		/**
		* @brief	Returns the sign of the value (-1, 0, 1)
		*/
		/************************************************************************************************************************/
		template< class T >
		static FORCEINLINE T Sign(const T A)
		{
			return (A > (T)0) ? (T)1 : ((A < (T)0) ? (T)-1 : (T)0);
		}

		/************************************************************************************************************************/
		/**
		* @copydoc	Math::Sign
		* @note		Specialization for Degree values
		*/
		/************************************************************************************************************************/
		static FORCEINLINE Degree Sign(const Degree& val) { return Degree(Sign(val.ValueDegrees())); }

		/************************************************************************************************************************/
		/**
		* @copydoc	Math::Sign
		* @note		Specialization for Radian values
		*/
		/************************************************************************************************************************/
		static FORCEINLINE Radian Sign(const Radian& val) { return Radian(Sign(val.ValueRadians())); }

		/************************************************************************************************************************/
		/**
		* @brief	Returns the lowest natural logarithm of the value
		*/
		/************************************************************************************************************************/
		static FORCEINLINE uint32 FloorLog2(uint32 Value)
		{
			uint32 Bit = 32;
			for(; Bit > 0;)
			{
				Bit--;
				if (Value & (1 << Bit))
				{
					break;
				}
			}
			return Bit;
		}

		/************************************************************************************************************************/
		/**
		* @brief	Returns the number of zeros before there is a value on the variable (how many bits are unused)
		*/
		/************************************************************************************************************************/
		static FORCEINLINE uint32 CountLeadingZeros(uint32 Value)
		{
			if (Value == 0) return 32;
			return 31 - FloorLog2(Value);
		}

		/************************************************************************************************************************/
		/**
		* @brief	Returns the highest natural logarithm of the value
		*/
		/************************************************************************************************************************/
		static FORCEINLINE uint32 CeilLog2(uint32 Arg)
		{
			int32 Bitmask = ((int32)(CountLeadingZeros(Arg) << 26)) >> 31;
			return (32 - CountLeadingZeros(Arg - 1)) & (~Bitmask);
		}

		/************************************************************************************************************************/
		/**
		* @brief	Rounds a given value to the immediate superior Power of 2
		*/
		/************************************************************************************************************************/
		static FORCEINLINE uint32 RoundUpToPowerOfTwo(uint32 Arg)
		{
			return (uint32)(1 << CeilLog2(Arg));
		}

		/************************************************************************************************************************/
		/**
		* @brief	Checks if the value is between the range. (MaxValue Exclusive)
		*/
		/************************************************************************************************************************/
		template< class U >
		static FORCEINLINE bool IsWithin(const U& TestValue, const U& MinValue, const U& MaxValue)
		{
			return ((TestValue >= MinValue) && (TestValue < MaxValue));
		}

		/************************************************************************************************************************/
		/**
		* @brief	Checks if the value is between the range. (MaxValue Inclusive)
		*/
		/************************************************************************************************************************/
		template< class U >
		static FORCEINLINE bool IsWithinInclusive(const U& TestValue, const U& MinValue, const U& MaxValue)
		{
			return ((TestValue >= MinValue) && (TestValue <= MaxValue));
		}

		/************************************************************************************************************************/
		/**
		* @brief	Compare 2 floats, using tolerance for inaccuracies.
		* @param	A First comparing float
		* @param	B Second comparing float
		* @param	ErrorTolerance Max allowed difference to be considered Nearly Equal
		* @return	true if A and B are Nearly Equal
		*/
		/************************************************************************************************************************/
		static FORCEINLINE bool IsNearlyEqual(float A, float B, float ErrorTolerance = SMALL_NUMBER)
		{
			return Abs<float>(A - B) < ErrorTolerance;
		}

		/************************************************************************************************************************/
		/**
		* @brief	Compare 2 doubles, using tolerance for inaccuracies.
		* @param	A First comparing doubles
		* @param	B Second comparing doubles
		* @param	ErrorTolerance Max allowed difference to be considered Nearly Equal
		* @return	true if A and B are Nearly Equal
		*/
		/************************************************************************************************************************/
		static FORCEINLINE bool IsNearlyEqual(double A, double B, double ErrorTolerance = SMALL_NUMBER)
		{
			return Abs<double>(A - B) < ErrorTolerance;
		}

		/************************************************************************************************************************/
		/**
		* @brief	Compare if a value is Nearly Zero, using tolerance for inaccuracies.
		* @param	A Value to test
		* @param	ErrorTolerance Max allowed difference to be considered Zero
		* @return	true if A is Nearly Zero
		*/
		/************************************************************************************************************************/
		static FORCEINLINE bool IsNearlyZero(float Value, float ErrorTolerance = SMALL_NUMBER)
		{
			return Abs<float>(Value) < ErrorTolerance;
		}

		/************************************************************************************************************************/
		/**
		* @brief	Compare if a value is Nearly Zero, using tolerance for inaccuracies.
		* @param	A Value to test
		* @param	ErrorTolerance Max allowed difference to be considered Zero
		* @return	true if A is Nearly Zero
		*/
		/************************************************************************************************************************/
		static FORCEINLINE bool IsNearlyZero(double Value, double ErrorTolerance = SMALL_NUMBER)
		{
			return Abs<double>(Value) < ErrorTolerance;
		}

		/************************************************************************************************************************/
		/**
		* @brief	Checks is the specified value a power of two. Only works on integer values.
		*/
		/************************************************************************************************************************/
		static FORCEINLINE bool IsPowerOfTwo(uint32 Value)
		{
			return ((Value & (Value - 1)) == 0);
		}

		/************************************************************************************************************************/
		/**
		* @brief	Returns the closest Grid multiply to the Location
		*/
		/************************************************************************************************************************/
		static FORCEINLINE float GridSnap(float Location, float Grid)
		{
			if (Grid == 0.f) return Location;
			else
			{
				return Floor((Location + 0.5f*Grid) / Grid)*Grid;
			}
		}

		/************************************************************************************************************************/
		/**
		* @brief	Compares the value of Comparand and return the "Greater" or "Lower" value accordingly
		*/
		/************************************************************************************************************************/
		static FORCEINLINE float FloatSelect(float Comparand, float ValueGEZero, float ValueLTZero)
		{
			return Comparand >= 0.f ? ValueGEZero : ValueLTZero;
		}

		/************************************************************************************************************************/
		/**
		* @copydoc	Math::FloatSelect
		*/
		/************************************************************************************************************************/
		static FORCEINLINE double FloatSelect(double Comparand, double ValueGEZero, double ValueLTZero)
		{
			return Comparand >= 0.f ? ValueGEZero : ValueLTZero;
		}

		/************************************************************************************************************************/
		/**
		* @brief	Calculates the tangent space vector for a given set of positions / texture coords.
		*/
		/************************************************************************************************************************/
		static Vector3 CalculateTriTangent(const Vector3& position1, const Vector3& position2, const Vector3& position3, float u1, float v1, float u2, float v2, float u3, float v3);


		/************************************************************************************************************************/
		/**
		* @brief	Sine function approximation.
		* @param	Value	Angle in range [0, pi/2].
		* @note		Evaluates trigonometric functions using polynomial approximations.
		*/
		/************************************************************************************************************************/
		static float FastSin0(const Radian& Value) { return (float)FastASin0(Value.ValueRadians()); }

		/************************************************************************************************************************/
		/**
		* @brief	Sine function approximation.
		* @param	Value	Angle in range [0, pi/2].
		* @note		Evaluates trigonometric functions using polynomial approximations.
		*/
		/************************************************************************************************************************/
		static float FastSin0(float Value);

		/************************************************************************************************************************/
		/**
		* @brief	Sine function approximation.
		* @param	Value	Angle in range [0, pi/2].
		* @note		Evaluates trigonometric functions using polynomial approximations. Slightly better (and slower) than "FastSin0".
		*/
		/************************************************************************************************************************/
		static float FastSin1(const Radian& Value) { return (float)FastASin1(Value.ValueRadians()); }

		/************************************************************************************************************************/
		/**
		* @brief	Sine function approximation.
		* @param	Value	Angle in range [0, pi/2].
		* @note		Evaluates trigonometric functions using polynomial approximations. Slightly better (and slower) than "FastSin0".
		*/
		/************************************************************************************************************************/
		static float FastSin1(float Value);

		/************************************************************************************************************************/
		/**
		* @brief	Cosine function approximation.
		* @param	Value	Angle in range [0, pi/2].
		* @note		Evaluates trigonometric functions using polynomial approximations.
		*/
		/************************************************************************************************************************/
		static float FastCos0(const Radian& Value) { return (float)FastACos0(Value.ValueRadians()); }

		/************************************************************************************************************************/
		/**
		* @brief	Cosine function approximation.
		* @param	Value	Angle in range [0, pi/2].
		* @note		Evaluates trigonometric functions using polynomial approximations.
		*/
		/************************************************************************************************************************/
		static float FastCos0(float Value);

		/************************************************************************************************************************/
		/**
		* @brief	Cosine function approximation.
		* @param	Value	Angle in range [0, pi/2].
		* @note		Evaluates trigonometric functions using polynomial approximations. Slightly better (and slower) than "FastCos0".
		*/
		/************************************************************************************************************************/
		static float FastCos1(const Radian& Value) { return (float)FastACos1(Value.ValueRadians()); }

		/************************************************************************************************************************/
		/**
		* @brief	Cosine function approximation.
		* @param	Value	Angle in range [0, pi/2].
		* @note		Evaluates trigonometric functions using polynomial approximations. Slightly better (and slower) than "FastCos0".
		*/
		/************************************************************************************************************************/
		static float FastCos1(float Value);

		/************************************************************************************************************************/
		/**
		* @brief	Tangent function approximation.
		* @param	Value	Angle in range [0, pi/4].
		* @note		Evaluates trigonometric functions using polynomial approximations.
		*/
		/************************************************************************************************************************/
		static float FastTan0(const Radian& Value) { return (float)FastATan0(Value.ValueRadians()); }

		/************************************************************************************************************************/
		/**
		* @brief	Tangent function approximation.
		* @param	Value	Angle in range [0, pi/4].
		* @note		Evaluates trigonometric functions using polynomial approximations.
		*/
		/************************************************************************************************************************/
		static float FastTan0(float Value);

		/************************************************************************************************************************/
		/**
		* @brief	Tangent function approximation.
		* @param	val	Angle in range [0, pi/4].
		* @note		Evaluates trigonometric functions using polynomial approximations. Slightly better (and slower) than "FastTan0".
		*/
		/************************************************************************************************************************/
		static float FastTan1(const Radian& Value) { return (float)FastATan1(Value.ValueRadians()); }

		/************************************************************************************************************************/
		/**
		* @brief	Tangent function approximation.
		* @param	Value	Angle in range [0, pi/4].
		* @note		Evaluates trigonometric functions using polynomial approximations. Slightly better (and slower) than "FastTan0".
		*/
		/************************************************************************************************************************/
		static float FastTan1(float Value);

		/************************************************************************************************************************/
		/**
		* @brief	Inverse sine function approximation.
		* @param	Value	Angle in range [0, 1].
		* @note		Evaluates trigonometric functions using polynomial approximations.
		*/
		/************************************************************************************************************************/
		static float FastASin0(const Radian& Value) { return (float)FastASin0(Value.ValueRadians()); }

		/************************************************************************************************************************/
		/**
		* @brief	Inverse sine function approximation.
		* @param	Value	Angle in range [0, 1].
		* @note		Evaluates trigonometric functions using polynomial approximations.
		*/
		/************************************************************************************************************************/
		static float FastASin0(float Value);

		/************************************************************************************************************************/
		/**
		* @brief	Inverse sine function approximation.
		* @param	Value	Angle in range [0, 1].
		* @note		Evaluates trigonometric functions using polynomial approximations. Slightly better (and slower) than "FastASin0".
		*/
		/************************************************************************************************************************/
		static float FastASin1(const Radian& Value) { return (float)FastASin1(Value.ValueRadians()); }

		/************************************************************************************************************************/
		/**
		* @brief	Inverse sine function approximation.
		* @param	Value	Angle in range [0, 1].
		* @note		Evaluates trigonometric functions using polynomial approximations. Slightly better (and slower) than "FastASin0".
		*/
		/************************************************************************************************************************/
		static float FastASin1(float Value);

		/************************************************************************************************************************/
		/**
		* @brief	Inverse cosine function approximation.
		* @param	Value	Angle in range [0, 1].
		* @note		Evaluates trigonometric functions using polynomial approximations.
		*/
		/************************************************************************************************************************/
		static float FastACos0(const Radian& Value) { return (float)FastACos0(Value.ValueRadians()); }

		/************************************************************************************************************************/
		/**
		* @brief	Inverse cosine function approximation.
		* @param	Value	Angle in range [0, 1].
		* @note		Evaluates trigonometric functions using polynomial approximations.
		*/
		/************************************************************************************************************************/
		static float FastACos0(float Value);

		/************************************************************************************************************************/
		/**
		* @brief	Inverse cosine function approximation.
		* @param	Value	Angle in range [0, 1].
		* @note		Evaluates trigonometric functions using polynomial approximations. Slightly better (and slower) than "FastACos0".
		*/
		/************************************************************************************************************************/
		static float FastACos1(const Radian& Value) { return (float)FastACos1(Value.ValueRadians()); }

		/************************************************************************************************************************/
		/**
		* @brief	Inverse cosine function approximation.
		* @param	Value	Angle in range [0, 1].
		* @note		Evaluates trigonometric functions using polynomial approximations. Slightly better (and slower) than "FastACos0".
		*/
		/************************************************************************************************************************/
		static float FastACos1(float Value);

		/************************************************************************************************************************/
		/**
		* @brief	Inverse tangent function approximation.
		* @param	Value	Angle in range [-1, 1].
		* @note		Evaluates trigonometric functions using polynomial approximations.
		*/
		/************************************************************************************************************************/
		static float FastATan0(const Radian& Value) { return (float)FastATan0(Value.ValueRadians()); }

		/************************************************************************************************************************/
		/**
		* @brief	Inverse tangent function approximation.
		* @param	Value	Angle in range [-1, 1].
		* @note		Evaluates trigonometric functions using polynomial approximations.
		*/
		/************************************************************************************************************************/
		static float FastATan0(float Value);

		/************************************************************************************************************************/
		/**
		* @brief	Inverse tangent function approximation.
		* @param	Value	Angle in range [-1, 1].
		* @note		Evaluates trigonometric functions using polynomial approximations. Slightly better (and slower) than "FastATan0".
		*/
		/************************************************************************************************************************/
		static float FastATan1(const Radian& Value) { return (float)FastATan1(Value.ValueRadians()); }

		/************************************************************************************************************************/
		/**
		* @brief	Inverse tangent function approximation.
		* @param	Value	Angle in range [-1, 1].
		* @note		Evaluates trigonometric functions using polynomial approximations. Slightly better (and slower) than "FastATan0".
		*/
		/************************************************************************************************************************/
		static float FastATan1(float Value);

		/************************************************************************************************************************/
		/**
		* @brief	Utility to ensure angle is between +/- 180 degrees by unwinding.
		*/
		/************************************************************************************************************************/
		static float UnwindDegrees(float A)
		{
			while (A > 180.f)
			{
				A -= 360.f;
			}

			while (A < -180.f)
			{
				A += 360.f;
			}

			return A;
		}

		/************************************************************************************************************************/
		/**
		* @brief	Given a heading which may be outside the +/- PI range, 'unwind' it back into that range.
		*/
		/************************************************************************************************************************/
		static float UnwindRadians(float A)
		{
			while (A > PI)
			{
				A -= TWO_PI;
			}

			while (A < -PI)
			{
				A += TWO_PI;
			}

			return A;
		}

		/************************************************************************************************************************/
		/**
		* @brief	Solves the linear equation with the parameters A, B. Returns number of roots found and the roots themselves
		*			will be output in the @p roots array.
		* @param[out]	roots	Must be at least size of 1.
		* @note		Only returns real roots.
		*/
		/************************************************************************************************************************/
		template <typename T>
		static uint32 SolveLinear(T A, T B, T* roots)
		{
			if( !IsNearlyEqual(A, (T)0) )
			{
				roots[0] = -B / A;
				return 1;
			}

			roots[0] = 0.0f;
			return 1;
		}

		/************************************************************************************************************************/
		/**
		* @brief	Solves the quadratic equation with the parameters A, B, C. Returns number of roots found and the roots
		*			themselves will be output in the @p roots array.
		* @param[out]	roots	Must be at least size of 2.
		* @note		Only returns real roots.
		*/
		/************************************************************************************************************************/
		template <typename T>
		static uint32 SolveQuadratic(T A, T B, T C, T* roots)
		{
			if( !IsNearlyEqual(A, (T)0) )
			{
				T p = B / (2 * A);
				T q = C / A;
				T D = p * p - q;

				if( !IsNearlyEqual(D, (T)0) )
				{
					if( D < (T)0 )
					{
						return 0;
					}

					T sqrtD = Sqrt(D);
					roots[0] = sqrtD - p;
					roots[1] = -sqrtD - p;
					return 2;
				}
				else
				{
					roots[0] = -p;
					roots[1] = -p;
					return 1;
				}
			}
			else
			{
				return SolveLinear(B, C, roots);
			}
		}

		/************************************************************************************************************************/
		/**
		* @brief	Solves the cubic equation with the parameters A, B, C, D. Returns number of roots found and the roots
		*			themselves will be output in the @p roots array.
		* @param[out]	roots	Must be at least size of 3.
		* @note		Only returns real roots.
		*/
		/************************************************************************************************************************/
		template <typename T>
		static uint32 SolveCubic(T A, T B, T C, T D, T* roots)
		{
			static const T THIRD = (1 / (T)3);

			T invA = 1 / A;
			A = B * invA;
			B = C * invA;
			C = D * invA;

			T sqA = Square(A);
			T p = THIRD * (-THIRD * sqA + B);
			T q = ((T)0.5) * ((2 / (T)27) * A * sqA - THIRD * A * B + C);

			T cbp = p * p * p;
			D = q * q + cbp;

			uint32 numRoots = 0;
			if( !IsNearlyEqual(D, (T)0) )
			{
				if( D < 0.0 )
				{
					T phi = THIRD * Acos(-q / Sqrt(-cbp));
					T t = 2 * Sqrt(-p);

					roots[0] = t * Cos(phi);
					roots[1] = -t * Cos(phi + PI * THIRD);
					roots[2] = -t * Cos(phi - PI * THIRD);

					numRoots = 3;
				}
				else
				{
					T sqrtD = Sqrt(D);
					T u = cbrt(sqrtD + Abs(q));

					if( q > (T)0 )
					{
						roots[0] = -u + p / u;
					}
					else
					{
						roots[0] = u - p / u;
					}

					numRoots = 1;
				}
			}
			else
			{
				if( !IsNearlyEqual(q, (T)0) )
				{
					T u = cbrt(-q);
					roots[0] = 2 * u;
					roots[1] = -u;
					numRoots = 2;
				}
				else
				{
					roots[0] = 0.0f;
					numRoots = 1;
				}
			}

			T sub = THIRD * A;
			for( uint32 i=0; i<numRoots; ++i )
			{
				roots[i] -= sub;
			}

			return numRoots;
		}

		/************************************************************************************************************************/
		/**
		* @brief	Solves the quartic equation with the parameters A, B, C, D, E. Returns number of roots found and the roots
		*			themselves will be output in the @p roots array.
		* @param[out]	roots	Must be at least size of 4.
		* @note		Only returns real roots.
		*/
		/************************************************************************************************************************/
		template <typename T>
		static uint32 SolveQuartic(T A, T B, T C, T D, T E, T* roots)
		{
			T invA = 1 / A;
			A = B * invA;
			B = C * invA;
			C = D * invA;
			D = E * invA;

			T sqA = Square(A);
			T p = -(3 / (T)8) * sqA + B;
			T q = (1 / (T)8) * sqA * A - (T)0.5 * A * B + C;
			T r = -(3 / (T)256) * sqA * sqA + (1 / (T)16) * sqA * B - (1 / (T)4) * A * C + D;

			uint32 numRoots = 0;
			if( !IsNearlyEqual(r, (T)0) )
			{
				T cubicA = 1;
				T cubicB = -(T)0.5 * p;
				T cubicC = -r;
				T cubicD = (T)0.5 * r * p - (1 / (T)8) * q * q;

				SolveCubic(cubicA, cubicB, cubicC, cubicD, roots);
				T z = roots[0];

				T u = z * z - r;
				T v = 2 * z - p;

				if( IsNearlyEqual(u, T(0)) )
				{
					u = 0;
				}
				else if(u > 0)
				{
					u = Sqrt(u);
				}
				else
				{
					return 0;
				}

				if( IsNearlyEqual(v, T(0)) )
				{
					v = 0;
				}
				else if (v > 0)
				{
					v = Sqrt(v);
				}
				else
				{
					return 0;
				}

				T quadraticA = 1;
				T quadraticB = q < 0 ? -v : v;
				T quadraticC = z - u;

				numRoots = SolveQuadratic(quadraticA, quadraticB, quadraticC, roots);

				quadraticA = 1;
				quadraticB = q < 0 ? v : -v;
				quadraticC = z + u;

				numRoots += SolveQuadratic(quadraticA, quadraticB, quadraticC, roots + numRoots);
			}
			else
			{
				numRoots = SolveCubic(q, p, (T)0, (T)1, roots);
				roots[numRoots++] = 0;
			}

			T sub = (1 / (T)4) * A;
			for( uint32 i=0; i<numRoots; ++i )
			{
				roots[i] -= sub;
			}

			return numRoots;
		}

		/************************************************************************************************************************/
		/**
		* @brief	Find the intersection of a line and an offset plane. Assumes that the line and plane do indeed intersect;
		*			you must make sure they're not parallel before calling.
		* @param	Point1 the first point defining the line
		* @param	Point2 the second point defining the line
		* @param	PlaneOrigin the origin of the plane
		* @param	PlaneNormal the normal of the plane
		* @return	The point of intersection between the line and the plane.
		*/
		/************************************************************************************************************************/
		static Vector3 LinePlaneIntersection(const Vector3& Point1, const Vector3& Point2, const Vector3& PlaneOrigin, const Vector3& PlaneNormal);

		/************************************************************************************************************************/
		/**
		* @brief	Find the intersection of a line and a plane. Assumes that the line and plane do indeed intersect; you must
		*			make sure they're not parallel before calling.
		* @param	Point1 the first point defining the line
		* @param	Point2 the second point defining the line
		* @param	plane the Plane
		* @return	The point of intersection between the line and the plane.
		*/
		/************************************************************************************************************************/
		static Vector3 LinePlaneIntersection(const Vector3& Point1, const Vector3& Point2, const Plane& plane);

		/************************************************************************************************************************/
		/**
		* @brief	Determine if a plane and an AABB intersect
		* @param	P - the plane to test
		* @param	AABB - the axis aligned bounding box to test
		* @return	if collision occurs
		*/
		/************************************************************************************************************************/
		static bool PlaneAABBIntersection(const Plane& InP, const AABox& AABB);

		/************************************************************************************************************************/
		/**
		* @brief	Checks if the Point intersects with the Box
		* @param	Point the point
		* @param	Box the Box to test against
		* @return	true if the point intersects with the box false otherwise
		*/
		/************************************************************************************************************************/
		static bool PointBoxIntersection(const Vector3& Point, const AABox& Box);

		/************************************************************************************************************************/
		/**
		* @brief	Checks if a line intersects with a box
		* @param	Box the Box to test against
		* @param	Start the starting position of the line
		* @param	End the end position of the line
		* @param	Direction Direction of the testing vector
		* @return	true if the line intersects with the box false otherwise
		*/
		/************************************************************************************************************************/
		static bool LineBoxIntersection(const AABox& Box, const Vector3& Start, const Vector3& End, const Vector3& Direction);

		/************************************************************************************************************************/
		/**
		* @brief	Checks if a line intersects with a box
		* @param	Box the Box to test against
		* @param	Start the starting position of the line
		* @param	End the end position of the line
		* @param	Direction Direction of the testing vector
		* @param	OneOverDirection Reciprocal direction of the testing
		* @return	true if the line intersects with the box false otherwise
		*/
		/************************************************************************************************************************/
		static bool LineBoxIntersection(const AABox& Box, const Vector3& Start, const Vector3& End, const Vector3& Direction, const Vector3& OneOverDirection);

		static bool LineSphereIntersection(const Vector3& Start, const Vector3& Dir, float Length, const Vector3& Origin, float Radius);
		
		static bool IntersectPlanes2(Vector3& I, Vector3& D, const Plane& P1, const Plane& P2);
		static bool IntersectPlanes3(Vector3& I, const Plane& P1, const Plane& P2, const Plane& P3);

		FORCEINLINE float GetRangePct(Vector2 const& Range, float Value);
		FORCEINLINE float GetRangeValue(Vector2 const& Range, float Pct);

		static bool SphereAABBIntersection(const Vector3& SphereCenter, const float RadiusSquared, const AABox& AABB);
		static bool SphereAABBIntersection(const Sphere& InSphere, const AABox& AABB);
	};
}
