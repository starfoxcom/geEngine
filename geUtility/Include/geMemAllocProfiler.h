/*****************************************************************************/
/**
* @file   geMemAllocProfiler.h
* @author Samuel Prince (samuel.prince.quezada@gmail.com)
* @date   2015/02/16
* @brief  Specialized allocator for the profiler.
*
* Specialized allocator for profiler so we can avoid tracking
* internal profiler memory allocations which would skew profiler
* results.
*
* @bug	   No known bugs.
*/
/*****************************************************************************/
#pragma once

namespace geEngineSDK {
  /**
  * @brief	Specialized allocator for profiler so we can avoid tracking internal profiler memory allocations
  * 			which would skew profiler results.
  */
  class ProfilerAlloc
  {};

  /**
  * @brief	Memory allocator providing a generic implementation. Specialize for specific categories as needed.
  */
  template<>
  class MemoryAllocator<ProfilerAlloc> : public MemoryAllocatorBase
  {
  public:
    /**
    * @brief	Allocates the given number of bytes.
    */
    static inline void* Allocate(SIZE_T bytes) {
      return malloc(bytes);
    }

    /**
    * @brief	Allocates the given a number of objects, each of the given number of bytes.
    */
    static inline void* AllocateArray(SIZE_T bytes, uint32 count) {
      return malloc(bytes * count);
    }

    /**
    * @brief	Frees memory previously allocated with "Allocate".
    */
    static inline void Free(void* ptr) {
      ::free(ptr);
    }

    /**
    * @brief	Frees memory previously allocated with "FreeArray". "count" must match the
    * 			original value when array was allocated.
    */
    static inline void FreeArray(void* ptr, uint32) {
      ::free(ptr);
    }
  };
}
