/*****************************************************************************/
/**
 * @file    geStringTableManager.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2017/11/01
 * @brief   Wrapper around an Unicode string for localization purposes.
 *
 * String handle. Provides a wrapper around an Unicode string, primarily for
 * localization purposes.
 * Actual value for this string is looked up in a global string table based on
 * the provided identifier string and currently active language. If such value
 * doesn't exist then the identifier is used as is.
 *
 * Use {0}, {1}, etc. in the string for values that might change dynamically.
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

namespace geEngineSDK {

}