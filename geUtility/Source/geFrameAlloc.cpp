/*****************************************************************************/
/**
 * @file    geFrameAlloc.cpp
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

/*****************************************************************************/
/**
 * Includes
 */
/*****************************************************************************/
#include "geFrameAlloc.h"
#include "geException.h"

namespace geEngineSDK {
  FrameAlloc::MemBlock::MemBlock(SIZE_T size) 
    : m_data(nullptr),
      m_freePtr(0),
      m_size(size)
  {}

  FrameAlloc::MemBlock::~MemBlock() {}

  uint8*
  FrameAlloc::MemBlock::alloc(SIZE_T amount) {
    uint8* freePtr = &m_data[m_freePtr];
    m_freePtr += amount;
    return freePtr;
  }

  void
  FrameAlloc::MemBlock::clear() {
    m_freePtr = 0;
  }

#if GE_DEBUG_MODE
  FrameAlloc::FrameAlloc(SIZE_T blockSize)
    : m_blockSize(blockSize),
      m_freeBlock(nullptr),
      m_nextBlockIdx(0),
     m_totalAllocBytes(0),
     m_lastFrame(nullptr),
     m_ownerThread(GE_THREAD_CURRENT_ID) {
    allocBlock(m_blockSize);
  }
#else
  FrameAlloc::FrameAlloc(uint32 blockSize)
    : m_blockSize(blockSize),
      m_freeBlock(nullptr),
      m_nextBlockIdx(0),
      m_totalAllocBytes(0),
      m_lastFrame(nullptr) {
    allocBlock(m_blockSize);
  }
#endif

  FrameAlloc::~FrameAlloc() {
    for (auto& block : m_blocks) {
      deallocBlock(block);
    }
  }

  uint8*
  FrameAlloc::alloc(SIZE_T amount) {
#if GE_DEBUG_MODE
    GE_ASSERT(GE_THREAD_CURRENT_ID == m_ownerThread &&
              "Frame allocator called from invalid thread.");
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

  void
  FrameAlloc::dealloc(uint8* data) {
#if GE_DEBUG_MODE
    data -= sizeof(SIZE_T);
    SIZE_T* storedSize = reinterpret_cast<SIZE_T*>(data);
    m_totalAllocBytes -= *storedSize;
#else
    GE_UNREFERENCED_PARAMETER(data);
#endif
  }

  void
  FrameAlloc::markFrame() {
    uint32** framePtr = reinterpret_cast<uint32**>(alloc(sizeof(uint32*)));
    *framePtr = m_lastFrame;
    m_lastFrame = *(uint32**)framePtr;
  }

  void
  FrameAlloc::clear() {
#if GE_DEBUG_MODE
    GE_ASSERT(GE_THREAD_CURRENT_ID == m_ownerThread &&
              "Frame allocator called from invalid thread.");
#endif
    if (nullptr != m_lastFrame) {
      GE_ASSERT(m_blocks.size() > 0 && 0 < m_nextBlockIdx);
      dealloc(m_lastFrame);	//HACK: Test if this casting is working correctly on PS4

      uint8* framePtr = (uint8*)m_lastFrame;
      m_lastFrame = *(uint32**)m_lastFrame;

#if GE_DEBUG_MODE
      framePtr -= sizeof(uint32);
#endif
      uint32 startBlockIdx = m_nextBlockIdx - 1;
      uint32 numFreedBlocks = 0;
      for (int32 i = startBlockIdx; i >= 0; --i) {
        MemBlock* curBlock = m_blocks[i];
        uint8* blockEnd = curBlock->m_data + curBlock->m_size;
        if (framePtr >= curBlock->m_data && framePtr < blockEnd) {
          uint8* dataEnd = curBlock->m_data + curBlock->m_freePtr;
          SIZE_T sizeInBlock = (SIZE_T)(dataEnd - framePtr);
          GE_ASSERT(sizeInBlock <= curBlock->m_freePtr);

          curBlock->m_freePtr -= sizeInBlock;
          if (0 == curBlock->m_freePtr) {
            numFreedBlocks++;

            //Reset block counter if we're gonna reallocate this one
            if (1 < numFreedBlocks) {
              m_nextBlockIdx = i;
            }
          }

          break;
        }
        else {
          curBlock->m_freePtr = 0;
          m_nextBlockIdx = i;
          numFreedBlocks++;
        }
      }

      if (1 < numFreedBlocks) {
        SIZE_T totalBytes = 0;
        for (uint32 i = 0; i<numFreedBlocks; ++i) {
          MemBlock* curBlock = m_blocks[m_nextBlockIdx];
          totalBytes += curBlock->m_size;

          deallocBlock(curBlock);
          m_blocks.erase(m_blocks.begin() + m_nextBlockIdx);
        }

        uint32 oldNextBlockIdx = m_nextBlockIdx;
        allocBlock(totalBytes);

        /** Point to the first non-full block, or if none available then point
         the the block we just allocated */
        if (0 < oldNextBlockIdx) {
          m_freeBlock = m_blocks[oldNextBlockIdx - 1];
        }
      }
      else {
        m_freeBlock = m_blocks[m_nextBlockIdx - 1];
      }
    }
    else {
#if GE_DEBUG_MODE
      if (m_totalAllocBytes.load() > 0) {
        GE_EXCEPT(InvalidStateException,
                  "Not all frame allocated bytes were properly released.");
      }
#endif
      if (m_blocks.size() > 1) {
        //Merge all blocks into one
        SIZE_T totalBytes = 0;
        for (auto& block : m_blocks) {
          totalBytes += block->m_size;
          deallocBlock(block);
        }

        m_blocks.clear();
        m_nextBlockIdx = 0;

        allocBlock(totalBytes);
      }
    }
  }

  FrameAlloc::MemBlock*
  FrameAlloc::allocBlock(SIZE_T wantedSize) {
    SIZE_T blockSize = m_blockSize;
    if (wantedSize > blockSize) {
      blockSize = wantedSize;
    }

    MemBlock* newBlock = nullptr;
    while (m_nextBlockIdx < m_blocks.size()) {
      MemBlock* curBlock = m_blocks[m_nextBlockIdx];
      if (blockSize <= curBlock->m_size) {
        newBlock = curBlock;
        m_nextBlockIdx++;
        break;
      }
      else {
        //Found an empty block that doesn't fit our data, delete it
        deallocBlock(curBlock);
        m_blocks.erase(m_blocks.begin() + m_nextBlockIdx);
      }
    }

    if (nullptr == newBlock) {
      uint8* data = (uint8*)reinterpret_cast<uint8*>(ge_alloc(blockSize + sizeof(MemBlock)));
      newBlock = new (data) MemBlock(blockSize);
      data += sizeof(MemBlock);
      newBlock->m_data = data;

      m_blocks.push_back(newBlock);
      m_nextBlockIdx++;
    }

    //If previous block had some empty space it is lost until next "clear"
    m_freeBlock = newBlock;

    return newBlock;
  }

  void
  FrameAlloc::deallocBlock(MemBlock* block) {
    block->~MemBlock();
    ge_free(block);
  }

  void
  FrameAlloc::setOwnerThread(GE_THREAD_ID_TYPE thread) {
#if GE_DEBUG_MODE
    m_ownerThread = thread;
#else
    GE_UNREFERENCED_PARAMETER(thread);
#endif
  }
}
