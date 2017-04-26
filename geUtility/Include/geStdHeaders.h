/*****************************************************************************/
/**
 * @file    geStdHeaders.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2015/02/09
 * @brief   Header for the STD libraries
 *
 * This file includes the most common and required STL objects
 * It takes account of the operating system of the build to modify
 * the required object
 *
 * @bug	    No known bugs.
 */
/*****************************************************************************/
#pragma once

/*****************************************************************************/
/**
 * If we are on a Borland compiler (C++ Builder)
 */
/*****************************************************************************/
#ifdef __BORLANDC__
# define __STD_ALGORITHM
#endif

/*****************************************************************************/
/**
 * C type objects
 */
/*****************************************************************************/
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <cstring>
#include <cstdarg>
#include <cmath>

/*****************************************************************************/
/**
 * For memory management
 */
/*****************************************************************************/
#include <memory>

/*****************************************************************************/
/**
 * STL containers
 */
/*****************************************************************************/
#include <vector>
#include <stack>
#include <map>
#include <string>
#include <set>
#include <list>
#include <deque>
#include <queue>
#include <bitset>
#include <array>

/*****************************************************************************/
/**
 * Note - not in the original STL, but exists in SGI STL and STLport
 * For GCC 4.3 see http://gcc.gnu.org/gcc-4.3/changes.html
 */
/*****************************************************************************/
#if (GE_COMPILER == GE_COMPILER_GNUC)
# if GE_COMP_VER >= 430
#   include <tr1/unordered_map>
#   include <tr1/unordered_set>
# elif (GE_PLATFORM == GE_PLATFORM_PS4)
#   include <unordered_map>
#   include <unordered_set>
# else
#   include <ext/hash_map>
#   include <ext/hash_set>
# endif
#else
# if (GE_COMPILER == GE_COMPILER_MSVC) && GE_COMP_VER >= 1600 //VC++ 10.0 or higher
#   include <unordered_map>
#   include <unordered_set>
# else
#   include <hash_set>
#   include <hash_map>
# endif
#endif

/*****************************************************************************/
/**
 * STL algorithms & functions
 */
/*****************************************************************************/
#include <algorithm>
#include <functional>
#include <limits>

/*****************************************************************************/
/**
 * C++ Stream stuff
 */
/*****************************************************************************/
#include <fstream>
#include <iostream>
#include <iomanip>
#include <sstream>

#ifdef __BORLANDC__
  namespace geEngineSDK {
    using namespace std;
  }
#endif

/*****************************************************************************/
/**
 * C Types and Stats
 */
/*****************************************************************************/
extern "C" {
# include <sys/types.h>
# include <sys/stat.h>
}

/*****************************************************************************/
/**
 * Windows specifics
 */
/*****************************************************************************/
#if GE_PLATFORM == GE_PLATFORM_WIN32
  //Undefine min & max
# undef min
# undef max

# if !defined(NOMINMAX) && defined(_MSC_VER)
#   define NOMINMAX     //Required to stop windows.h messing up std::min
# endif
# if defined( __MINGW32__ )
#   include <unistd.h>
# endif
#endif

/*****************************************************************************/
/**
 * Linux specifics
 */
/*****************************************************************************/
#if GE_PLATFORM == GE_PLATFORM_LINUX
  extern "C" {
#   include <unistd.h>
#   include <dlfcn.h>
  }
#endif

/*****************************************************************************/
/**
 * Apple specifics
 */
/*****************************************************************************/
#if GE_PLATFORM == GE_PLATFORM_APPLE
  extern "C" {
#   include <unistd.h>
#   include <sys/param.h>
#   include <CoreFoundation/CoreFoundation.h>
  }
#endif

/*****************************************************************************/
/**
 * PS4 specifics
 */
/*****************************************************************************/
#if GE_PLATFORM == GE_PLATFORM_PS4
  extern "C" {
#   include <unistd.h>
#   include <sys/param.h>
  }
#endif

namespace geEngineSDK {
  /***************************************************************************/
	/**
   * Standard containers, for easier access in my own namespace
   */
  /***************************************************************************/
  template <typename T, typename A = StdAlloc<T>>
  using Deque = std::deque<T, A>;

  template <typename T, typename A = StdAlloc<T>>
  using Vector = std::vector<T, A>;

  template <typename T, typename A = StdAlloc<T>>
  using List = std::list<T, A>;

  template <typename T, typename A = StdAlloc<T>>
  using Stack = std::stack<T, std::deque<T, A>>;

  template <typename T, typename A = StdAlloc<T>>
  using Queue = std::queue<T, std::deque<T, A>>;

  template <typename T, typename P = std::less<T>, typename A = StdAlloc<T>>
  using Set = std::set<T, P, A>;

  template <typename K, 
            typename V, 
            typename P = std::less<K>, 
            typename A = StdAlloc<std::pair<const K, V>>>
  using Map = std::map<K, V, P, A>;

  template <typename K, 
            typename V, 
            typename P = std::less<K>, 
            typename A = StdAlloc<std::pair<const K, V>>>
  using MultiMap = std::multimap<K, V, P, A>;

  template <typename T, 
            typename H = std::hash<T>, 
            typename C = std::equal_to<T>, 
            typename A = StdAlloc<T>>
  using UnorderedSet = std::unordered_set<T, H, C, A>;

  template <typename K, 
            typename V, 
            typename H = std::hash<K>, 
            typename C = std::equal_to<K>, 
            typename A = StdAlloc<std::pair<const K, V>>>
  using UnorderedMap = std::unordered_map<K, V, H, C, A>;

  template <typename K, 
            typename V, 
            typename H = std::hash<K>, 
            typename C = std::equal_to<K>, 
            typename A = StdAlloc<std::pair<const K, V>>>
  using UnorderedMultimap = std::unordered_multimap<K, V, H, C, A>;

  /***************************************************************************/
  /**
   * Smart pointers
   */
  /***************************************************************************/
  
  /**
   * @brief Smart pointer that retains shared ownership of a project through
   *        a pointer. The object is destroyed automatically when the last
   *        shared pointer to the object is destroyed.
   */
	template <typename T>
	using SPtr = std::shared_ptr<T>;

  /**
   * @brief Smart pointer that retains shared ownership of a project through
   *        a pointer. Reference to the object must be unique. The object is
   *        destroyed automatically when the pointer to the object is destroyed.
   */
	template <typename T, typename Alloc = GenAlloc>
	using UPtr = std::unique_ptr<T, decltype(&ge_delete<T, Alloc>)>;

  /**
   * @brief Create a new shared pointer using a custom allocator category.
   */
  template<class Type, class AllocCategory, class... Args>
	SPtr<Type>
  ge_shared_ptr_new(Args&&... args) {
		return std::allocate_shared<Type>(StdAlloc<Type, AllocCategory>(),
                                      std::forward<Args>(args)...);
	}

	/**
   * @brief Create a new shared pointer using the default allocator category.
   */
  template<class Type, class... Args>
  SPtr<Type>
  ge_shared_ptr_new(Args&&... args) {
    return std::allocate_shared<Type>(StdAlloc<Type, GenAlloc>(),
                                      std::forward<Args>(args)...);
  }

	/**
   * @brief	Create a new shared pointer from a previously constructed object.
   *        Pointer specific data will be allocated using the provided allocator category.
   */
	template<class Type, class MainAlloc = GenAlloc, class PtrDataAlloc = GenAlloc>
	SPtr<Type>
  ge_shared_ptr(Type* data) {
		return std::shared_ptr<Type>(data, &ge_delete<Type, MainAlloc>, 
									 StdAlloc<Type, PtrDataAlloc>());
	}

  /**
   * @brief Create a new unique pointer from a previously constructed object.
   *        Pointer specific data will be allocated using the provided allocator category.
   */
  template<class Type, class Alloc = GenAlloc>
  UPtr<Type, Alloc>
  ge_unique_ptr(Type* data) {
    return std::unique_ptr<Type,
                           decltype(&ge_delete<Type, Alloc>)> (data, ge_delete<Type, Alloc>);
  }

  /**
   * @brief	Create a new unique pointer using a custom allocator category.
   */
  template<class Type, class Alloc, class... Args>
  UPtr<Type>
  ge_unique_ptr_new(Args &&... args) {
    Type* rawPtr = ge_new<Type, Alloc>(std::forward<Args>(args)...);
    return ge_unique_ptr<Type, Alloc>(rawPtr);
  }

  /**
   * @brief	Create a new unique pointer using the default allocator category.
   */
  template<class Type, class... Args>
  UPtr<Type>
  ge_unique_ptr_new(Args &&... args) {
    Type* rawPtr = ge_new<Type, GenAlloc>(std::forward<Args>(args)...);
    return ge_unique_ptr<Type, GenAlloc>(rawPtr);
  }
}
