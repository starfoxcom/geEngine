/********************************************************************/
/**
 * @file   geColor.h
 * @author Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date   2015/04/08
 * @brief  Color structures for use on the geEngineSDK
 *
 * Color represented as 4 components, each being a floating point
 * value ranging from 0 to 1.
 *
 * @bug	   No known bugs.
 */
 /********************************************************************/
#pragma once

/************************************************************************************************************************/
/* Includes                                                                     										*/
/************************************************************************************************************************/
#include "gePrerequisitesUtil.h"
#include "geMath.h"
#include "geFloat16Color.h"

namespace geEngineSDK
{
	/************************************************************************************************************************/
	/**
	* @brief	Color represented as 4 components, each being a floating point value ranging from 0 to 1.
	*			Color components are Red, Green, Blue and Alpha.
	*/
	/************************************************************************************************************************/
	class GE_UTILITY_EXPORT LinearColor
	{
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
		static float PowOneOver255Table[256];		/** Static lookup table used for Color -> LinearColor conversion. */
	public:
		float R, G, B, A;

		FORCEINLINE LinearColor()
		{

		}

		FORCEINLINE explicit LinearColor(FORCE_INIT::E) : R(0), G(0), B(0), A(0)
		{
		
		}

		FORCEINLINE LinearColor(float InR, float InG, float InB, float InA = 1.0f) : R(InR), G(InG), B(InB), A(InA)
		{
		
		}

		LinearColor(const Vector3& Vector);
		LinearColor(const Vector4& Vector);
		LinearColor(const Color& C);					/** Color->LinearColor conversion. */
		explicit LinearColor(const Float16Color& C);	/** Float16Color->LinearColor conversion. */

		/************************************************************************************************************************/
		/*													OPERATORS															*/
		/************************************************************************************************************************/
		
		/************************************************************************************************************************/
		/**
		* @brief	Pointer accessor for direct copying.
		*/
		/************************************************************************************************************************/
		FORCEINLINE float* Ptr()
		{
			return &R;
		}

		/************************************************************************************************************************/
		/**
		* @brief	Pointer accessor for direct copying.
		*/
		/************************************************************************************************************************/
		FORCEINLINE const float* Ptr() const
		{
			return &R;
		}

		/************************************************************************************************************************/
		/**
		* @brief	Error-tolerant comparison, returns true if they are really close to be the same
		*/
		/************************************************************************************************************************/
		FORCEINLINE bool Equals(const LinearColor& ColorB, float Tolerance = Math::KINDA_SMALL_NUMBER) const
		{
			return Math::Abs(this->R - ColorB.R) < Tolerance && Math::Abs(this->G - ColorB.G) < Tolerance && Math::Abs(this->B - ColorB.B) < Tolerance && Math::Abs(this->A - ColorB.A) < Tolerance;
		}

		FORCEINLINE bool operator==(const LinearColor& rhs) const
		{
			return this->R == rhs.R && this->G == rhs.G && this->B == rhs.B && this->A == rhs.A;
		}
		FORCEINLINE bool operator!=(const LinearColor& rhs) const
		{
			return this->R != rhs.R || this->G != rhs.G || this->B != rhs.B || this->A != rhs.A;
		}

		FORCEINLINE float operator[] (const int32 Index) const
		{
			GE_ASSERT(Index >= 0 && Index < 4);
			if(Index == 0)		{ return R; }
			else if(Index == 1)	{ return G; }
			else if(Index == 2)	{ return B; }
			else				{ return A; }
		}

		FORCEINLINE float& operator[] (const int32 Index)
		{
			GE_ASSERT(Index >= 0 && Index < 4);
			if(Index == 0)		{ return R; }
			else if(Index == 1)	{ return G; }
			else if(Index == 2)	{ return B; }
			else				{ return A; }
		}

		FORCEINLINE LinearColor operator+ (const LinearColor& rhs) const
		{
			return LinearColor(R + rhs.R, G + rhs.G, B + rhs.B, A + rhs.A);
		}

		FORCEINLINE LinearColor& operator+= (const LinearColor& rhs)
		{
			R += rhs.R;
			G += rhs.G;
			B += rhs.B;
			A += rhs.A;

			return *this;
		}

		FORCEINLINE LinearColor operator- (const LinearColor& rhs) const
		{
			return LinearColor(R - rhs.R, G - rhs.G, B - rhs.B, A - rhs.A);
		}

		FORCEINLINE LinearColor& operator-= (const LinearColor& rhs)
		{
			R -= rhs.R;
			G -= rhs.G;
			B -= rhs.B;
			A -= rhs.A;

			return *this;
		}

		FORCEINLINE LinearColor operator* (const float rhs) const
		{
			return LinearColor(R * rhs, G * rhs, B * rhs, A * rhs);
		}

		FORCEINLINE LinearColor& operator*= (const float rhs)
		{
			R *= rhs;
			G *= rhs;
			B *= rhs;
			A *= rhs;

			return *this;
		}

		FORCEINLINE LinearColor operator* (const LinearColor& rhs) const
		{
			return LinearColor(R * rhs.R, G * rhs.G, B * rhs.B, A * rhs.A);
		}

		FORCEINLINE LinearColor& operator*=(const LinearColor& rhs)
		{
			R *= rhs.R;
			G *= rhs.G;
			B *= rhs.B;
			A *= rhs.A;
			return *this;
		}

		FORCEINLINE LinearColor operator/ (const float rhs) const
		{
			const float	InvScalar = 1.0f / rhs;	//To avoids possible zero division (it's also faster)
			return LinearColor(this->R * InvScalar, this->G * InvScalar, this->B * InvScalar, this->A * InvScalar);
		}

		FORCEINLINE LinearColor& operator/= (const float rhs)
		{
			const float	InvScalar = 1.0f / rhs;
			R *= InvScalar;
			G *= InvScalar;
			B *= InvScalar;
			A *= InvScalar;
			return *this;
		}

		FORCEINLINE LinearColor operator/ (const LinearColor& rhs) const
		{
			return LinearColor(R / rhs.R, G / rhs.G, B / rhs.B, A / rhs.A);
		}

		FORCEINLINE LinearColor& operator/=(const LinearColor& rhs)
		{
			R /= rhs.R;
			G /= rhs.G;
			B /= rhs.B;
			A /= rhs.A;
			return *this;
		}

		/************************************************************************************************************************/
		/**
		* @brief	Quantizes the linear color and returns the result as a FColor.  This bypasses the sRGB conversion.
		*/
		/************************************************************************************************************************/
		Color Quantize() const;

		/************************************************************************************************************************/
		/**
		* @brief	Quantizes the linear color and returns the result as a Color with optional sRGB conversion and quality as goal
		*/
		/************************************************************************************************************************/
		Color ToColor(const bool bSRGB) const;

		/************************************************************************************************************************/
		/**
		* @brief	Convert from float to RGBE as outlined in Gregory Ward's Real Pixels article, Graphics Gems II, page 80.
		*/
		/************************************************************************************************************************/
		Color ToRGBE(void) const;

		/************************************************************************************************************************/
		/**
		* @brief	Clamps color value to the range [0..1].
		*/
		/************************************************************************************************************************/
		FORCEINLINE void Saturate()
		{
			R = Math::Clamp(R, 0.0f, 1.0f);
			G = Math::Clamp(G, 0.0f, 1.0f);
			B = Math::Clamp(B, 0.0f, 1.0f);
			A = Math::Clamp(A, 0.0f, 1.0f);
		}

		/************************************************************************************************************************/
		/**
		* @brief	Clamps color value to the range [0..1]. Returned the saturated color as a copy.
		*/
		/************************************************************************************************************************/
		FORCEINLINE LinearColor SaturateCopy() const
		{
			return GetClamped();
		}

		/************************************************************************************************************************/
		/**
		* @brief	Returns a desaturated color, with 0 meaning no desaturation and 1 == full desaturation
		*
		* @param	Desaturation	Desaturation factor in range [0..1]
		* @return	Desaturated color
		*/
		/************************************************************************************************************************/
		LinearColor Desaturate(float Desaturation) const;

		/************************************************************************************************************************/
		/**
		* @brief	Computes the perceptually weighted luminance value of a color.
		* @note		The Luminance Factor used here is (0.3, 0.59, 0.11)
		*/
		/************************************************************************************************************************/
		float ComputeLuminance() const;

		/************************************************************************************************************************/
		/**
		* @brief	Clamps the color to the submitted scale
		*/
		/************************************************************************************************************************/
		FORCEINLINE LinearColor GetClamped(float InMin = 0.0f, float InMax = 1.0f) const
		{
			LinearColor Ret;

			Ret.R = Math::Clamp(R, InMin, InMax);
			Ret.G = Math::Clamp(G, InMin, InMax);
			Ret.B = Math::Clamp(B, InMin, InMax);
			Ret.A = Math::Clamp(A, InMin, InMax);

			return Ret;
		}
		
		LinearColor CopyWithNewOpacity(float NewOpacicty) const
		{
			LinearColor NewCopy = *this;
			NewCopy.A = NewOpacicty;
			return NewCopy;
		}

		/************************************************************************************************************************/
		/**
		* @brief	Converts a linear space RGB color to an HSV color
		*/
		/************************************************************************************************************************/
		LinearColor LinearRGBToHSV() const;

		/************************************************************************************************************************/
		/**
		* @brief	Converts an HSV color to a linear space RGB color
		* @note		In this color, the values must be: R = H, G = S, B = V
		*/
		/************************************************************************************************************************/
		LinearColor HSVToLinearRGB() const;

		/************************************************************************************************************************/
		/**
		* @brief	Euclidean distance between two Colors
		*/
		/************************************************************************************************************************/
		static FORCEINLINE float Dist(const LinearColor &V1, const LinearColor &V2)
		{
			return Math::Sqrt(Math::Square(V2.R - V1.R) + Math::Square(V2.G - V1.G) + Math::Square(V2.B - V1.B) + Math::Square(V2.A - V1.A));
		}

		/************************************************************************************************************************/
		/**
		* @brief	Returns the maximum value in this color structure
		* @return	The maximum color channel value
		*/
		/************************************************************************************************************************/
		FORCEINLINE float GetMax() const
		{
			return Math::Max(Math::Max(Math::Max(R, G), B), A);
		}

		/************************************************************************************************************************/
		/**
		* @brief	Returns the minimum value in this color structure
		* @return	The minimum color channel value
		*/
		/************************************************************************************************************************/
		FORCEINLINE float GetMin() const
		{
			return Math::Min(Math::Min(Math::Min(R, G), B), A);
		}

		/************************************************************************************************************************/
		/**
		* @brief	Useful to detect if a light contribution needs to be rendered
		*/
		/************************************************************************************************************************/
		bool IsAlmostBlack() const
		{
			return Math::Square(R) < Math::DELTA && Math::Square(G) < Math::DELTA && Math::Square(B) < Math::DELTA;
		}

		FORCEINLINE float GetLuminance() const
		{
			return R * 0.3f + G * 0.59f + B * 0.11f;
		}
	};

	FORCEINLINE LinearColor operator*(float Scalar, const LinearColor& Color)
	{
		return Color.operator*(Scalar);
	}

	/************************************************************************************************************************/
	/**
	* @brief	Color represented as 4 components, each being a floating point value ranging from 0 to 1.
	*			Color components are Red, Green, Blue and Alpha.
	*/
	/************************************************************************************************************************/
	class GE_UTILITY_EXPORT Color
	{
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

		// Variables.
		#if GE_ENDIAN == GE_ENDIAN_LITTLE
			#if GE_COMPILER == GE_COMPILER_MSVC

#pragma warning( push )
#pragma warning( disable : 4201 )

				//Win32 x86
				union
				{
					struct
					{
						uint8 B, G, R, A;
					};
					uint32 AlignmentDummy;
				};

#pragma warning( pop )

			#else
				//Linux x86, etc
				uint8 B GCC_ALIGN(4);
				uint8 G, R, A;
			#endif
		#else
				union { struct{ uint8 A, R, G, B; }; uint32 AlignmentDummy; };
		#endif

		uint32& DWColor(void) { return *((uint32*)this); }
		const uint32& DWColor(void) const { return *((uint32*)this); }

		FORCEINLINE Color()
		{

		}

		FORCEINLINE explicit Color(FORCE_INIT::E)
		{
			//Put these into the body for proper ordering with INTEL vs non-INTEL_BYTE_ORDER
			R = G = B = A = 0;
		}

		FORCEINLINE Color(uint8 InR, uint8 InG, uint8 InB, uint8 InA = 255)
		{
			//Put these into the body for proper ordering with INTEL vs non-INTEL_BYTE_ORDER
			R = InR;
			G = InG;
			B = InB;
			A = InA;
		}

		/************************************************************************************************************************/
		/**
		* @brief	Fast, for more accuracy use LinearColor::ToColor()
		* @note		TODO: doesn't handle negative colors well, implicit constructor can cause accidental conversion
		*			(better use .ToColor(true))
		*/
		/************************************************************************************************************************/
		Color(const LinearColor& C)
		{
			// put these into the body for proper ordering with INTEL vs non-INTEL_BYTE_ORDER
			R = (int8)Math::Clamp(Math::Trunc(Math::Pow(C.R, 1.0f / 2.2f) * 255.0f), 0, 255);
			G = (int8)Math::Clamp(Math::Trunc(Math::Pow(C.G, 1.0f / 2.2f) * 255.0f), 0, 255);
			B = (int8)Math::Clamp(Math::Trunc(Math::Pow(C.B, 1.0f / 2.2f) * 255.0f), 0, 255);
			A = (int8)Math::Clamp(Math::Trunc(C.A							* 255.0f), 0, 255);
		}

		FORCEINLINE explicit Color(uint32 InColor)
		{
			DWColor() = InColor;
		}

		/************************************************************************************************************************/
		/*													OPERATORS															*/
		/************************************************************************************************************************/
		FORCEINLINE bool operator==(const Color &C) const
		{
			return DWColor() == C.DWColor();
		}

		FORCEINLINE bool operator!=(const Color& C) const
		{
			return DWColor() != C.DWColor();
		}

		FORCEINLINE void operator+=(const Color& C)
		{
			R = (uint8)Math::Min((int32)R + (int32)C.R, 255);
			G = (uint8)Math::Min((int32)G + (int32)C.G, 255);
			B = (uint8)Math::Min((int32)B + (int32)C.B, 255);
			A = (uint8)Math::Min((int32)A + (int32)C.A, 255);
		}

		/************************************************************************************************************************/
		/**
		* @brief	Convert from RGBE to float as outlined in Gregory Ward's Real Pixels article, Graphics Gems II, page 80.
		*/
		/************************************************************************************************************************/
		LinearColor FromRGBE() const;

		/************************************************************************************************************************/
		/**
		* @brief	Makes a color red->green with the passed in scalar (e.g. 0 is red, 1 is green)
		*/
		/************************************************************************************************************************/
		static Color MakeRedToGreenColorFromScalar(float Scalar);

		/************************************************************************************************************************/
		/**
		* @brief	Returns a new Color based of this color with the new alpha value.
		* @note		Usage: const Color& MyColor = ColorList::Green.WithAlpha(128);
		*/
		/************************************************************************************************************************/
		Color WithAlpha(uint8 Alpha) const
		{
			return Color(R, G, B, Alpha);
		}

		/************************************************************************************************************************/
		/**
		* @brief	Reinterprets the color as a linear color.
		* @return	The linear color representation.
		*/
		/************************************************************************************************************************/
		FORCEINLINE LinearColor ReinterpretAsLinear() const
		{
			return LinearColor(R / 255.f, G / 255.f, B / 255.f, A / 255.f);
		}
	};

	FORCEINLINE uint32 GetTypeHash(const Color& color)
	{
		return color.DWColor();
	}

	FORCEINLINE uint32 GetTypeHash(const LinearColor& color)
	{
		return GetTypeHash(Color(color));
	}

	/** Computes a brightness and a fixed point color from a floating point color. */
	extern GE_UTILITY_EXPORT void ComputeAndFixedColorAndIntensity(const LinearColor& InLinearColor, Color& OutColor, float& OutIntensity);

	GE_ALLOW_MEMCPY_SERIALIZATION(LinearColor);
	GE_ALLOW_MEMCPY_SERIALIZATION(Color);
}
