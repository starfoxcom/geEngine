/*****************************************************************************/
/**
 * @file    Vector2I.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2015/04/14
 * @brief   Structure for integer points in 2-d space.
 *
 * Structure for integer points in 2-d space.
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
#include "gePrerequisitesUtil.h"

namespace geEngineSDK {
  /**
   * @brief Structure for integer points in 2-d space.
   */
  struct Vector2I
  {
   public:
    /**
     * @brief Default constructor (no initialization).
     */
    Vector2I();

    /**
     * @brief Create and initialize a new instance with the specified coordinates.
     * @param InX The x-coordinate.
     * @param InY The y-coordinate.
     */
    Vector2I(int32 InX, int32 InY);

    /**
     * @brief Create and initialize a new instance to zero.
     *
     * @param EForceInit Force init enum
     */
    explicit FORCEINLINE Vector2I(FORCE_INIT::E);

    /**
     * @brief Get specific component of a point.
     * @param PointIndex Index of point component.
     * @return const reference to component.
     */
    const int32&
    operator()(int32 PointIndex) const;

    /**
     * @brief Get specific component of a point.
     * @param PointIndex Index of point component
     * @return reference to component.
     */
    int32&
    operator()(int32 PointIndex);

    /**
    * Compare two points for equality.
    *
    * @param Other The other int point being compared.
    * @return true if the points are equal, false otherwise.
    */
    bool operator==(const Vector2I& Other) const;

    /**
    * Compare two points for inequality.
    *
    * @param Other The other int point being compared.
    * @return true if the points are not equal, false otherwise.
    */
    bool operator!=(const Vector2I& Other) const;

    /**
    * Scale this point.
    *
    * @param Scale What to multiply the point by.
    * @return Reference to this point after multiplication.
    */
    Vector2I& operator*=(int32 Scale);

    /**
    * Divide this point by a scalar.
    *
    * @param Divisor What to divide the point by.
    * @return Reference to this point after division.
    */
    Vector2I& operator/=(int32 Divisor);

    /**
    * Add another point component-wise to this point.
    *
    * @param Other The point to add to this point.
    * @return Reference to this point after addition.
    */
    Vector2I& operator+=(const Vector2I& Other);

    /**
    * Subtract another point component-wise from this point.
    *
    * @param Other The point to subtract from this point.
    * @return Reference to this point after subtraction.
    */
    Vector2I& operator-=(const Vector2I& Other);

    /**
    * Divide this point component-wise by another point.
    *
    * @param Other The point to divide with.
    * @return Reference to this point after division.
    */
    Vector2I& operator/=(const Vector2I& Other);

    /**
    * Assign another point to this one.
    *
    * @param Other The point to assign this point from.
    * @return Reference to this point after assignment.
    */
    Vector2I& operator=(const Vector2I& Other);

    /**
    * Get the result of scaling on this point.
    *
    * @param Scale What to multiply the point by.
    * @return A new scaled int point.
    */
    Vector2I operator*(int32 Scale) const;

    /**
    * Get the result of division on this point.
    *
    * @param Divisor What to divide the point by.
    * @return A new divided int point.
    */
    Vector2I operator/(int32 Divisor) const;

    /**
    * Get the result of addition on this point.
    *
    * @param Other The other point to add to this.
    * @return A new combined int point.
    */
    Vector2I operator+(const Vector2I& Other) const;

    /**
    * Get the result of subtraction from this point.
    *
    * @param Other The other point to subtract from this.
    * @return A new subtracted int point.
    */
    Vector2I operator-(const Vector2I& Other) const;

    /**
    * Get the result of division on this point.
    *
    * @param Other The other point to subtract from this.
    * @return A new subtracted int point.
    */
    Vector2I operator/(const Vector2I& Other) const;

    /**
    * Get specific component of the point.
    *
    * @param Index the index of point component
    * @return reference to component.
    */
    int32& operator[](int32 Index);

    /**
    * Get specific component of the point.
    *
    * @param Index the index of point component
    * @return copy of component value.
    */
    int32 operator[](int32 Index) const;

  public:

    /**
    * Get the component-wise min of two points.
    *
    * @see ComponentMax, GetMax
    */
    FORCEINLINE Vector2I ComponentMin(const Vector2I& Other) const;

    /**
    * Get the component-wise max of two points.
    *
    * @see ComponentMin, GetMin
    */
    FORCEINLINE Vector2I ComponentMax(const Vector2I& Other) const;

    /**
    * Get the larger of the point's two components.
    *
    * @return The maximum component of the point.
    * @see GetMin, Size, SizeSquared
    */
    int32 GetMax() const;

    /**
    * Get the smaller of the point's two components.
    *
    * @return The minimum component of the point.
    * @see GetMax, Size, SizeSquared
    */
    int32 GetMin() const;

    /**
    * Get the distance of this point from (0,0).
    *
    * @return The distance of this point from (0,0).
    * @see GetMax, GetMin, SizeSquared
    */
    int32 Size() const;

    /**
    * Get the squared distance of this point from (0,0).
    *
    * @return The squared distance of this point from (0,0).
    * @see GetMax, GetMin, Size
    */
    int32 SizeSquared() const;

    /**
    * Get a textual representation of this point.
    *
    * @return A string describing the point.
    */
    FString ToString() const;

  public:

    /**
    * Divide an int point and round up the result.
    *
    * @param lhs The int point being divided.
    * @param Divisor What to divide the int point by.
    * @return A new divided int point.
    * @see DivideAndRoundDown
    */
    static Vector2I DivideAndRoundUp(Vector2I lhs, int32 Divisor);
    static Vector2I DivideAndRoundUp(Vector2I lhs, Vector2I Divisor);

    /**
    * Divide an int point and round down the result.
    *
    * @param lhs The int point being divided.
    * @param Divisor What to divide the int point by.
    * @return A new divided int point.
    * @see DivideAndRoundUp
    */
    static Vector2I DivideAndRoundDown(Vector2I lhs, int32 Divisor);

    /**
    * Get number of components point has.
    *
    * @return number of components point has.
    */
    static int32 Num();

  public:

    /**
    * Serialize the point.
    *
    * @param Ar The archive to serialize into.
    * @param Point The point to serialize.
    * @return Reference to the Archive after serialization.
    */
    friend FArchive& operator<<(FArchive& Ar, Vector2I& Point) {
      return Ar << Point.x << Point.y;
    }

    /**
    * Serialize the point.
    *
    * @param Ar The archive to serialize into.
    * @return true on success, false otherwise.
    */
    bool Serialize(FArchive& Ar) {
      Ar << *this;
      return true;
    }

    /**
    * @brief Holds the point's x-coordinate.
    */
    int32 x;

    /**
    * @brief Holds the point's y-coordinate.
    */
    int32 y;

    /**
    * @brief An integer point with zeroed values.
    */
    static const Vector2I ZERO;
  };


  /* Vector2I inline functions
  *****************************************************************************/

  FORCEINLINE Vector2I::Vector2I() {}


  FORCEINLINE Vector2I::Vector2I(int32 InX, int32 InY)
    : x(InX)
    , y(InY) {}


  FORCEINLINE Vector2I::Vector2I(EForceInit)
    : x(0)
    , y(0) {}


  FORCEINLINE const int32& Vector2I::operator()(int32 PointIndex) const {
    return (&x)[PointIndex];
  }


  FORCEINLINE int32& Vector2I::operator()(int32 PointIndex) {
    return (&x)[PointIndex];
  }


  FORCEINLINE int32 Vector2I::Num() {
    return 2;
  }


  FORCEINLINE bool Vector2I::operator==(const Vector2I& Other) const {
    return x == Other.x && y == Other.y;
  }


  FORCEINLINE bool Vector2I::operator!=(const Vector2I& Other) const {
    return (x != Other.x) || (y != Other.y);
  }


  FORCEINLINE Vector2I& Vector2I::operator*=(int32 Scale) {
    x *= Scale;
    y *= Scale;

    return *this;
  }


  FORCEINLINE Vector2I& Vector2I::operator/=(int32 Divisor) {
    x /= Divisor;
    y /= Divisor;

    return *this;
  }


  FORCEINLINE Vector2I& Vector2I::operator+=(const Vector2I& Other) {
    x += Other.x;
    y += Other.y;

    return *this;
  }


  FORCEINLINE Vector2I& Vector2I::operator-=(const Vector2I& Other) {
    x -= Other.x;
    y -= Other.y;

    return *this;
  }


  FORCEINLINE Vector2I& Vector2I::operator/=(const Vector2I& Other) {
    x /= Other.x;
    y /= Other.y;

    return *this;
  }


  FORCEINLINE Vector2I& Vector2I::operator=(const Vector2I& Other) {
    x = Other.x;
    y = Other.y;

    return *this;
  }


  FORCEINLINE Vector2I Vector2I::operator*(int32 Scale) const {
    return Vector2I(*this) *= Scale;
  }


  FORCEINLINE Vector2I Vector2I::operator/(int32 Divisor) const {
    return Vector2I(*this) /= Divisor;
  }


  FORCEINLINE int32& Vector2I::operator[](int32 Index) {
    check(Index >= 0 && Index < 2);
    return ((Index == 0) ? x : y);
  }


  FORCEINLINE int32 Vector2I::operator[](int32 Index) const {
    check(Index >= 0 && Index < 2);
    return ((Index == 0) ? x : y);
  }


  FORCEINLINE Vector2I Vector2I::ComponentMin(const Vector2I& Other) const {
    return Vector2I(FMath::Min(x, Other.x), FMath::Min(y, Other.y));
  }


  FORCEINLINE Vector2I Vector2I::ComponentMax(const Vector2I& Other) const {
    return Vector2I(FMath::Max(x, Other.x), FMath::Max(y, Other.y));
  }

  FORCEINLINE Vector2I Vector2I::DivideAndRoundUp(Vector2I lhs, int32 Divisor) {
    return Vector2I(FMath::DivideAndRoundUp(lhs.x, Divisor), FMath::DivideAndRoundUp(lhs.y, Divisor));
  }

  FORCEINLINE Vector2I Vector2I::DivideAndRoundUp(Vector2I lhs, Vector2I Divisor) {
    return Vector2I(FMath::DivideAndRoundUp(lhs.x, Divisor.x), FMath::DivideAndRoundUp(lhs.y, Divisor.y));
  }

  FORCEINLINE Vector2I Vector2I::DivideAndRoundDown(Vector2I lhs, int32 Divisor) {
    return Vector2I(FMath::DivideAndRoundDown(lhs.x, Divisor), FMath::DivideAndRoundDown(lhs.y, Divisor));
  }


  FORCEINLINE Vector2I Vector2I::operator+(const Vector2I& Other) const {
    return Vector2I(*this) += Other;
  }


  FORCEINLINE Vector2I Vector2I::operator-(const Vector2I& Other) const {
    return Vector2I(*this) -= Other;
  }


  FORCEINLINE Vector2I Vector2I::operator/(const Vector2I& Other) const {
    return Vector2I(*this) /= Other;
  }


  FORCEINLINE int32 Vector2I::GetMax() const {
    return FMath::Max(x, y);
  }


  FORCEINLINE int32 Vector2I::GetMin() const {
    return FMath::Min(x, y);
  }

  FORCEINLINE uint32 GetTypeHash(const Vector2I& InPoint) {
    return HashCombine(GetTypeHash(InPoint.x), GetTypeHash(InPoint.y));
  }


  FORCEINLINE int32 Vector2I::Size() const {
    int64 X64 = (int64)x;
    int64 Y64 = (int64)y;
    return int32(FMath::Sqrt(float(X64 * X64 + Y64 * Y64)));
  }

  FORCEINLINE int32 Vector2I::SizeSquared() const {
    return x*x + y*y;
  }

  FORCEINLINE FString Vector2I::ToString() const {
    return FString::Printf(TEXT("X=%d Y=%d"), x, y);
  }

  template <> struct TIsPODType<Vector2I> { enum { Value = true }; };
}
