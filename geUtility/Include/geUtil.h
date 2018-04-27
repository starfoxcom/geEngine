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
  using std::size_t;
  using std::conditional;
  using std::is_enum;
  using std::hash;
  using std::memset;

  /**
   * @brief Generates a new hash for the provided type using the default hasher
   *        and combines it with a previous hash.
   * @note  This one came out of boost::hash_combine
  */
  template <class T>
  inline void
  hash_combine(size_t& seed, const T& v) {
    using HashType = typename conditional<is_enum<T>::value, EnumClassHash, hash<T>>::type;
    
    HashType hasher;
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

  /**
   * @brief Sets contents of a struct to zero.
   */
  template<class T>
  void
  ge_zero_out(T& s) {
    memset(&s, 0, sizeof(T));
  }

  /**
   * @brief Sets contents of a static array to zero.
   */
  template<class T, SIZE_T N>
  void
  ge_zero_out(T(&arr)[N]) {
    memset(arr, 0, sizeof(T) * N);
  }

  /**
   * @brief Sets contents of a block of memory to zero.
   */
  template<class T>
  void
  ge_zero_out(T* arr, SIZE_T count) {
    GE_ASSERT(nullptr != arr);
    memset(arr, 0, sizeof(T) * count);
  }

  /**
   * @brief Returns the size of the provided static array.
   */
  template <class T, SIZE_T N>
  constexpr SIZE_T
  ge_size(const T(&array)[N]) {
    return N;
  }
}
