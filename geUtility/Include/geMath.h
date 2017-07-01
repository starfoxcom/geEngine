/*****************************************************************************/
/**
 * @file    geMath.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2015/04/04
 * @brief   Utility class providing common scalar math operations.
 *
 * @bug	    No known bugs.
 */
/*****************************************************************************/
#pragma once

/*****************************************************************************/
/**
 * Includes
 */
/*****************************************************************************/
#include "gePrerequisitesUtil.h"
#include "geDegree.h"
#include "geRadian.h"

namespace geEngineSDK {
  /**
   * @brief abs specialization template
   *        (this is done here because of standard compliant)
   */
  template<class T>
  FORCEINLINE T
  abs(const T A) {
    return (A >= (T)0) ? A : -A;
  }

  template<>
  FORCEINLINE float
  abs<float>(const float A) {
    return static_cast<float>(std::fabsf(A));
  }

  /**
   * @brief Utility class providing common scalar math operations.
   */
  class GE_UTILITY_EXPORT Math
  {
   public:
    static FORCEINLINE int32
    trunc(float F) {
      return static_cast<int32>(F);
    }

    static FORCEINLINE float
    truncFloat(float F) {
      return static_cast<float>(trunc(F));
    }

    static FORCEINLINE int32
    floor(float F) {
      return trunc(std::floorf(F));
    }

    static FORCEINLINE float
    floorFloat(float F) {
      return std::floorf(F);
    }

    static FORCEINLINE double
    floorDouble(double F) {
      return std::floor(F);
    }

    static FORCEINLINE int32
    round(float F) {
      return floor(F + 0.5f);
    }

    static FORCEINLINE float
    roundFloat(float F) {
      return floorFloat(F + 0.5f);
    }

    static FORCEINLINE double
    roundDouble(double F) {
      return floorDouble(F + 0.5);
    }
    
    static FORCEINLINE int32
    ceil(float F) {
      return trunc(std::ceilf(F));
    }

    static FORCEINLINE float
    ceilFloat(float F) {
      return std::ceilf(F);
    }

    static FORCEINLINE double
    ceilDouble(double F) {
      return std::ceil(F);
    }

    static FORCEINLINE float
    fractional(float Value) {
      return Value - truncFloat(Value);
    }

    static FORCEINLINE float
    fmod(float X, float Y) {
      return std::fmodf(X, Y);
    }

    static FORCEINLINE float
    pow(float Base, float Exponent) {
      return std::powf(Base, Exponent);
    }

    static FORCEINLINE float
    exp(float Value) {
      return std::expf(Value);
    }

    static FORCEINLINE float
    logE(float Value) {
      return std::logf(Value);
    }
    
    static FORCEINLINE float
    log2(float Value) {
      return std::logf(Value) / LOG2;
    }

    static FORCEINLINE float
    logX(float Base, float Value) {
      return logE(Value) / logE(Base);
    }

    static FORCEINLINE float
    sqrt(float Value) {
      return std::sqrtf(Value);
    }

    static FORCEINLINE Radian
    sqrt(const Radian& Value) {
      return Radian(sqrt(Value.valueRadians()));
    }

    static FORCEINLINE Degree
    sqrt(const Degree& Value) {
      return Degree(sqrt(Value.valueDegrees()));
    }

    static FORCEINLINE float
    invSqrt(float F) {
      return 1.0f / std::sqrtf(F);
    }

    static FORCEINLINE float
    invSqrtEst(float F) {
      return invSqrt(F);
    }

    static FORCEINLINE bool
    isNaN(float A) {
      return ((*reinterpret_cast<uint32*>(&A)) & 0x7FFFFFFF) > 0x7F800000;
    }

    static FORCEINLINE bool
    isFinite(float A) {
      return ((*reinterpret_cast<uint32*>(&A)) & 0x7F800000) != 0x7F800000;
    }

    static FORCEINLINE bool
    isNegativeFloat(const float& F1) {
      return ((*(uint32*)&F1) >= static_cast<uint32>(0x80000000));
    }

    static FORCEINLINE bool
    isNegativeDouble(const double& A) {
      return ((*(uint64*)&A) >= (uint64)0x8000000000000000);
    }

    static FORCEINLINE float
    cos(float Value) {
      return std::cosf(Value);
    }

    static FORCEINLINE float
    sin(float Value) {
      return std::sin(Value);
    }

    static FORCEINLINE float
    tan(float Value) {
      return std::tanf(Value);
    }

    static FORCEINLINE float
    cos(const Radian& Value) {
      return std::cosf(Value.valueRadians());
    }

    static FORCEINLINE float
    sin(const Radian& Value) {
      return std::sinf(Value.valueRadians());
    }

    static FORCEINLINE float
    tan(const Radian& Value) {
      return std::tan(Value.valueRadians());
    }

    static Radian
    acos(float Value);

    static Radian
    asin(float Value);

    static FORCEINLINE Radian
    atan(float Value) {
      return Radian(std::atanf(Value));
    }

    static FORCEINLINE Radian
    atan2(float Y, float X) {
      return Radian(std::atan2f(Y, X));
    }

    /**
     * @brief Generic Lineal Interpolation function
     * @param x0 Starting value
     * @param x1 Ending value
     * @param Alpha Time coefficient in the range of [0..1]
     * @return Value between x0 and x1 related to alpha
     */
    template<class T, class U>
    static FORCEINLINE T
    lerp(const T& A, const T& B, const U& Alpha) {
      return static_cast<T>(A + Alpha * (B - A));
    }

    /**
     * @brief Divide two integers and rounds the result
     */
    template<class T>
    static FORCEINLINE T
    divideAndRoundUp(T Dividend, T Divisor) {
      return (Dividend + Divisor - 1) / Divisor;
    }

    /**
     * @brief Divide two integers and rounds the result
     */
    template <class T>
    static FORCEINLINE T
    divideAndRoundDown(T Dividend, T Divisor) {
      return Dividend / Divisor;
    }

    /**
     * @brief Returns the Square of a value
     */
    template<class T>
    static FORCEINLINE T
    square(const T A) {
      return A*A;
    }

    /**
     * @brief Returns the lowest between two values
     */
    template<class T>
    static FORCEINLINE T
    min(const T A, const T B) {
      return (A <= B) ? A : B;
    }

    /**
     * @brief Returns the highest between two values
     */
    template<class T>
    static FORCEINLINE T
    max(const T A, const T B) {
      return (A >= B) ? A : B;
    }

    /**
     * @brief Returns the lowest between three values
     */
    template<class T>
    static FORCEINLINE T
    min3(const T A, const T B, const T C) {
      return min(min(A, B), C);
    }

    /**
     * @brief Returns the highest between three values
     */
    template<class T>
    static FORCEINLINE T
    max3(const T A, const T B, const T C) {
      return max(max(A, B), C);
    }

    /**
     * @brief Clamp a value within an inclusive range.
     */
    template<class T>
    static FORCEINLINE T
    clamp(const T X, const T Min, const T Max) {
      return X < Min ? Min : X < Max ? X : Max;
    }

    /**
     * @brief Clamp a value within an inclusive range [0..1].
     */
    template<typename T>
    static T
    clamp01(T val) {
      return max(min(val, static_cast<T>(1)), static_cast<T>(0));
    }

    /**
     * @brief Returns the Absolute of a value
     */
    template<class T>
    static FORCEINLINE T
    abs(const T A) {
      return geEngineSDK::abs<T>(A);
    }

    /**
     * @copydoc Math::abs
     * @note    Specialization for Degree data
     */
    static FORCEINLINE Degree
    abs(const Degree& Value) {
      return Degree(std::fabs(Value.valueDegrees()));
    }

    /**
     * @copydoc Math::Abs
     * @note    Specialization for Radian data
     */
    static FORCEINLINE Radian
    abs(const Radian& Value) {
      return Radian(std::fabs(Value.valueRadians()));
    }

    /**
     * @brief Returns the sign of the value (-1, 0, 1)
     */
    template<class T>
    static FORCEINLINE T
    sign(const T A) {
      return (A > static_cast<T>(0)) ? static_cast<T>(1)
          : ((A < static_cast<T>(0)) ? static_cast<T>(-1) : static_cast<T>(0));
    }

    /**
     * @copydoc Math::sign
     * @note    Specialization for Degree values
     */
    static FORCEINLINE Degree
    sign(const Degree& val) {
      return Degree(sign(val.valueDegrees()));
    }

    /**
     * @copydoc Math::sign
     * @note    Specialization for Radian values
     */
    static FORCEINLINE Radian
    sign(const Radian& val) {
      return Radian(sign(val.valueRadians()));
    }

    /**
     * @brief Computes the base 2 logarithm for an integer value that is greater than 0.
     * The result is rounded down to the nearest integer.
     * @param Value The value to compute the log of
     * @return Log2 of Value. 0 if Value is 0.
     */
    static uint32
    floorLog2(uint32 Value);

    static FORCEINLINE uint64
    floorLog2_64(uint64 Value) {
      uint64 pos = 0;
      if (Value >= 1ull << 32) { Value >>= 32; pos += 32; }
      if (Value >= 1ull << 16) { Value >>= 16; pos += 16; }
      if (Value >= 1ull << 8) { Value >>= 8; pos += 8; }
      if (Value >= 1ull << 4) { Value >>= 4; pos += 4; }
      if (Value >= 1ull << 2) { Value >>= 2; pos += 2; }
      if (Value >= 1ull << 1) { pos += 1; }
      return (Value == 0) ? 0 : pos;
    }

    /**
     * @brief Returns the number of zeros before there is a value on the
     *        variable (how many bits are unused)
     */
    static uint32
    countLeadingZeros(uint32 Value);

    /**
    * @copydoc Math::countLeadingZeros(uint32)
    */
    static FORCEINLINE uint64
    countLeadingZeros64(uint64 Value) {
      if (Value == 0) return 64;
      return 63 - floorLog2_64(Value);
    }

    static uint32
    countTrailingZeros(uint32 Value);

    /**
     * @brief Returns the highest natural logarithm of the value
     */
    static FORCEINLINE uint32
    ceilLog2(uint32 Arg) {
      int32 Bitmask = (static_cast<int32>(countLeadingZeros(Arg) << 26)) >> 31;
      return (32 - countLeadingZeros(Arg - 1)) & (~Bitmask);
    }

    static FORCEINLINE uint64
    ceilLog2_64(uint64 Arg) {
      int64 Bitmask = ((int64)(countLeadingZeros64(Arg) << 57)) >> 63;
      return (64 - countLeadingZeros64(Arg - 1)) & (~Bitmask);
    }

    /**
     * @brief Rounds a given value to the immediate superior Power of 2
     */
    static FORCEINLINE uint32
    roundUpToPowerOfTwo(uint32 Arg) {
      return static_cast<uint32>(1 << ceilLog2(Arg));
    }

    /**
     * @brief Checks if the value is between the range. (MaxValue Exclusive)
     */
    template<class U>
    static FORCEINLINE bool
    isWithin(const U& TestValue, const U& MinValue, const U& MaxValue) {
      return ((TestValue >= MinValue) && (TestValue < MaxValue));
    }

    /**
     * @brief Checks if the value is between the range. (MaxValue Inclusive)
     */
    template<class U>
    static FORCEINLINE bool
    isWithinInclusive(const U& TestValue, const U& MinValue, const U& MaxValue) {
      return ((TestValue >= MinValue) && (TestValue <= MaxValue));
    }

    /**
     * @brief Compare 2 floats, using tolerance for inaccuracies.
     * @param A First comparing float
     * @param B Second comparing float
     * @param ErrorTolerance Max allowed difference to be considered Nearly Equal
     * @return true if A and B are Nearly Equal
     */
    static FORCEINLINE bool
    isNearlyEqual(float A, float B, float ErrorTolerance = SMALL_NUMBER) {
      return abs<float>(A - B) < ErrorTolerance;
    }

    /**
     * @brief Compare 2 doubles, using tolerance for inaccuracies.
     * @param A First comparing doubles
     * @param B Second comparing doubles
     * @param ErrorTolerance Max allowed difference to be considered Nearly Equal
     * @return true if A and B are Nearly Equal
     */
    static FORCEINLINE bool
    isNearlyEqual(double A, double B, double ErrorTolerance = SMALL_NUMBER) {
      return abs<double>(A - B) < ErrorTolerance;
    }

    /**
     * @brief	Compare if a value is Nearly Zero, using tolerance for inaccuracies.
     * @param	A Value to test
     * @param	ErrorTolerance Max allowed difference to be considered Zero
     * @return	true if A is Nearly Zero
     */
    static FORCEINLINE bool
    isNearlyZero(float Value, float ErrorTolerance = SMALL_NUMBER) {
      return abs<float>(Value) < ErrorTolerance;
    }

    /**
     * @brief Compare if a value is Nearly Zero, using tolerance for inaccuracies.
     * @param A Value to test
     * @param ErrorTolerance Max allowed difference to be considered Zero
     * @return true if A is Nearly Zero
     */
    static FORCEINLINE bool
    isNearlyZero(double Value, double ErrorTolerance = SMALL_NUMBER) {
      return abs<double>(Value) < ErrorTolerance;
    }

    /**
     * @brief Checks is the specified value a power of two. Only works on integer values.
     */
    static FORCEINLINE bool
    isPowerOfTwo(uint32 Value) {
      return (0 == (Value & (Value - 1)));
    }

    /**
     * @brief Returns the closest Grid multiply to the Location
     */
    static FORCEINLINE float
    gridSnap(float Location, float Grid) {
      if (0.f == Grid) return Location;
      else {
        return floor((Location + 0.5f*Grid) / Grid)*Grid;
      }
    }

    /**
     * @brief Compares the value of Comparand and return the "Greater" or
     *        "Lower" value accordingly
     */
    static FORCEINLINE float
    floatSelect(float Comparand, float ValueGEZero, float ValueLTZero) {
      return Comparand >= 0.f ? ValueGEZero : ValueLTZero;
    }

    /**
     * @copydoc Math::floatSelect
     */
    static FORCEINLINE double
    floatSelect(double Comparand, double ValueGEZero, double ValueLTZero) {
      return Comparand >= 0.f ? ValueGEZero : ValueLTZero;
    }

    /**
     * @brief Calculates the tangent space vector for a given set of
     *        positions / texture coords.
     */
    static Vector3
    calculateTriTangent(const Vector3& position1,
                        const Vector3& position2,
                        const Vector3& position3,
                        float u1,
                        float v1,
                        float u2,
                        float v2,
                        float u3,
                        float v3);


    /**
     * @brief Sine function approximation.
     * @param Value Angle in range [0, pi/2].
     * @note  Evaluates trigonometric functions using polynomial approximations.
     */
    static float
    fastSin0(const Radian& Value) {
      return static_cast<float>(fastASin0(Value.valueRadians()));
    }

    /**
     * @brief Sine function approximation.
     * @param Value Angle in range [0, pi/2].
     * @note  Evaluates trigonometric functions using polynomial approximations.
     */
    static float
    fastSin0(float Value);

    /**
     * @brief Sine function approximation.
     * @param Value Angle in range [0, pi/2].
     * @note  Evaluates trigonometric functions using polynomial approximations.
     *        Slightly better (and slower) than "fastSin0".
     */
    static float
    fastSin1(const Radian& Value) {
      return static_cast<float>(fastASin1(Value.valueRadians()));
    }

    /**
     * @brief Sine function approximation.
     * @param Value Angle in range [0, pi/2].
     * @note  Evaluates trigonometric functions using polynomial approximations.
     *        Slightly better (and slower) than "fastSin0".
     */
    static float
    fastSin1(float Value);

    /**
     * @brief Cosine function approximation.
     * @param Value Angle in range [0, pi/2].
     * @note  Evaluates trigonometric functions using polynomial approximations.
     */
    static float
    fastCos0(const Radian& Value) {
      return static_cast<float>(fastACos0(Value.valueRadians()));
    }

    /**
     * @brief Cosine function approximation.
     * @param Value Angle in range [0, pi/2].
     * @note  Evaluates trigonometric functions using polynomial approximations.
     */
    static float
    fastCos0(float Value);

    /**
     * @brief Cosine function approximation.
     * @param Value Angle in range [0, pi/2].
     * @note  Evaluates trigonometric functions using polynomial approximations.
     *        Slightly better (and slower) than "fastCos0".
     */
    static float
    fastCos1(const Radian& Value) {
      return static_cast<float>(fastACos1(Value.valueRadians()));
    }

    /**
     * @brief Cosine function approximation.
     * @param Value Angle in range [0, pi/2].
     * @note  Evaluates trigonometric functions using polynomial approximations.
     *        Slightly better (and slower) than "fastCos0".
     */
    static float
    fastCos1(float Value);

    /**
     * @brief Tangent function approximation.
     * @param Value Angle in range [0, pi/4].
     * @note  Evaluates trigonometric functions using polynomial approximations.
     */
    static float
    fastTan0(const Radian& Value) {
      return static_cast<float>(fastATan0(Value.valueRadians()));
    }

    /**
     * @brief Tangent function approximation.
     * @param Value	Angle in range [0, pi/4].
     * @note  Evaluates trigonometric functions using polynomial approximations.
     */
    static float
    fastTan0(float Value);

    /**
     * @brief Tangent function approximation.
     * @param val Angle in range [0, pi/4].
     * @note  Evaluates trigonometric functions using polynomial approximations.
     *        Slightly better (and slower) than "fastTan0".
     */
    static float
    fastTan1(const Radian& Value) {
      return static_cast<float>(fastATan1(Value.valueRadians()));
    }

    /**
     * @brief Tangent function approximation.
     * @param Value Angle in range [0, pi/4].
     * @note  Evaluates trigonometric functions using polynomial approximations.
     *        Slightly better (and slower) than "fastTan0".
     */
    static float
    fastTan1(float Value);

    /**
     * @brief Inverse sine function approximation.
     * @param Value Angle in range [0, 1].
     * @note  Evaluates trigonometric functions using polynomial approximations.
     */
    static float
    fastASin0(const Radian& Value) {
      return static_cast<float>(fastASin0(Value.valueRadians()));
    }

    /**
     * @brief Inverse sine function approximation.
     * @param Value Angle in range [0, 1].
     * @note  Evaluates trigonometric functions using polynomial approximations.
     */
    static float
    fastASin0(float Value);

    /**
     * @brief Inverse sine function approximation.
     * @param Value Angle in range [0, 1].
     * @note  Evaluates trigonometric functions using polynomial approximations.
     *        Slightly better (and slower) than "fastASin0".
     */
    static float
    fastASin1(const Radian& Value) {
      return static_cast<float>(fastASin1(Value.valueRadians()));
    }

    /**
     * @brief Inverse sine function approximation.
     * @param Value Angle in range [0, 1].
     * @note  Evaluates trigonometric functions using polynomial approximations.
     *        Slightly better (and slower) than "FastASin0".
     */
    static float
    fastASin1(float Value);

    /**
     * @brief Inverse cosine function approximation.
     * @param Value Angle in range [0, 1].
     * @note  Evaluates trigonometric functions using polynomial approximations.
     */
    static float
    fastACos0(const Radian& Value) {
      return static_cast<float>(fastACos0(Value.valueRadians()));
    }

    /**
     * @brief Inverse cosine function approximation.
     * @param Value Angle in range [0, 1].
     * @note  Evaluates trigonometric functions using polynomial approximations.
     */
    static float
    fastACos0(float Value);

    /**
     * @brief Inverse cosine function approximation.
     * @param Value Angle in range [0, 1].
     * @note  Evaluates trigonometric functions using polynomial approximations.
     *        Slightly better (and slower) than "fastACos0".
     */
    static float
    fastACos1(const Radian& Value) {
      return static_cast<float>(fastACos1(Value.valueRadians()));
    }

    /**
     * @brief Inverse cosine function approximation.
     * @param Value Angle in range [0, 1].
     * @note  Evaluates trigonometric functions using polynomial approximations.
     *        Slightly better (and slower) than "fastACos0".
     */
    static float
    fastACos1(float Value);

    /**
     * @brief Inverse tangent function approximation.
     * @param Value Angle in range [-1, 1].
     * @note  Evaluates trigonometric functions using polynomial approximations.
     */
    static float
    fastATan0(const Radian& Value) {
      return static_cast<float>(fastATan0(Value.valueRadians()));
    }

    /**
     * @brief Inverse tangent function approximation.
     * @param Value Angle in range [-1, 1].
     * @note  Evaluates trigonometric functions using polynomial approximations.
     */
    static float
    fastATan0(float Value);

    /**
     * @brief Inverse tangent function approximation.
     * @param Value Angle in range [-1, 1].
     * @note  Evaluates trigonometric functions using polynomial approximations.
     *        Slightly better (and slower) than "fastATan0".
     */
    static float
    fastATan1(const Radian& Value) {
      return static_cast<float>(fastATan1(Value.valueRadians()));
    }

    /**
     * @brief Inverse tangent function approximation.
     * @param Value Angle in range [-1, 1].
     * @note  Evaluates trigonometric functions using polynomial approximations.
     *        Slightly better (and slower) than "FastATan0".
     */
    static float
    fastATan1(float Value);

    /**
     * @brief Utility to ensure angle is between +/- 180 degrees by unwinding.
     */
    static float
    unwindDegrees(float A) {
      while (180.f < A) {
        A -= 360.f;
      }

      while (-180.f > A) {
        A += 360.f;
      }

      return A;
    }

    /**
     * @brief Given a heading which may be outside the +/- PI range, 'unwind'
     *        it back into that range.
     */
    static float
    unwindRadians(float A) {
      while (PI < A) {
        A -= TWO_PI;
      }

      while (-PI > A) {
        A += TWO_PI;
      }

      return A;
    }

    /**
     * @brief Solves the linear equation with the parameters A, B.
     *        Returns number of roots found and the roots themselves will be
     *        output in the @p roots array.
     * @param[out] roots Must be at least size of 1.
     * @note		Only returns real roots.
     */
    template<typename T>
    static uint32
    solveLinear(T A, T B, T* roots) {
      if (!isNearlyEqual(A, (T)0)) {
        roots[0] = -B / A;
        return 1;
      }

      roots[0] = 0.0f;
      return 1;
    }

    /**
     * @brief Solves the quadratic equation with the parameters A, B, C.
     *        Returns number of roots found and the roots themselves will be
     *        output in the @p roots array.
     * @param[out] roots Must be at least size of 2.
     * @note Only returns real roots.
     */
    template<typename T>
    static uint32
    solveQuadratic(T A, T B, T C, T* roots) {
      if (!isNearlyEqual(A, (T)0)) {
        T p = B / (2 * A);
        T q = C / A;
        T D = p * p - q;

        if (!isNearlyEqual(D, (T)0)) {
          if (static_cast<T>(0) > D) {
            return 0;
          }

          T sqrtD = sqrt(D);
          roots[0] = sqrtD - p;
          roots[1] = -sqrtD - p;
          return 2;
        }
        else {
          roots[0] = -p;
          roots[1] = -p;
          return 1;
        }
      }

      return solveLinear(B, C, roots);
    }

    /**
     * @brief Solves the cubic equation with the parameters A, B, C, D.
     *        Returns number of roots found and the roots themselves will be
     *        output in the @p roots array.
     * @param[out] roots Must be at least size of 3.
     * @note Only returns real roots.
     */
    template<typename T>
    static uint32
    solveCubic(T A, T B, T C, T D, T* roots) {
      static const T THIRD = (1 / static_cast<T>(3));

      T invA = 1 / A;
      A = B * invA;
      B = C * invA;
      C = D * invA;

      T sqA = square(A);
      T p = THIRD * (-THIRD * sqA + B);
      T q = (static_cast<T>(0.5)) * ((2 / static_cast<T>(27)) * A * sqA - THIRD * A * B + C);

      T cbp = p * p * p;
      D = q * q + cbp;

      uint32 numRoots = 0;
      if (!isNearlyEqual(D, (T)0)) {
        if (0.0 > D) {
          T phi = THIRD * Acos(-q / sqrt(-cbp));
          T t = 2 * sqrt(-p);

          roots[0] =  t * cos(phi);
          roots[1] = -t * cos(phi + PI * THIRD);
          roots[2] = -t * cos(phi - PI * THIRD);

          numRoots = 3;
        }
        else {
          T sqrtD = sqrt(D);
          T u = cbrt(sqrtD + abs(q));

          if (static_cast<T>(0) < q) {
            roots[0] = -u + p / u;
          }
          else {
            roots[0] = u - p / u;
          }
          numRoots = 1;
        }
      }
      else {
        if (!isNearlyEqual(q, (T)0)) {
          T u = cbrt(-q);
          roots[0] = 2 * u;
          roots[1] = -u;
          numRoots = 2;
        }
        else {
          roots[0] = 0.0f;
          numRoots = 1;
        }
      }

      T sub = THIRD * A;
      for (uint32 i = 0; i<numRoots; ++i) {
        roots[i] -= sub;
      }

      return numRoots;
    }

    /**
     * @brief	Solves the quartic equation with the parameters A, B, C, D, E. 
     *        Returns number of roots found and the roots themselves will be
     *        output in the @p roots array.
     * @param[out] roots	Must be at least size of 4.
     * @note Only returns real roots.
     */
    template<typename T>
    static uint32
    solveQuartic(T A, T B, T C, T D, T E, T* roots) {
      T invA = 1 / A;
      A = B * invA;
      B = C * invA;
      C = D * invA;
      D = E * invA;

      T sqA = square(A);
      T p = -(3 / static_cast<T>(8))   * sqA + B;
      T q =  (1 / static_cast<T>(8))   * sqA * A - static_cast<T>(0.5) * A * B + C;
      T r = -(3 / static_cast<T>(256)) * sqA * sqA + (1 / static_cast<T>(16)) * sqA * B - 
             (1 / static_cast<T>(4)) * A * C + D;

      uint32 numRoots = 0;
      if (!isNearlyEqual(r, static_cast<T>(0))) {
        T cubicA = 1;
        T cubicB = -(T)0.5 * p;
        T cubicC = -r;
        T cubicD = (T)0.5 * r * p - (1 / (T)8) * q * q;

        solveCubic(cubicA, cubicB, cubicC, cubicD, roots);
        T z = roots[0];

        T u = z * z - r;
        T v = 2 * z - p;

        if (isNearlyEqual(u, static_cast<T>(0))) {
          u = 0;
        }
        else if (0 < u) {
          u = sqrt(u);
        }
        else {
          return 0;
        }

        if (isNearlyEqual(v, static_cast<T>(0))) {
          v = 0;
        }
        else if (0 < v) {
          v = sqrt(v);
        }
        else {
          return 0;
        }

        T quadraticA = 1;
        T quadraticB = q < 0 ? -v : v;
        T quadraticC = z - u;

        numRoots = solveQuadratic(quadraticA, quadraticB, quadraticC, roots);

        quadraticA = 1;
        quadraticB = q < 0 ? v : -v;
        quadraticC = z + u;

        numRoots += solveQuadratic(quadraticA, quadraticB, quadraticC, roots + numRoots);
      }
      else
      {
        numRoots = solveCubic(q, p, (T)0, (T)1, roots);
        roots[numRoots++] = 0;
      }

      T sub = (1 / static_cast<T>(4)) * A;
      for (uint32 i = 0; i < numRoots; ++i) {
        roots[i] -= sub;
      }

      return numRoots;
    }

    /**
     * @brief Checks if two 2D lines intersect
     * @return true if the lines intersect, false otherwise
     */
    static bool
    lineLineIntersection(const Vector2& aa,
                         const Vector2& ab,
                         const Vector2& ba,
                         const Vector2& bb);

    /**
     * @brief Get the point where two 2D lines intersect
     * @return Vector2 describing the point of intersection
     */
    static Vector2
    getLineLineIntersect(const Vector2& aa,
                         const Vector2& ab,
                         const Vector2& ba,
                         const Vector2& bb);

    /**
     * @brief Find the intersection of a line and an offset plane. Assumes that
     *        the line and plane do indeed intersect; you must make sure they're
     *        not parallel before calling.
     * @param Point1 the first point defining the line
     * @param Point2 the second point defining the line
     * @param PlaneOrigin the origin of the plane
     * @param PlaneNormal the normal of the plane
     * @return The point of intersection between the line and the plane.
     */
    static Vector3
    linePlaneIntersection(const Vector3& Point1,
                          const Vector3& Point2,
                          const Vector3& PlaneOrigin,
                          const Vector3& PlaneNormal);

    /**
     * @brief Find the intersection of a line and a plane. Assumes that the
     *        line and plane do indeed intersect; you must make sure they're
     *        not parallel before calling.
     * @param Point1 the first point defining the line
     * @param Point2 the second point defining the line
     * @param plane the Plane
     * @return The point of intersection between the line and the plane.
     */
    static Vector3
    linePlaneIntersection(const Vector3& Point1,
                          const Vector3& Point2,
                          const Plane& plane);

    /**
     * @brief Determine if a plane and an AABB intersect
     * @param P - the plane to test
     * @param AABB - the axis aligned bounding box to test
     * @return if collision occurs
     */
    static bool
    planeAABBIntersection(const Plane& InP, const AABox& AABB);

    /**
     * @brief Checks if the Point intersects with the Box
     * @param Point the point
     * @param Box the Box to test against
     * @return true if the point intersects with the box false otherwise
     */
    static bool
    pointBoxIntersection(const Vector3& Point, const AABox& Box);

    /**
     * @brief Checks if a line intersects with a box
     * @param Box the Box to test against
     * @param Start the starting position of the line
     * @param End the end position of the line
     * @param Direction Direction of the testing vector
     * @return true if the line intersects with the box false otherwise
     */
    static bool
    lineBoxIntersection(const AABox& Box,
                        const Vector3& Start,
                        const Vector3& End,
                        const Vector3& Direction);

    /**
     * @brief Checks if a line intersects with a box
     * @param Box the Box to test against
     * @param Start the starting position of the line
     * @param End the end position of the line
     * @param Direction Direction of the testing vector
     * @param OneOverDirection Reciprocal direction of the testing
     * @return true if the line intersects with the box false otherwise
     */
    static bool
    lineBoxIntersection(const AABox& Box,
                        const Vector3& Start,
                        const Vector3& End,
                        const Vector3& Direction,
                        const Vector3& OneOverDirection);

    static bool
    lineSphereIntersection(const Vector3& Start,
                           const Vector3& Dir,
                           float Length,
                           const Vector3& Origin,
                           float Radius);

    static bool
    intersectPlanes2(Vector3& I, Vector3& D, const Plane& P1, const Plane& P2);

    static bool
    intersectPlanes3(Vector3& I, const Plane& P1, const Plane& P2, const Plane& P3);

    static float
    getRangePct(Vector2 const& Range, float Value);

    static float
    getRangeValue(Vector2 const& Range, float Pct);

    static bool
    sphereAABBIntersection(const Vector3& SphereCenter,
                           const float RadiusSquared,
                           const AABox& AABB);

    static bool
    sphereAABBIntersection(const Sphere& InSphere, const AABox& AABB);

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

    /**
     * Magic numbers for numerical precision.
     */
    
     /** Thickness of plane for front/back/inside test */
    static const float THRESH_POINT_ON_PLANE;
    
    /** Thickness of polygon side's side-plane for point-inside/outside/on side test */
    static const float THRESH_POINT_ON_SIDE;
    
    /** Two points are same if within this distance */
    static const float THRESH_POINTS_ARE_SAME;
    
    /**
     * Two points are near if within this distance and can be combined
     * if imprecise math is ok
     */
    static const float THRESH_POINTS_ARE_NEAR;
    
    /** Two normal points are same if within this distance */
    static const float THRESH_NORMALS_ARE_SAME;

    /*************************************************************************/
    //Making this too large results in incorrect CSG classification and disaster
     /**
      *Two vectors are near if within this distance and can be combined
      * if imprecise math is ok
      */
    static const float THRESH_VECTORS_ARE_NEAR;
    /*************************************************************************/

    /** A plane splits a polygon in half */
    static const float THRESH_SPLIT_POLY_WITH_PLANE;
    
    /** A plane exactly splits a polygon */
    static const float THRESH_SPLIT_POLY_PRECISELY;
    
    /** Size of a unit normal that is considered "zero", squared */
    static const float THRESH_ZERO_NORM_SQUARED;
    
    /**
     * Two unit vectors are parallel if abs(A dot B) is greater than or equal to this.
     * This is roughly cosine(1.0 degrees).
     */
    static const float THRESH_NORMALS_ARE_PARALLEL;
    
    /**
     * Two unit vectors are orthogonal (perpendicular) if abs(A dot B) is less
     * than or equal this. This is roughly cosine(89.0 degrees).
     */
    static const float THRESH_NORMALS_ARE_ORTHOGONAL;

    /** Allowed error for a normalized vector (against squared magnitude) */
    static const float THRESH_VECTOR_NORMALIZED;
    
    /** Allowed error for a normalized quaternion (against squared magnitude) */
    static const float THRESH_QUAT_NORMALIZED;
  };
}
