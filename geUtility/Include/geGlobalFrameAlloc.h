/*****************************************************************************/
/**
 * @file    geGlobalFrameAlloc.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2016/03/06
 * @brief   Global Frame allocator objects
 *
 * Functions used to create a Frame allocator for each thread
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
#include "geStdHeaders.h"
#include "geThreadDefines.h"

namespace geEngineSDK {
  class FrameAlloc;

  /**
   * @brief Returns a global, application wide frame allocator.
   *        Each thread gets its own frame allocator.
   * @note  Thread safe.
   */
  GE_UTILITY_EXPORT FrameAlloc&
  g_frameAlloc();

  /**
   * @brief Allocates some memory using the global frame allocator.
   * @param[in] numBytes  Number of bytes to allocate.
   */
  GE_UTILITY_EXPORT uint8*
  ge_frame_alloc(SIZE_T numBytes);

  /**
   * @brief Allocates the specified number of bytes aligned to the provided boundary,
   *        using the global frame allocator. Boundary is in bytes and must be a power of two.
   */
  GE_UTILITY_EXPORT uint8*
  ge_frame_alloc_aligned(SIZE_T count, SIZE_T align);


  /**
   * @brief Deallocates memory allocated with the global frame allocator.
   * @note  Must be called on the same thread the memory was allocated on.
   */
  GE_UTILITY_EXPORT void
  ge_frame_free(void* data);

  /**
   * @brief Frees memory previously allocated with ge_frame_alloc_aligned().
   * @note	Must be called on the same thread the memory was allocated on.
   */
  GE_UTILITY_EXPORT void
  ge_frame_free_aligned(void* data);

  /**
   * @brief Allocates enough memory to hold the object of specified type using
   *        the global frame allocator, but does not construct the object.
   */
  template<class T>
  T*
  ge_frame_alloc() {
    return reinterpret_cast<T*>(ge_frame_alloc(sizeof(T)));
  }

  /**
   * @brief Allocates enough memory to hold N objects of specified type using
   *        the global frame allocator, but does not construct the object.
   */
  template<class T>
  T*
  ge_frame_alloc(SIZE_T numBytes) {
    return reinterpret_cast<T*>(ge_frame_alloc(sizeof(T) * numBytes));
  }

  /**
   * @brief Allocates enough memory to hold the object(s) of specified type
   *        using the global frame allocator, and constructs them.
   */
  template<class T>
  T*
  ge_frame_new(SIZE_T count = 0) {
    T* data = ge_frame_alloc<T>(count);

    for (SIZE_T i = 0; i<count; ++i) {
      new (reinterpret_cast<void*>(&data[i])) T;
    }

    return data;
  }

  /**
   * @brief Allocates enough memory to hold the object(s) of specified type
   *        using the global frame allocator, and constructs them.
   */
  template<class T, class... Args>
  T*
  ge_frame_new(Args&& ...args, SIZE_T count = 0) {
    T* data = ge_frame_alloc<T>(count);
    for (SIZE_T i = 0; i<count; ++i) {
      new (reinterpret_cast<void*>(&data[i])) T(std::forward<Args>(args)...);
    }
    return data;
  }

  /**
   * @brief Destructs and deallocates an object allocated with the global frame allocator.
   * @note  Must be called on the same thread the memory was allocated on.
   */
  template<class T>
  void
  ge_frame_delete(T* data) {
    data->~T();
    ge_frame_free(reinterpret_cast<void*>(data));
  }

  /**
   * @brief Destructs and deallocates an array of objects allocated with the
   *        global frame allocator.
   * @note  Must be called on the same thread the memory was allocated on.
   */
  template<class T>
  void
  ge_frame_delete(T* data, SIZE_T count) {
    for (SIZE_T i = 0; i<count; ++i) {
      data[i].~T();
    }

    ge_frame_free(reinterpret_cast<void*>(data));
  }

  /**
   * @copydoc FrameAlloc::markFrame
   */
  GE_UTILITY_EXPORT inline void
  ge_frame_mark();

  /**
   * @copydoc FrameAlloc::clear
   */
  GE_UTILITY_EXPORT inline void
  ge_frame_clear();

  typedef std::basic_string<ANSICHAR,
                            std::char_traits<ANSICHAR>,
                            StdAlloc<ANSICHAR, FrameAlloc>>
                            FrameString;

  typedef std::basic_string<UNICHAR,
                            std::char_traits<UNICHAR>,
                            StdAlloc<UNICHAR, FrameAlloc>>
                            FrameWString;

  template<typename T, typename A = StdAlloc<T, FrameAlloc>>
  using FrameVector = std::vector<T, A>;

  template<typename T, typename A = StdAlloc<T, FrameAlloc>>
  using FrameStack = std::stack<T, std::deque<T, A>>;

  template <typename T, typename A = StdAlloc<T, FrameAlloc>>
  using FrameQueue = std::queue<T, std::deque<T, A>>;

  template<typename T, typename P = std::less<T>, typename A = StdAlloc<T, FrameAlloc>>
  using FrameSet = std::set<T, P, A>;

  template<typename K, 
           typename V,
           typename P = std::less<K>,
           typename A = StdAlloc<std::pair<const K, V>, FrameAlloc>>
  using FrameMap = std::map<K, V, P, A>;

  template<typename T,
           typename H = std::hash<T>,
           typename C = std::equal_to<T>,
           typename A = StdAlloc<T, FrameAlloc>>
  using FrameUnorderedSet = std::unordered_set<T, H, C, A>;

  template<typename K,
           typename V,
           typename H = std::hash<K>,
           typename C = std::equal_to<K>,
           typename A = StdAlloc<std::pair<const K, V>, FrameAlloc>>
  using FrameUnorderedMap = std::unordered_map<K, V, H, C, A>;

  extern GE_THREADLOCAL FrameAlloc* _GlobalFrameAlloc;

  /**
   * @brief Specialized memory allocator implementations that allows use of a
   *        global frame allocator in normal new/delete/free/dealloc operators.
   */
  template<>
  class MemoryAllocator<FrameAlloc> : public MemoryAllocatorBase
  {
   public:
    /** @copydoc MemoryAllocator::allocate */
    static void*
    allocate(SIZE_T bytes) {
      return ge_frame_alloc(bytes);
    }

    /** @copydoc MemoryAllocator::allocateAligned */
    static void*
    allocateAligned(SIZE_T bytes, SIZE_T alignment) {
#if GE_PROFILING_ENABLED
      incrementAllocCount();
#endif
      return ge_frame_alloc_aligned(bytes, alignment);
    }

    /** @copydoc MemoryAllocator::allocateAligned16 */
    static void*
    allocateAligned16(SIZE_T bytes) {
#if GE_PROFILING_ENABLED
      incrementAllocCount();
#endif
      return ge_frame_alloc_aligned(bytes, 16);
    }

    /** @copydoc MemoryAllocator::free */
    static void
    free(void* ptr) {
      ge_frame_free(ptr);
    }

    /** @copydoc MemoryAllocator::freeAligned */
    static void
    freeAligned(void* ptr) {
#if GE_PROFILING_ENABLED
      incrementFreeCount();
#endif
      ge_frame_free_aligned(ptr);
    }

    /** @copydoc MemoryAllocator::freeAligned16 */
    static void
    freeAligned16(void* ptr) {
#if GE_PROFILING_ENABLED
      incrementFreeCount();
#endif
      ge_frame_free_aligned(ptr);
    }
  };
}
