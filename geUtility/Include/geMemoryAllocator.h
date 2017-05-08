/*****************************************************************************/
/**
 * @file    geMemoryAllocator.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2015/02/16
 * @brief   Memory allocation objects and templates
 *
 * Define templates and classes used to allocate memory and create
 * object on the heap dynamically.
 * Also defines StdAlloc used to define the way STL objects will
 * allocate their objects
 *
 * @bug	    No known bugs.
 */
/*****************************************************************************/
#pragma once

#undef min
#undef max

/*****************************************************************************/
/**
 * Includes
 */
/*****************************************************************************/
#include <new>
#include <atomic>
#include <limits>
#include <utility>

namespace geEngineSDK {
  class MemoryAllocatorBase;

  /**
   * @class MemoryCounter
   * @brief Thread safe class used for storing total number of memory
   *        allocations and deallocations, primarily for statistic purposes.
   */
  class MemoryCounter
  {
   public:
    static GE_UTILITY_EXPORT uint64
    getNumAllocs() { 
      return m_allocs;
    }
    
    static GE_UTILITY_EXPORT uint64
    getNumFrees() {
      return m_frees;
    }

   private:
    friend class MemoryAllocatorBase;

    /**
     * Thread local data can't be exported, so some magic to make it accessible
     * from MemoryAllocator
     */
    static GE_UTILITY_EXPORT void
    incrementAllocCount() {
      m_allocs++;
    }
    
    static GE_UTILITY_EXPORT void
    incrementFreeCount() {
      m_frees++;
    }

    static GE_THREADLOCAL uint64 m_allocs;
    static GE_THREADLOCAL uint64 m_frees;
  };

  /**
   * @class MemoryAllocatorBase
   * @brief Base class all memory allocators need to inherit.
   *        Provides allocation and free counting.
   */
  class MemoryAllocatorBase
  {
   protected:
    static void
    incrementAllocCount() {
      MemoryCounter::incrementAllocCount();
    }

    static void
    incrementFreeCount() {
      MemoryCounter::incrementFreeCount();
    }
  };

  /**
   * @class MemoryAllocator
   * @brief Provides a generic implementation. Specialize for specific
   *        categories as needed. For example you might implement a pool
   *        allocator for specific types in order to reduce allocation overhead.
   *        By default standard malloc/free are used.
   */
  template<class T>
  class MemoryAllocator : public MemoryAllocatorBase
  {
   public:
    static inline void*
    allocate(SIZE_T bytes) {
#if GE_PROFILING_ENABLED
      incrementAllocCount();
#endif
      return malloc(bytes);
    }

    static inline void*
    allocateArray(SIZE_T bytes, SIZE_T count) {
#if GE_PROFILING_ENABLED
      incrementAllocCount();
#endif
      return malloc(bytes * count);
    }

    static inline void
    free(void* ptr) {
#if GE_PROFILING_ENABLED
      incrementFreeCount();
#endif
      ::free(ptr);
    }

    static inline void
    freeArray(void* ptr, SIZE_T count) {
      GE_UNREFERENCED_PARAMETER(count);
#if GE_PROFILING_ENABLED
      incrementFreeCount();
#endif
      ::free(ptr);
    }
  };

  /**
   * @brief General allocator provided by the OS. Use for persistent long term
   *        allocations, and allocations that don't happen often.
   */
  class GenAlloc
  {};

  /**
   * @brief Allocates the specified number of bytes.
   */
  template<class Alloc>
  inline void*
  ge_alloc(SIZE_T count) {
    return MemoryAllocator<Alloc>::allocate(count);
  }

  /**
   * @brief Allocates enough bytes to hold the specified type, but doesn't construct it.
   */
  template<class T, class Alloc>
  inline T*
  ge_alloc() {
    return (T*)MemoryAllocator<Alloc>::allocate(sizeof(T));
  }

  /**
   * @brief Creates and constructs an array of "count" elements.
   */
  template<class T, class Alloc>
  inline T*
  ge_newN(SIZE_T count) {
    T* ptr = reinterpret_cast<T*>(MemoryAllocator<Alloc>::allocateArray(sizeof(T), count));
    for (SIZE_T i = 0; i < count; ++i) {
      new (reinterpret_cast<void*>(&ptr[i])) T;
    }

    return ptr;
  }

  /**
   * @brief Create a new object with the specified allocator and the specified parameters.
   */
  template<class T, class Alloc, class... Args>
  T*
  ge_new(Args&&... args) {
    return new (ge_alloc<Alloc>(sizeof(T))) T(std::forward<Args>(args)...);
  }

  /**
   * @brief Frees all the bytes allocated at the specified location.
   */
  template<class Alloc>
  inline void
  ge_free(void* ptr) {
    MemoryAllocator<Alloc>::free(ptr);
  }

  /**
   * @brief Destructs and frees the specified object.
   */
  template<class T, class Alloc = GenAlloc>
  inline void
  ge_delete(T* ptr) {
    (ptr)->~T();
    MemoryAllocator<Alloc>::free(ptr);
  }

  /**
   * @brief Destructs and frees the specified array of objects.
   */
  template<class T, class Alloc = GenAlloc>
  inline void
  ge_deleteN(T* ptr, SIZE_T count) {
    for (SIZE_T i = 0; i < count; ++i) {
      ptr[i].~T();
    }
    MemoryAllocator<Alloc>::freeArray(ptr, count);
  }

  /***************************************************************************/
  /**
   * Default versions of all alloc/free/new/delete methods which call GenAlloc
   */
  /***************************************************************************/

  /**
   * @brief Allocates the specified number of bytes.
   */
  inline void*
  ge_alloc(SIZE_T count) {
    return MemoryAllocator<GenAlloc>::allocate(count);
  }

  /**
   * @brief Allocates enough bytes to hold the specified type, but doesn't construct it.
   */
  template<class T>
  inline T*
  ge_alloc() {
    return reinterpret_cast<T*>(MemoryAllocator<GenAlloc>::allocate(sizeof(T)));
  }

  /**
   * @brief Creates and constructs an array of "count" elements.
   */
  template<class T>
  inline T*
  ge_newN(SIZE_T count) {
    T* ptr = reinterpret_cast<T*>(MemoryAllocator<GenAlloc>::allocateArray(sizeof(T), count));
    for (SIZE_T i = 0; i < count; ++i) {
      new (reinterpret_cast<void*>(&ptr[i])) T;
    }

    return ptr;
  }

  /**
   * @brief Create a new object with the specified allocator and the specified parameters.
   */
  template<class T, class... Args>
  T*
  ge_new(Args&&... args) {
    return new (ge_alloc<GenAlloc>(sizeof(T))) T(std::forward<Args>(args)...);
  }

  /**
   * @brief Frees all the bytes allocated at the specified location.
   */
  inline void
  ge_free(void* ptr) {
    MemoryAllocator<GenAlloc>::free(ptr);
  }

  /***************************************************************************/
  /**                     MACRO VERSIONS
   * You will almost always want to use the template versions but in some cases
   * (private destructor) it is not possible. In which case you may use these
   * instead.
   */
  /***************************************************************************/
#define GE_PVT_DELETE(T, ptr)                                                 \
      (ptr)->~T();                                                            \
      MemoryAllocator<GenAlloc>::free(ptr);

#define GE_PVT_DELETE_A(T, ptr, Alloc)                                        \
      (ptr)->~T();                                                            \
      MemoryAllocator<Alloc>::free(ptr);
}

namespace geEngineSDK {
  /**
   * @brief Allocator for the standard library that internally uses the
   *        Genesis Engine memory allocator.
   */
  template <class T, class Alloc = GenAlloc>
  class StdAlloc
  {
   public:
    typedef T value_type;
    
    StdAlloc() _NOEXCEPT {}
    
    template<class T, class Alloc>
    StdAlloc(const StdAlloc<T, Alloc>&) _NOEXCEPT {}
    
    template<class T, class Alloc>
    bool
    operator==(const StdAlloc<T, Alloc>&) const _NOEXCEPT {
      return true;
    }

    template<class T, class Alloc>
    bool
    operator!=(const StdAlloc<T, Alloc>&) const _NOEXCEPT {
      return false;
    }

    /**
     * @brief Allocate but don't initialize number elements of type T.
     */
    T*
    allocate(const size_t num) const {
      if (0 == num) {
        return nullptr;
      }

      if (num > static_cast<size_t>(-1) / sizeof(T)) {
        throw std::bad_array_new_length();
      }

      void* const pv = ge_alloc<Alloc>(num * sizeof(T));
      if (!pv) {
        throw std::bad_alloc();
      }

      return static_cast<T*>(pv);
    }

    /**
     * @brief Deallocate storage p of deleted elements.
     */
    void
    deallocate(T* p, size_t) const _NOEXCEPT {
      ge_free<Alloc>((void*)p);
    }
  };
}

#include "geMemStack.h"
#include "geGlobalFrameAlloc.h"
#include "geMemAllocProfiler.h"
