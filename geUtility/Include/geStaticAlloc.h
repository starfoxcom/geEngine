/*****************************************************************************/
/**
 * @file    geStaticAlloc.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2017/10/11
 * @brief   Static allocator
 *
 * Static allocator that attempts to perform zero heap (dynamic) allocations by
 * always keeping an active preallocated buffer. The allocator provides a fixed
 * amount of preallocated memory, and if the size of the allocated data goes
 * over that limit the allocator will fall back to dynamic heap allocations.
 *
 * This kind of allocator is only able to free all of its memory at once.
 * Freeing individual elements wont free the memory until a call to clear().
 *
 * @bug     No known bugs.
 */
/*****************************************************************************/
#pragma once

#include "gePrerequisitesUtil.h"

namespace geEngineSDK {
  /**
   * @tparam  BlockSize Size of the initially allocated static block, and minimum size of any
   *           dynamically allocated memory.
   * @tparam  MaxDynamicMemory  Maximum amount of unused memory allowed in the buffer after a
   *          call to clear(). Keeping active dynamic buffers can help prevent further memory
   *          allocations at the cost of memory. This is not relevant if you stay within the
   *          bounds of the statically allocated memory.
   */
  template<int BlockSize = 512, int MaxDynamicMemory = 512>
  class StaticAlloc
  {
   private:
    /**
     * @brief A single block of memory within a static allocator.
     */
    class MemBlock
    {
     public:
      MemBlock(uint8* data, SIZE_T size)
        : m_data(data),
        m_freePtr(nullptr),
        m_size(size),
        m_prevBlock(nullptr),
        m_nextBlock(nullptr) {}

      /**
       * @brief Allocates a piece of memory within the block.
       *        Caller must ensure the block has enough empty space.
       */
      uint8*
      alloc(SIZE_T amount) {
        uint8* freePtr = &m_data[m_freePtr];
        m_freePtr += amount;

        return freePtr;
      }

      /**
       * @brief Releases all allocations within a block but doesn't actually free the memory.
       */
      void
      clear() {
        m_freePtr = 0;
      }

      uint8* m_data;
      SIZE_T m_freePtr;
      SIZE_T m_size;
      MemBlock* m_nextBlock;
      MemBlock* m_prevBlock;
    };

   public:
    StaticAlloc()
      : m_staticBlock(m_staticData, BlockSize),
      m_freeBlock(&m_staticBlock),
      m_totalAllocBytes(0) {}

    ~StaticAlloc() {
      GE_ASSERT(m_freeBlock == &m_staticBlock && nullptr == m_staticBlock.m_freePtr);
      freeBlocks(m_freeBlock);
    }

    /**
     * @brief  Allocates a new piece of memory of the specified size.
     * @param[in]  amount  Amount of memory to allocate, in bytes.
     */
    uint8*
    alloc(SIZE_T amount) {
      if (0 == amount) {
        return nullptr;
      }

#if GE_DEBUG_MODE
      amount += sizeof(SIZE_T);
#endif

      SIZE_T freeMem = m_freeBlock->m_size - m_freeBlock->m_freePtr;
      if (amount > freeMem) {
        allocBlock(amount);
      }

      uint8* data = m_freeBlock->alloc(amount);

#if GE_DEBUG_MODE
      m_totalAllocBytes += amount;

      SIZE_T* storedSize = reinterpret_cast<SIZE_T*>(data);
      *storedSize = amount;

      return data + sizeof(SIZE_T);
#else
      return data;
#endif
    }

    /**
     * @brief Deallocates a previously allocated piece of memory.
     */
    void
    free(void* data) {
      if (nullptr == data) {
        return;
      }

      //Dealloc is only used for debug and can be removed if needed.
      //All the actual deallocation happens in clear()

#if GE_DEBUG_MODE
      uint8* dataPtr = (uint8*)data;
      dataPtr -= sizeof(SIZE_T);

      SIZE_T* storedSize = (SIZE_T*)(dataPtr);
      m_totalAllocBytes -= *storedSize;
#endif
    }

    /**
     * @brief Allocates enough memory to hold the object(s) of specified type
     *        using the static allocator, and constructs them.
     */
    template<class T>
    T*
    construct(SIZE_T count = 0) {
      T* data = reinterpret_cast<T*>(alloc(sizeof(T) * count));

      for (SIZE_T i = 0; i < count; ++i) {
        new (reinterpret_cast<void*>(&data[i])) T;
      }

      return data;
    }

    /**
    * @brief Allocates enough memory to hold the object(s) of specified type
    *        using the static allocator, and constructs them.
    */
    template<class T, class... Args>
    T*
    construct(Args &&...args, SIZE_T count = 0) {
      T* data = reinterpret_cast<T*>(alloc(sizeof(T) * count));

      for (SIZE_T i = 0; i < count; ++i) {
        new (reinterpret_cast<void*>(&data[i])) T(std::forward<Args>(args)...);
      }

      return data;
    }

    /**
     * @brief Destructs and deallocates an object allocated with the static allocator.
     */
    template<class T>
    void
    destruct(T* data) {
      data->~T();
      free(data);
    }

    /**
     * @brief Destructs and deallocates an array of objects allocated with the
     *        static frame allocator.
     */
    template<class T>
    void
    destruct(T* data, SIZE_T count) {
      for (SIZE_T i = 0; i < count; ++i) {
        data[i].~T();
      }
      free(data);
    }

    /**
     * @brief Frees the internal memory buffers. All external allocations must
     *        be freed before calling this.
     */
    void
    clear() {
      GE_ASSERT(0 == m_totalAllocBytes);

      MemBlock* dynamicBlock = m_staticBlock.m_nextBlock;
      int32 totalDynamicMemAmount = 0;
      uint32 numDynamicBlocks = 0;

      while (nullptr != dynamicBlock) {
        totalDynamicMemAmount += dynamicBlock->m_freePtr;
        dynamicBlock->clear();

        dynamicBlock = dynamicBlock->m_nextBlock;
        ++numDynamicBlocks;
      }

      m_freeBlock = &m_staticBlock;
      m_staticBlock.clear();

      if (1 < numDynamicBlocks) {
        freeBlocks(&m_staticBlock);
        allocBlock(std::min(totalDynamicMemAmount, MaxDynamicMemory));
        m_freeBlock = &m_staticBlock;
      }
      else if (1 == numDynamicBlocks && 0 == MaxDynamicMemory) {
        freeBlocks(&m_staticBlock);
      }
    }

   private:
    uint8 m_staticData[BlockSize];
    MemBlock m_staticBlock;

    MemBlock* m_freeBlock;
    SIZE_T m_totalAllocBytes;

    /**
    * @brief Allocates a dynamic block of memory of the wanted size.
    *        The exact allocation size might be slightly higher in order to
    *        store block meta data.
    */
    MemBlock*
    allocBlock(SIZE_T wantedSize) {
      SIZE_T blockSize = BlockSize;
      if (wantedSize > blockSize) {
        blockSize = wantedSize;
      }

      MemBlock* dynamicBlock = m_freeBlock->m_nextBlock;
      MemBlock* newBlock = nullptr;
      while (nullptr != dynamicBlock) {
        if (dynamicBlock->m_size >= blockSize) {
          newBlock = dynamicBlock;
          break;
        }

        dynamicBlock = dynamicBlock->m_nextBlock;
      }

      if (nullptr == newBlock) {
        uint8* data = reinterpret_cast<uint8*>(ge_alloc(blockSize + sizeof(MemBlock)));
        newBlock = new (data)MemBlock(data + sizeof(MemBlock), blockSize);
        newBlock->m_prevBlock = m_freeBlock;
        m_freeBlock->m_nextBlock = newBlock;
      }

      m_freeBlock = newBlock;
      return newBlock;
    }

    /**
     * @brief Releases memory for any dynamic blocks following the provided block
     *        (if there are any).
     */
    void
    freeBlocks(MemBlock* start) {
      MemBlock* dynamicBlock = start->m_nextBlock;
      while (nullptr != dynamicBlock) {
        MemBlock* nextBlock = dynamicBlock->m_nextBlock;
        dynamicBlock->~MemBlock();
        ge_free(dynamicBlock);
        dynamicBlock = nextBlock;
      }

      start->m_nextBlock = nullptr;
    }
  };
}
