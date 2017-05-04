/*****************************************************************************/
/**
 * @file    geUtil.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2016/03/06
 * @brief   Utility functions
 *
 * Some utility functions used across the whole engine but not specific to any
 * object type
 *
 * @bug     No known bugs.
 */
/*****************************************************************************/
#pragma once

namespace geEngineSDK {

  /**
   * @brief Generates a new hash for the provided type using the default hasher
   *        and combines it with a previous hash.
   * @note  This one came out of boost::hash_combine
  */
  template <class T>
  inline void
  hash_combine(std::size_t& seed, const T& v) {
    std::hash<T> hasher;
    seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
  }

  /**
   * @brief Generates an MD5 hash string for the provided source string.
   */
  String GE_UTILITY_EXPORT
  md5(const WString& source);

  /**
   * @brief Generates an MD5 hash string for the provided source string.
   */
  String GE_UTILITY_EXPORT
  md5(const String& source);
}
