/*****************************************************************************/
/**
 * @file    geFloat16Color.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2015/05/05
 * @brief   RGBA Color made up of Float16.
 *
 * Class to use color on a reduced memory and precision ambient
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
#include "geFloat16.h"

namespace geEngineSDK {
  class LinearColor;

  /**
   * @brief RGBA Color made up of Float16.
   */
  class GE_UTILITY_EXPORT Float16Color
  {
   public:
    Float16Color() {}
    Float16Color(const Float16Color& Src) {
      R = Src.R;
      G = Src.G;
      B = Src.B;
      A = Src.A;
    }

    Float16Color(const LinearColor& Src);

    /**
     * @brief	Assignment operator
     */
    Float16Color& operator=(const Float16Color& Src) {
      R = Src.R;
      G = Src.G;
      B = Src.B;
      A = Src.A;
      return *this;
    }

    /**
     * @brief Checks whether two colors are identical.
     * @param Src The other color.
     * @return true if the two colors are identical, otherwise false.
     */
    bool operator==(const Float16Color& Src) {
      return ((R == Src.R) && (G == Src.G) && (B == Src.B) && (A == Src.A));
    }

   public:
    Float16 R;
    Float16 G;
    Float16 B;
    Float16 A;
  };
}
