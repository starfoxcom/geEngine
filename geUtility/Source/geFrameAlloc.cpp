/********************************************************************/
/**
 * @file   geFrameAlloc.cpp
 * @author Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date   2016/03/06
 * @brief  Frame allocator
 *
 * Performs very fast allocations but can only free all of its
 * memory at once. Perfect for allocations that last just a single
 * frame.
 *
 * @bug	   No known bugs.
 */
/********************************************************************/

/************************************************************************************************************************/
/* Includes																												*/
/************************************************************************************************************************/
#include "geFrameAlloc.h"
#include "geException.h"

namespace geEngineSDK
{
	FrameAlloc::MemBlock::MemBlock(uint32 size) : m_Data(nullptr), m_FreePtr(0), m_Size(size)
	{
	}

	FrameAlloc::MemBlock::~MemBlock()
	{
	}

	uint8* FrameAlloc::MemBlock::Alloc(uint32 amount)
	{
		uint8* freePtr = &m_Data[m_FreePtr];
		m_FreePtr += amount;

		return freePtr;
	}

	void FrameAlloc::MemBlock::Clear()
	{
		m_FreePtr = 0;
	}

#if GE_DEBUG_MODE
	FrameAlloc::FrameAlloc(uint32 blockSize)
		: m_BlockSize(blockSize), m_FreeBlock(nullptr), m_NextBlockIdx(0), m_TotalAllocBytes(0), m_LastFrame(nullptr), m_OwnerThread(GE_THREAD_CURRENT_ID)
	{
		AllocBlock(m_BlockSize);
	}
#else
	FrameAlloc::FrameAlloc(uint32 blockSize)
		: m_BlockSize(blockSize), m_FreeBlock(nullptr), m_NextBlockIdx(0), m_TotalAllocBytes(0), m_LastFrame(nullptr)
	{
		AllocBlock(m_BlockSize);
	}
#endif

	FrameAlloc::~FrameAlloc()
	{
		for(auto& block : m_Blocks)
		{
			DeallocBlock(block);
		}
	}

	uint8* FrameAlloc::Alloc(uint32 amount)
	{
#if GE_DEBUG_MODE
		GE_ASSERT(m_OwnerThread == GE_THREAD_CURRENT_ID && "Frame allocator called from invalid thread.");
		amount += sizeof(uint32);
#endif
		uint32 freeMem = m_FreeBlock->m_Size - m_FreeBlock->m_FreePtr;
		if( amount > freeMem )
		{
			AllocBlock(amount);
		}

		uint8* data = m_FreeBlock->Alloc(amount);

#if GE_DEBUG_MODE
		m_TotalAllocBytes += amount;

		uint32* storedSize = reinterpret_cast<uint32*>(data);
		*storedSize = amount;

		return data + sizeof(uint32);
#else
		return data;
#endif
	}

	void FrameAlloc::Dealloc(uint8* data)
	{
#if GE_DEBUG_MODE
		data -= sizeof(uint32);
		uint32* storedSize = reinterpret_cast<uint32*>(data);
		m_TotalAllocBytes -= *storedSize;
#else
    GE_UNREFERENCED_PARAMETER(data);
#endif
	}

	void FrameAlloc::MarkFrame()
	{
		uint32** framePtr = (uint32**)Alloc( sizeof(uint32*) );
		*framePtr = m_LastFrame;
		m_LastFrame = *(uint32**)framePtr;
	}

	void FrameAlloc::Clear()
	{
#if GE_DEBUG_MODE
		GE_ASSERT(m_OwnerThread == GE_THREAD_CURRENT_ID && "Frame allocator called from invalid thread.");
#endif
		if( m_LastFrame != nullptr )
		{
			GE_ASSERT(m_Blocks.size() > 0 && m_NextBlockIdx > 0);
			Dealloc(m_LastFrame);	//HACK: Test if this casting is working correctly on PS4

			uint8* framePtr = (uint8*)m_LastFrame;
			m_LastFrame = *(uint32**)m_LastFrame;

#if GE_DEBUG_MODE
			framePtr -= sizeof(uint32);
#endif
			uint32 startBlockIdx = m_NextBlockIdx - 1;
			uint32 numFreedBlocks = 0;
			for( int32 i=startBlockIdx; i>=0; i-- )
			{
				MemBlock* curBlock = m_Blocks[i];
				uint8* blockEnd = curBlock->m_Data + curBlock->m_Size;
				if( framePtr>=curBlock->m_Data && framePtr < blockEnd )
				{
					uint8* dataEnd = curBlock->m_Data + curBlock->m_FreePtr;
					uint32 sizeInBlock = (uint32)(dataEnd - framePtr);
					GE_ASSERT( sizeInBlock <= curBlock->m_FreePtr );

					curBlock->m_FreePtr -= sizeInBlock;
					if( curBlock->m_FreePtr == 0 )
					{
						numFreedBlocks++;

						//Reset block counter if we're gonna reallocate this one
						if( numFreedBlocks > 1 )
						{
							m_NextBlockIdx = i;
						}
					}

					break;
				}
				else
				{
					curBlock->m_FreePtr = 0;
					m_NextBlockIdx = i;
					numFreedBlocks++;
				}
			}

			if( numFreedBlocks > 1 )
			{
				uint32 totalBytes = 0;
				for( uint32 i=0; i<numFreedBlocks; ++i )
				{
					MemBlock* curBlock = m_Blocks[m_NextBlockIdx];
					totalBytes += curBlock->m_Size;

					DeallocBlock(curBlock);
					m_Blocks.erase(m_Blocks.begin() + m_NextBlockIdx);
				}

				uint32 oldNextBlockIdx = m_NextBlockIdx;
				AllocBlock(totalBytes);

				//Point to the first non-full block, or if none available then point the the block we just allocated
				if( oldNextBlockIdx > 0 )
				{
					m_FreeBlock = m_Blocks[oldNextBlockIdx - 1];
				}
			}
			else
			{
				m_FreeBlock = m_Blocks[m_NextBlockIdx - 1];
			}
		}
		else
		{
#if GE_DEBUG_MODE
			if( m_TotalAllocBytes.load() > 0 )
			{
				GE_EXCEPT(InvalidStateException, "Not all frame allocated bytes were properly released.");
			}
#endif
			if( m_Blocks.size() > 1 )
			{
				//Merge all blocks into one
				uint32 totalBytes = 0;
				for( auto& block : m_Blocks )
				{
					totalBytes += block->m_Size;
					DeallocBlock(block);
				}

				m_Blocks.clear();
				m_NextBlockIdx = 0;

				AllocBlock(totalBytes);
			}
		}
	}

	FrameAlloc::MemBlock* FrameAlloc::AllocBlock(uint32 wantedSize)
	{
		uint32 blockSize = m_BlockSize;
		if( wantedSize > blockSize )
		{
			blockSize = wantedSize;
		}

		MemBlock* newBlock = nullptr;
		while( m_NextBlockIdx < m_Blocks.size() )
		{
			MemBlock* curBlock = m_Blocks[m_NextBlockIdx];
			if( blockSize <= curBlock->m_Size )
			{
				newBlock = curBlock;
				m_NextBlockIdx++;
				break;
			}
			else
			{
				//Found an empty block that doesn't fit our data, delete it
				DeallocBlock(curBlock);
				m_Blocks.erase(m_Blocks.begin() + m_NextBlockIdx);
			}
		}

		if( newBlock == nullptr )
		{
			uint8* data = (uint8*)reinterpret_cast<uint8*>(ge_alloc(blockSize + sizeof(MemBlock)));
			newBlock = new (data) MemBlock(blockSize);
			data += sizeof(MemBlock);
			newBlock->m_Data = data;

			m_Blocks.push_back(newBlock);
			m_NextBlockIdx++;
		}

		m_FreeBlock = newBlock; //If previous block had some empty space it is lost until next "clear"

		return newBlock;
	}

	void FrameAlloc::DeallocBlock(MemBlock* block)
	{
		block->~MemBlock();
		ge_free(block);
	}

	void FrameAlloc::SetOwnerThread(GE_THREAD_ID_TYPE thread)
	{
#if GE_DEBUG_MODE
		m_OwnerThread = thread;
#else
    GE_UNREFERENCED_PARAMETER(thread);
#endif
	}
}