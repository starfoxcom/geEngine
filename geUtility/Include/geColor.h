/*****************************************************************************/
/**
 * @file    geColor.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2015/04/08
 * @brief   Color structures for use on the geEngineSDK
 *
 * Color represented as 4 components, each being a floating point value ranging
 * from 0 to 1.
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
#include "geMath.h"

namespace geEngineSDK {
  class Float16Color;
  class Vector3;
  class Vector4;

  namespace GAMMA_SPACE {
    enum E {
      kLinear,
      kPow22,
      ksRGB
    };
  }

  /**
   * @brief Color represented as 4 components, each being a floating point
   *        value ranging from 0 to 1.
   *        Color components are Red, Green, Blue and Alpha.
   */
  class GE_UTILITY_EXPORT LinearColor
  {
   public:
    LinearColor() {}
    explicit LinearColor(FORCE_INIT::E) : R(0), G(0), B(0), A(0) {}
    LinearColor(float InR, float InG, float InB, float InA = 1.0f) 
      : R(InR),
        G(InG),
        B(InB),
        A(InA) {}

    explicit LinearColor(const Vector3& Vector);
    explicit LinearColor(const Vector4& Vector);
    
    /**
     * @brief Color->LinearColor conversion.
     */
    explicit LinearColor(const Color& refColor);

    /**
     * @brief Float16Color->LinearColor conversion.
     */
    explicit LinearColor(const Float16Color& refColor);

    /**
     * @brief Pointer accessor for direct copying.
     */
    float*
    ptr() {
      return &R;
    }

    /**
     * @brief Pointer accessor for direct copying.
     */
    const float*
    ptr() const {
      return &R;
    }

    /**
     * @brief Error-tolerant compare, returns true if they are really close to be the same
     */
    bool
    equals(const LinearColor& ColorB, float Tolerance = Math::KINDA_SMALL_NUMBER) const {
      return  Math::isNearlyEqual(R, ColorB.R, Tolerance) && 
              Math::isNearlyEqual(G, ColorB.G, Tolerance) && 
              Math::isNearlyEqual(B, ColorB.B, Tolerance) && 
              Math::isNearlyEqual(A, ColorB.A, Tolerance);
    }

    bool
    operator==(const LinearColor& rhs) const {
      return  R == rhs.R && 
              G == rhs.G && 
              B == rhs.B &&
              A == rhs.A;
    }

    bool
    operator!=(const LinearColor& rhs) const {
      return  R != rhs.R ||
              G != rhs.G ||
              B != rhs.B ||
              A != rhs.A;
    }

    const float&
    operator[](const int32 Index) const {
      GE_ASSERT(Index >= 0 && Index < 4);
      return (&R)[Index];
    }

    float&
    operator[](const int32 Index) {
      GE_ASSERT(Index >= 0 && Index < 4);
      return (&R)[Index];
    }

    LinearColor
    operator+(const LinearColor& rhs) const {
      return LinearColor(R + rhs.R, G + rhs.G, B + rhs.B, A + rhs.A);
    }

    LinearColor&
    operator+=(const LinearColor& rhs) {
      R += rhs.R;
      G += rhs.G;
      B += rhs.B;
      A += rhs.A;
      return *this;
    }

    LinearColor
    operator-(const LinearColor& rhs) const {
      return LinearColor(R - rhs.R, G - rhs.G, B - rhs.B, A - rhs.A);
    }

    LinearColor&
    operator-=(const LinearColor& rhs) {
      R -= rhs.R;
      G -= rhs.G;
      B -= rhs.B;
      A -= rhs.A;
      return *this;
    }

    LinearColor
    operator*(const float rhs) const {
      return LinearColor(R * rhs, G * rhs, B * rhs, A * rhs);
    }

    LinearColor&
    operator*=(const float rhs) {
      R *= rhs;
      G *= rhs;
      B *= rhs;
      A *= rhs;
      return *this;
    }

    LinearColor
    operator*(const LinearColor& rhs) const {
      return LinearColor(R * rhs.R, G * rhs.G, B * rhs.B, A * rhs.A);
    }

    LinearColor&
    operator*=(const LinearColor& rhs) {
      R *= rhs.R;
      G *= rhs.G;
      B *= rhs.B;
      A *= rhs.A;
      return *this;
    }

    LinearColor
    operator/(const float rhs) const {
      //To avoids possible zero division (it's also faster)
      const float	InvScalar = 1.0f / rhs;
      return LinearColor(R * InvScalar,
                         G * InvScalar,
                         B * InvScalar,
                         A * InvScalar);
    }

    LinearColor&
    operator/=(const float rhs) {
      const float	InvScalar = 1.0f / rhs;
      R *= InvScalar;
      G *= InvScalar;
      B *= InvScalar;
      A *= InvScalar;
      return *this;
    }

    LinearColor
    operator/(const LinearColor& rhs) const {
      return LinearColor(R / rhs.R, G / rhs.G, B / rhs.B, A / rhs.A);
    }

    LinearColor&
    operator/=(const LinearColor& rhs) {
      R /= rhs.R;
      G /= rhs.G;
      B /= rhs.B;
      A /= rhs.A;
      return *this;
    }

    /**
     * @brief Quantizes the linear color and returns the result as a Color.
     *        This bypasses the sRGB conversion.
     */
    Color
    quantize() const;

    /**
     * @brief Quantizes the linear color and returns the result as a Color
     *        with optional sRGB conversion and quality as goal
     */
    Color
    toColor(const bool bSRGB) const;

    /**
     * @brief Convert from float to RGBE as outlined in Gregory Ward's Real
     *        Pixels article, Graphics Gems II, page 80.
     */
    Color
    toRGBE(void) const;

    /**
     * @brief Converts an Color coming from an observed sRGB output, into a linear color.
     * @param Color The sRGB color that needs to be converted into linear space.
     */
    static LinearColor
    fromSRGBColor(const Color& color);

    /**
    * @brief Converts an Color coming from an observed pow(1/2.2) output, into a linear color.
    * @param color The pow(1/2.2) color that needs to be converted into linear space.
    */
    static LinearColor
    fromPow22Color(const Color& color);

    /**
     * @brief Clamps color value to the range [0..1].
     */
    void
    saturate() {
      R = Math::clamp(R, 0.0f, 1.0f);
      G = Math::clamp(G, 0.0f, 1.0f);
      B = Math::clamp(B, 0.0f, 1.0f);
      A = Math::clamp(A, 0.0f, 1.0f);
    }

    /**
     * @brief Clamps color value to the range [0..1].
     *        Returned the saturated color as a copy.
     */
    LinearColor
    saturateCopy() const {
      return getClamped();
    }

    /**
     * @brief Returns a desaturated color, with 0 meaning no desaturation
     *        and 1 == full desaturation
     * @param Desaturation  Desaturation factor in range [0..1]
     * @return Desaturated color
     */
    LinearColor
    desaturate(float Desaturation) const;

    /**
     * @brief Computes the perceptually weighted luminance value of a color.
     * @note The Luminance Factor used here is (0.3, 0.59, 0.11)
     */
    float
    computeLuminance() const;

    /**
     * @brief Clamps the color to the submitted scale
     */
    LinearColor
    getClamped(float InMin = 0.0f, float InMax = 1.0f) const {
      LinearColor Ret;
      Ret.R = Math::clamp(R, InMin, InMax);
      Ret.G = Math::clamp(G, InMin, InMax);
      Ret.B = Math::clamp(B, InMin, InMax);
      Ret.A = Math::clamp(A, InMin, InMax);
      return Ret;
    }

    LinearColor
    copyWithNewOpacity(float NewOpacicty) const {
      LinearColor NewCopy = *this;
      NewCopy.A = NewOpacicty;
      return NewCopy;
    }

    /**
     * @brief Converts byte hue-saturation-brightness to floating point red-green-blue.
     */
    static LinearColor
    getHSV(uint8 H, uint8 S, uint8 V);

    /**
     * @brief Makes a random but quite nice color.
     */
    static LinearColor
    makeRandomColor();

    /**
     * @brief Converts temperature in Kelvins of a black body radiator to RGB chromaticity.
     */
    static LinearColor
    makeFromColorTemperature(float Temp);

    /**
     * @brief Converts a linear space RGB color to an HSV color
     */
    LinearColor
    linearRGBToHSV() const;

    /**
     * @brief Converts an HSV color to a linear space RGB color
     * @note  In this color, the values must be: R = H, G = S, B = V
     */
    LinearColor
    hsvToLinearRGB() const;

    /**
     * @brief Linearly interpolates between two colors by the specified progress amount.
     *        The interpolation is performed in HSV color space taking the shortest path
     *        to the new color's hue. This can give better results than Math::lerp(), but
     *        is much more expensive. The incoming colors are in RGB space, and the output
     *        color will be RGB. The alpha value will also be interpolated.
     *
     * @param From The color and alpha to interpolate from as linear RGBA
     * @param To The color and alpha to interpolate to as linear RGBA
     * @param Progress Scalar interpolation amount (usually between 0.0 and 1.0 inclusive)
     * @return The interpolated color in linear RGB space along with the interpolated alpha
     */
    static LinearColor
    lerpUsingHSV(const LinearColor& From, const LinearColor& To, const float Progress);

    /**
     * @brief Euclidean distance between two Colors
     */
    static FORCEINLINE float
    dist(const LinearColor &V1, const LinearColor &V2) {
      return Math::sqrt(Math::square(V2.R - V1.R) +
                        Math::square(V2.G - V1.G) +
                        Math::square(V2.B - V1.B) +
                        Math::square(V2.A - V1.A));
    }

    /**
     * @brief Returns the maximum value in this color structure
     * @return  The maximum color channel value
     */
    FORCEINLINE float
    getMax() const {
      return Math::max(Math::max3(R, G, B), A);
    }

    /**
     * @brief Returns the minimum value in this color structure
     * @return  The minimum color channel value
     */
    FORCEINLINE float
    getMin() const {
      return Math::min(Math::min3(R, G, B), A);
    }

    /**
     * @brief Useful to detect if a light contribution needs to be rendered
     */
    bool
    isAlmostBlack() const {
      return Math::square(R) < Math::DELTA &&
             Math::square(G) < Math::DELTA &&
             Math::square(B) < Math::DELTA;
    }

    FORCEINLINE float
    getLuminance() const {
      return R * 0.3f + G * 0.59f + B * 0.11f;
    }

   public:
    static const LinearColor Transparent;
    static const LinearColor White;
    static const LinearColor Gray;
    static const LinearColor Black;
    static const LinearColor Red;
    static const LinearColor Green;
    static const LinearColor Blue;
    static const LinearColor Yellow;
    static const LinearColor Cyan;
    static const LinearColor Magenta;
   public:
     /** Static lookup table used for Color -> LinearColor conversion. */
     static float s_pow22OneOver255Table[256];

     /** Static lookup table used for Color -> LinearColor conversion. sRGB */
     static float s_sRGBToLinearTable[256];
   public:
    float R, G, B, A;
  };

  FORCEINLINE LinearColor
  operator*(float Scalar, const LinearColor& color) {
    return color.operator*(Scalar);
  }

  typedef uint32 RGBA;
  typedef uint32 ARGB;
  typedef uint32 ABGR;
  typedef uint32 BGRA;

  /**
   * @brief Color represented as 4 components, each being a floating point
   *        value ranging from 0 to 1.
   *        Color components are Red, Green, Blue and Alpha.
   */
  class GE_UTILITY_EXPORT Color
  {
   public:
    Color() {}

    explicit Color(FORCE_INIT::E) {
      //Put these into the body for proper ordering with
      //INTEL vs non-INTEL_BYTE_ORDER
      R = G = B = A = 0;
    }

    Color(uint8 InR, uint8 InG, uint8 InB, uint8 InA = 255) {
      //Put these into the body for proper ordering with
      //INTEL vs non-INTEL_BYTE_ORDER
      R = InR;
      G = InG;
      B = InB;
      A = InA;
    }

    explicit Color(uint32 InColor) {
      dwColor() = InColor;
    }
   
   private:
     /**
      * @brief Please use .toColor(true) on LinearColor if you wish to convert
      *       from LinearColor to Color, with proper sRGB conversion applied.
      * Note: Do not implement or make public. We don't want people needlessly
      *       and implicitly converting between LinearColor and Color.
      *       It's not a free conversion.
      */
     explicit Color(const LinearColor& LinearColor);
  
   public:
    uint32&
    dwColor(void) {
      return *(reinterpret_cast<uint32*>(this));
    }

    const uint32&
    dwColor(void) const {
      return *((uint32*)this);
    }

    bool
    operator==(const Color &C) const {
      return dwColor() == C.dwColor();
    }

    bool
    operator!=(const Color& C) const {
      return dwColor() != C.dwColor();
    }

    void
    operator+=(const Color& C) {
      R = static_cast<int8>(Math::min(static_cast<int32>(R) + static_cast<int32>(C.R), 255));
      G = static_cast<int8>(Math::min(static_cast<int32>(G) + static_cast<int32>(C.G), 255));
      B = static_cast<int8>(Math::min(static_cast<int32>(B) + static_cast<int32>(C.B), 255));
      A = static_cast<int8>(Math::min(static_cast<int32>(A) + static_cast<int32>(C.A), 255));
    }

    /**
     * @brief Convert from RGBE to float as outlined in Gregory Ward's
     *        Real Pixels article, Graphics Gems II, page 80.
     */
    LinearColor
    fromRGBE() const;

    /**
     * @brief Returns a new Color based of this color with the new alpha value.
     * @note  Usage: const Color& MyColor = ColorList::Green.WithAlpha(128);
     */
    Color
    withAlpha(uint8 Alpha) const {
      return Color(R, G, B, Alpha);
    }

    /**
     * @brief Reinterprets the color as a linear color.
     * @return  The linear color representation.
     */
    LinearColor
    reinterpretAsLinear() const {
      return LinearColor(R / 255.f, G / 255.f, B / 255.f, A / 255.f);
    }

    static Color
    makeRandomColor();

    static Color
    makeRedToGreenColorFromScalar(float Scalar);

    static Color
    makeFromColorTemperature(float Temp);

    /**
     * @brief Gets the color in a packed uint32 format packed in the order ARGB.
     */
    FORCEINLINE ARGB
    toPackedARGB() const {
      return (A << 24) | (R << 16) | (G << 8) | (B << 0);
    }

    /**
     * @brief Gets the color in a packed uint32 format packed in the order ABGR.
     */
    FORCEINLINE ABGR toPackedABGR() const {
      return (A << 24) | (B << 16) | (G << 8) | (R << 0);
    }

    /**
     * @brief Gets the color in a packed uint32 format packed in the order RGBA.
     */
    FORCEINLINE RGBA
    toPackedRGBA() const {
      return (R << 24) | (G << 16) | (B << 8) | (A << 0);
    }

    /**
     * @brief Gets the color in a packed uint32 format packed in the order BGRA.
     */
    FORCEINLINE BGRA
    toPackedBGRA() const {
      return (B << 24) | (G << 16) | (R << 8) | (A << 0);
    }

   public:
    static const Color Transparent;
    static const Color White;
    static const Color Gray;
    static const Color Black;
    static const Color Red;
    static const Color Green;
    static const Color Blue;
    static const Color Yellow;
    static const Color Cyan;
    static const Color Magenta;
    static const Color Orange;
    static const Color Purple;
    static const Color Turquoise;
    static const Color Silver;
    static const Color Emerald;

#if GE_ENDIAN == GE_ENDIAN_LITTLE
# if GE_COMPILER == GE_COMPILER_MSVC
#   pragma warning( push )
#   pragma warning( disable : 4201 )
    //Win32 x86
    union {
      struct
      {
        uint8 B, G, R, A;
      };
      uint32 AlignmentDummy;
    };
#   pragma warning( pop )
# else
    //Linux x86, etc
    uint8 B GCC_ALIGN(4);
    uint8 G, R, A;
# endif
#else
    union {
      struct
      {
        uint8 A, R, G, B;
      };
      uint32 AlignmentDummy;
    };
#endif
  };

  /** Computes a brightness and a fixed point color from a floating point color. */
  extern GE_UTILITY_EXPORT void
  computeAndFixedColorAndIntensity(const LinearColor& InLinearColor,
                                   Color& OutColor,
                                   float& OutIntensity);

  GE_ALLOW_MEMCPY_SERIALIZATION(LinearColor);
  GE_ALLOW_MEMCPY_SERIALIZATION(Color);
}

namespace std {
  /** Hash value generator for Color. */
  template<>
  struct hash<geEngineSDK::Color>
  {
    size_t operator()(const geEngineSDK::Color& color) const {
      size_t hash = 0;
      geEngineSDK::hash_combine(hash, color.R);
      geEngineSDK::hash_combine(hash, color.G);
      geEngineSDK::hash_combine(hash, color.B);
      geEngineSDK::hash_combine(hash, color.A);

      return hash;
    }
  };
}