/********************************************************************/
/**
 * @file   geMemStack.h
 * @author Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date   2015/02/16
 * @brief  Classes used to create our own Memory Stacks
 *
 * This file defines all the needed classes to create a Memory Stack.
 * Normally it's used in combination with STL objects.
 *
 * @bug	   No known bugs.
 */
/********************************************************************/
#pragma once

/************************************************************************************************************************/
/* Includes                                                                     										*/
/************************************************************************************************************************/
#include <stack>
#include <assert.h>
#include "geStdHeaders.h"
#include "geThreadDefines.h"

namespace geEngineSDK
{
	/************************************************************************************************************************/
	/**
	* @brief	Describes a memory stack of a certain block capacity. See "MemoryStack" for more information.
	*
	* @tparam	BlockCapacity Minimum size of a block. Larger blocks mean less memory allocations, but also potentially
	* 			more wasted memory. If an allocation requests more bytes than BlockCapacity, first largest multiple is
	* 			used instead.
	*/
	/************************************************************************************************************************/
	template <int BlockCapacity = 1024 * 1024>	//Default to 1.0 MB
	class MemStackInternal
	{
	private:
		/************************************************************************************************************************/
		/**
		* @brief	A single block of memory of "BlockCapacity" size. A pointer to the first free address is stored, and a 
		*			remaining size.
		*/
		/************************************************************************************************************************/
		class MemBlock
		{
		public:
			uint8* m_Data;
			uint32 m_FreePtr;
			uint32 m_Size;
			MemBlock* m_NextBlock;
			MemBlock* m_PrevBlock;

		public:
			MemBlock(uint32 size) : m_Data(nullptr), m_FreePtr(0), m_Size(size), m_NextBlock(nullptr), m_PrevBlock(nullptr)
			{}

			~MemBlock()
			{}

			/************************************************************************************************************************/
			/**
			* @brief	Returns the first free address and increments the free pointer.
			* 			Caller needs to ensure the remaining block size is adequate before calling.
			*/
			/************************************************************************************************************************/
			uint8* Alloc(uint32 amount)
			{
				uint8* freePtr = &m_Data[m_FreePtr];
				m_FreePtr += amount;

				return freePtr;
			}

			/************************************************************************************************************************/
			/**
			* @brief	Deallocates the provided pointer. Deallocation must happen in opposite order from allocation otherwise
			*			corruption will occur.
			*
			* @note		Pointer to "data" isn't actually needed, but is provided for debug purposes in order to more easily track
			*			out-of-order deallocations.
			*/
			/************************************************************************************************************************/
			void Dealloc(uint8* data, uint32 amount)
			{
				m_FreePtr -= amount;
				GE_ASSERT( (&m_Data[m_FreePtr]) == data && "Out of order stack deallocation detected. Deallocations need to happen in order opposite of allocations." );
#if !GE_DEBUG_MODE
        GE_UNREFERENCED_PARAMETER(data);
#endif
			}
		};

	private:
		MemBlock* m_FreeBlock;

	public:
		MemStackInternal() : m_FreeBlock(nullptr)
		{
			m_FreeBlock = AllocBlock(BlockCapacity);
		}

		~MemStackInternal()
		{
			GE_ASSERT( m_FreeBlock->m_FreePtr == 0 && "Not all blocks were released before shutting down the stack allocator.");

			MemBlock* curBlock = m_FreeBlock;
			while( curBlock != nullptr )
			{
				MemBlock* nextBlock = curBlock->m_NextBlock;
				DeallocBlock(curBlock);

				curBlock = nextBlock;
			}
		}

		/************************************************************************************************************************/
		/**
		* @brief	Allocates the given amount of memory on the stack.
		*
		* @param	amount	The amount to allocate in bytes.
		*
		* @note		Allocates the memory in the currently active block if it is large enough, otherwise a new block is allocated.
		*			If the allocation is larger than default block size a separate block will be allocated only for that
		*			allocation, making it essentially a slower heap allocator.
		*
		*			Each allocation comes with a 4 byte overhead.
		*/
		/************************************************************************************************************************/
		uint8* Alloc(uint32 amount)
		{
			amount += sizeof(uint32);

			uint32 freeMem = m_FreeBlock->m_Size - m_FreeBlock->m_FreePtr;
			if( amount > freeMem )
			{
				AllocBlock(amount);
			}

			uint8* data = m_FreeBlock->Alloc(amount);

			uint32* storedSize = reinterpret_cast<uint32*>(data);
			*storedSize = amount;

			return data + sizeof(uint32);
		}

		/************************************************************************************************************************/
		/**
		* @brief	Deallocates the given memory. Data must be deallocated in opposite order then when it was allocated.
		*/
		/************************************************************************************************************************/
		void Dealloc(uint8* data)
		{
			data -= sizeof(uint32);

			uint32* storedSize = reinterpret_cast<uint32*>(data);
			m_FreeBlock->Dealloc(data, *storedSize);

			if( m_FreeBlock->m_FreePtr == 0 )
			{
				MemBlock* emptyBlock = m_FreeBlock;

				if( emptyBlock->m_PrevBlock != nullptr )
				{
					m_FreeBlock = emptyBlock->m_PrevBlock;
				}

				//Merge with next block
				if( emptyBlock->m_NextBlock != nullptr )
				{
					uint32 totalSize = emptyBlock->m_Size + emptyBlock->m_NextBlock->m_Size;

					if( emptyBlock->m_PrevBlock != nullptr )
					{
						emptyBlock->m_PrevBlock->m_NextBlock = nullptr;
					}
					else
					{
						m_FreeBlock = nullptr;
					}

					DeallocBlock(emptyBlock->m_NextBlock);
					DeallocBlock(emptyBlock);

					AllocBlock(totalSize);
				}
			}
		}

	private:

		/************************************************************************************************************************/
		/**
		* @brief	Allocates a new block of memory using a heap allocator. Block will never be smaller than "BlockCapacity" 
		*			no matter the "wantedSize".
		*/
		/************************************************************************************************************************/
		MemBlock* AllocBlock(uint32 wantedSize)
		{
			uint32 blockSize = BlockCapacity;
			if( wantedSize > blockSize )
			{
				blockSize = wantedSize;
			}

			MemBlock* newBlock = nullptr;
			MemBlock* curBlock = m_FreeBlock;

			while( curBlock != nullptr )
			{
				MemBlock* nextBlock = curBlock->m_NextBlock;
				if( nextBlock != nullptr && nextBlock->m_Size >= blockSize )
				{
					newBlock = nextBlock;
					break;
				}

				curBlock = nextBlock;
			}

			if( newBlock == nullptr )
			{
				uint8* data = (uint8*)reinterpret_cast<uint8*>(ge_alloc(blockSize + sizeof(MemBlock)));
				newBlock = new (data)MemBlock(blockSize);
				data += sizeof(MemBlock);

				newBlock->m_Data = data;
				newBlock->m_PrevBlock = m_FreeBlock;

				if( m_FreeBlock != nullptr )
				{
					if( m_FreeBlock->m_NextBlock != nullptr)
					{
						m_FreeBlock->m_NextBlock->m_PrevBlock = newBlock;
					}

					newBlock->m_NextBlock = m_FreeBlock->m_NextBlock;
					m_FreeBlock->m_NextBlock = newBlock;
				}
			}

			m_FreeBlock = newBlock;
			return newBlock;
		}

		/************************************************************************************************************************/
		/**
		* @brief	Deallocates a block of memory.
		*/
		/************************************************************************************************************************/
		void DeallocBlock(MemBlock* block)
		{
			block->~MemBlock();
			ge_free(block);
		}
	};

	/************************************************************************************************************************/
	/**
	* @brief	One of the fastest, but also very limiting type of allocator. All deallocations must happen in opposite
	*			order from allocations.
	*
	* @note		It's mostly useful when you need to allocate something temporarily on the heap, usually something that gets
	*			allocated and freed within the same method.
	*
	*			Each allocation comes with a pretty hefty 4 byte memory overhead, so don't use it for small allocations.
	*
	*			Thread safe. But you cannot allocate on one thread and deallocate on another. Threads will keep
	*			separate stacks internally. Make sure to call BeginThread/EndThread for any thread this stack is used on.
	*/
	/************************************************************************************************************************/
	class MemStack
	{
	public:
		/************************************************************************************************************************/
		/**
		* @brief	Sets up the stack with the currently active thread. You need to call this on any thread before doing any
		*			allocations or deallocations
		*/
		/************************************************************************************************************************/
		static GE_UTILITY_EXPORT void BeginThread();

		/************************************************************************************************************************/
		/**
		* @brief	Cleans up the stack for the current thread. You may not perform any allocations or deallocations after this
		*			is called, unless you call BeginThread again.
		*/
		/************************************************************************************************************************/
		static GE_UTILITY_EXPORT void EndThread();

		/************************************************************************************************************************/
		/**
		* @copydoc	MemoryStackInternal::Alloc
		*/
		/************************************************************************************************************************/
		static GE_UTILITY_EXPORT uint8* Alloc(uint32 numBytes);

		/************************************************************************************************************************/
		/**
		* @copydoc	MemoryStackInternal::Dealloc
		*/
		/************************************************************************************************************************/
		static GE_UTILITY_EXPORT void DeallocLast(uint8* data);

	private:
		static GE_THREADLOCAL MemStackInternal<1024 * 1024>* ThreadMemStack;
	};

	/************************************************************************************************************************/
	/**
	* @copydoc	MemoryStackInternal::Alloc
	*/
	/************************************************************************************************************************/
	GE_UTILITY_EXPORT void* ge_stack_alloc(uint32 numBytes);
	
	/************************************************************************************************************************/
	/**
	* @brief	Allocates enough memory to hold the specified type, on the stack, but
	* 			does not initialize the object.
	* @see		MemoryStackInternal::Alloc()
	*/
	/************************************************************************************************************************/
	template<class T>
	T* ge_stack_alloc()
	{
		return (T*)MemStack::Alloc(sizeof(T));
	}

	/************************************************************************************************************************/
	/**
	* @brief	Allocates enough memory to hold N objects of the specified type,
	* 			on the stack, but does not initialize the objects.
	* @see		MemoryStackInternal::Alloc()
	*/
	/************************************************************************************************************************/
	template<class T>
	T* ge_stack_alloc(uint32 count)
	{
		return (T*)MemStack::Alloc(sizeof(T) * count);
	}

	/************************************************************************************************************************/
	/**
	* @brief	Allocates enough memory to hold the specified type, on the stack,
	* 			and initializes the object using the parameterless constructor.
	* @see		MemoryStackInternal::Alloc()
	*/
	/************************************************************************************************************************/
	template<class T>
	T* ge_stack_new(uint32 count = 0)
	{
		T* data = ge_stack_alloc<T>(count);

		for( uint32 i=0; i<count; i++ )
		{
			new ((void*)&data[i]) T;
		}

		return data;
	}

	/************************************************************************************************************************/
	/**
	* @brief	Allocates enough memory to hold the specified type, on the stack, and constructs the object.
	* @see		MemoryStackInternal::Alloc()
	*/
	/************************************************************************************************************************/
	template<class T, class... Args>
	T* ge_stack_new(Args&&... args, uint32 count = 0)
	{
		T* data = ge_stack_alloc<T>(count);

		for( uint32 i=0; i<count; i++ )
		{
			new ((void*)&data[i]) T(std::forward<Args>(args)...);
		}

		return data;
	}

	/************************************************************************************************************************/
	/**
	* @brief	Destructs and deallocates last allocated entry currently located on stack.
	* @see		MemoryStackInternal::Dealloc()
	*/
	/************************************************************************************************************************/
	template<class T>
	void ge_stack_delete(T* data)
	{
		data->~T();
		MemStack::DeallocLast((uint8*)data);
	}

	/************************************************************************************************************************/
	/**
	* @brief	Destructs an array of objects and deallocates last allocated entry currently located on stack.
	*
	* @see		MemoryStackInternal::Dealloc()
	*/
	/************************************************************************************************************************/
	template<class T>
	void ge_stack_delete(T* data, uint32 count)
	{
		for( uint32 i=0; i<count; i++ )
		{
			data[i].~T();
		}

		MemStack::DeallocLast((uint8*)data);
	}

	/************************************************************************************************************************/
	/**
	* @copydoc	MemoryStackInternal::Dealloc()
	*/
	/************************************************************************************************************************/
	GE_UTILITY_EXPORT void ge_stack_free(void* data);

	/************************************************************************************************************************/
	/**
	* @brief	Allows use of a stack allocator by using normal new/delete/free/dealloc operators.
	*
	* @see		MemStack
	*/
	/************************************************************************************************************************/
	class StackAlloc
	{};

	/************************************************************************************************************************/
	/**
	* @brief	Specialized memory allocator implementations that allows use of a stack allocator in normal
	*			new/delete/free/dealloc operators.
	*
	* @see		MemStack
	*/
	/************************************************************************************************************************/
	template<>
	class MemoryAllocator<StackAlloc> : public MemoryAllocatorBase
	{
	public:
		static inline void* Allocate(SIZE_T bytes)
		{
			return ge_stack_alloc((uint32)bytes);
		}

		static inline void* AllocateArray(SIZE_T bytes, uint32 count)
		{
			return ge_stack_alloc((uint32)(bytes * count));
		}

		static inline void Free(void* ptr)
		{
			ge_stack_free(ptr);
		}

		static inline void FreeArray(void* ptr, uint32)
		{
			ge_stack_free(ptr);
		}
	};
}
