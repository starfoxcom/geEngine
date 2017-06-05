/*****************************************************************************/
/**
 * @file    geFrameAlloc.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2016/03/06
 * @brief   Frame allocator
 *
 * Performs very fast allocations but can only free all of its memory at once.
 * Perfect for allocations that last just a single frame.
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
#include <limits>
#include <new>

#include "gePrerequisitesUtil.h"

namespace geEngineSDK {
  /**
   * @brief Frame allocator. Performs very fast allocations but can only free all of its
   *        memory at once. Perfect for allocations that last just a single frame.
   * @note  Not thread safe with an exception. alloc() and clear() methods need to be called
   *        from the same thread. Dealloc() is thread safe and can be called from any thread.
   */
  class GE_UTILITY_EXPORT FrameAlloc
  {
   private:
    /**
     * @brief A single block of memory within a frame allocator.
     */
    class MemBlock
    {
     public:
      explicit MemBlock(SIZE_T size);
      ~MemBlock();

      /**
       * @brief Allocates a piece of memory within the block. Caller must
       *        ensure the block has enough empty space.
       */
      uint8*
      alloc(SIZE_T amount);

      /**
       * @brief Releases all allocations within a block but doesn't actually free the memory.
       */
      void
      clear();

     public:
      uint8* m_data;
      SIZE_T m_freePtr;
      SIZE_T m_size;
    };

   public:
    explicit FrameAlloc(SIZE_T blockSize = 1024 * 1024);
    ~FrameAlloc();

    /**
     * @brief Allocates a new block of memory of the specified size.
     * @param[in] amount  Amount of memory to allocate, in bytes.
     * @note  Not thread safe.
     */
    uint8*
    alloc(SIZE_T amount);

    /**
     * @brief Allocates and constructs a new object.
     * @note  Not thread safe.
     */
    template<class T, class... Args>
    T*
    alloc(Args&& ...args) {
      return new (reinterpret_cast<T*>(alloc(sizeof(T)))) T(std::forward<Args>(args)...);
    }

    /**
     * @brief Allocates a new block of memory of the specified size aligned to
     *        the specified boundary. If the alignment is less or equal to 16 it is
              more efficient to use the allocAligned16() alternative of this method.
     * @param[in] amount  Amount of memory to allocate, in bytes.
     * @param[in] alignment Alignment of the allocated memory. Must be power of two.
     * @note  Not thread safe.
     */
    uint8* allocAligned(SIZE_T amount, SIZE_T alignment);

    /**
     * @brief Deallocates a previously allocated block of memory.
     * @note  No deallocation is actually done here. This method is only used
     *        for debug purposes so it is easier to track down memory leaks and
     *        corruption.
     * @note  Thread safe.
     */
    void
    dealloc(uint8* data);

    /**
     * @brief Deallocates and destructs a previously allocated object.
     * @note  No deallocation is actually done here. This method is only used
     *        to call the destructor and for debug purposes so it is easier to 
     *        track down memory leaks and corruption.
     * @note  Thread safe.
     */
    template<class T>
    void
    dealloc(T* obj) {
      if (nullptr != obj) {
        obj->~T();
      }
      dealloc(reinterpret_cast<uint8*>(obj));
    }

    /**
     * @brief Starts a new frame. Next call to ::Clear will only clear memory
     *        allocated past this point.
     */
    void
    markFrame();

    /**
     * @brief Deallocates all allocated memory since the last call to markFrame()
     *        (or all the memory if there was no call to markFrame()).
     * @note  Not thread safe.
     */
    void
    clear();

    /**
     * @brief Changes the frame allocator owner thread. After the owner thread
     *        has changed only allocations from that thread can be made.
     */
    void
    setOwnerThread(ThreadId thread);

    /**
     * @brief Allocates a dynamic block of memory of the wanted size. The exact
     *        allocation size might be slightly higher in order to store block
     *        meta data.
     */
    MemBlock*
    allocBlock(SIZE_T wantedSize);

    /**
     * @brief Frees a memory block.
     */
    void
    deallocBlock(MemBlock* block);

   private:
    SIZE_T m_blockSize;
    Vector<MemBlock*> m_blocks;
    MemBlock* m_freeBlock;
    uint32 m_nextBlockIdx;
    std::atomic<SIZE_T> m_totalAllocBytes;
    uint32* m_lastFrame;

#if GE_DEBUG_MODE
    ThreadId m_ownerThread;
#endif
  };

  /**
   * @brief Allocator for the standard library that internally uses a frame allocator.
   */
  template <class T>
  class StdFrameAlloc
  {
   public:
    typedef T value_type;
    typedef value_type* pointer;
    typedef const value_type* const_pointer;
    typedef value_type& reference;
    typedef const value_type& const_reference;
    typedef std::size_t size_type;
    typedef std::ptrdiff_t difference_type;
    
    StdFrameAlloc() _NOEXCEPT : m_FrameAlloc(nullptr) {}
    explicit StdFrameAlloc(FrameAlloc* pAlloc) _NOEXCEPT : m_FrameAlloc(pAlloc) {}

    template<class U>
    StdFrameAlloc(const StdFrameAlloc<U>& refAlloc) _NOEXCEPT
      : m_FrameAlloc(refAlloc.m_FrameAlloc) {
    }

    /**
     * @brief Copy operator, added for completion, but probably should never be used.
     */
    template<class U>
    const StdFrameAlloc<U>&
    operator=(const StdFrameAlloc<U>& refAlloc) {
      m_FrameAlloc = refAlloc.m_FrameAlloc;
    }

    template<class U>
    bool
    operator==(const StdFrameAlloc<U>&) const _NOEXCEPT {
      return true;
    }

    template<class U>
    bool
    operator!=(const StdFrameAlloc<U>&) const _NOEXCEPT {
      return false;
    }

    template<class U>
    class rebind
    {
     public: typedef StdFrameAlloc<U> other;
    };

    /**
     * @brief Allocate but don't initialize number elements of type T.
     */
    T*
    allocate(const size_t num) const {
      if (0 == num) {
        return nullptr;
      }

      if (static_cast<size_t>(-1) / sizeof(T) < num) {
        throw std::bad_array_new_length();
      }

      void* const pv = m_FrameAlloc->alloc(static_cast<uint32>(num * sizeof(T)));
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
      m_FrameAlloc->dealloc(reinterpret_cast<uint8*>(p));
    }

   public:
    FrameAlloc* m_FrameAlloc;

    size_t
    max_size() const {
      return std::numeric_limits<size_type>::max() / sizeof(T);
    }

    void
    construct(pointer p, const_reference t) {
      new (p) T(t);
    }
    
    void
    destroy(pointer p) {
      p->~T();
    }

    template<class U, class... Args>
    void
    construct(U* p, Args&& ...args) {
      new(p) U(std::forward<Args>(args)...);
    }
  };

  /**
   * @brief Return that all specializations of this allocator are interchangeable.
   */
  template <class T1, class T2>
  bool
  operator==(const StdFrameAlloc<T1>&, const StdFrameAlloc<T2>&) throw() {
    return true;
  }

  /**
   * @brief Return that all specializations of this allocator are interchangeable.
   */
  template <class T1, class T2>
  bool
  operator!=(const StdFrameAlloc<T1>&, const StdFrameAlloc<T2>&) throw() {
    return false;
  }
}
