/********************************************************************/
/**
 * @file   geFrameAlloc.h
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
#pragma once

/************************************************************************************************************************/
/* Includes																												*/
/************************************************************************************************************************/
#include "gePrerequisitesUtil.h"

namespace geEngineSDK
{
	/************************************************************************************************************************/
	/**
	* @brief	Frame allocator. Performs very fast allocations but can only free all of its memory at once.
	*			Perfect for allocations that last just a single frame.
	*
	* @note		Not thread safe with an exception. Alloc() and Clear() methods need to be called from the same thread.
	* 			Dealloc() is thread safe and can be called from any thread.
	*/
	/************************************************************************************************************************/
	class GE_UTILITY_EXPORT FrameAlloc
	{
	private:
		/************************************************************************************************************************/
		/**
		* @brief	A single block of memory within a frame allocator.
		*/
		/************************************************************************************************************************/
		class MemBlock
		{
		public:
			uint8* m_Data;
			uint32 m_FreePtr;
			uint32 m_Size;

		public:
			MemBlock(uint32 size);
			~MemBlock();

			/************************************************************************************************************************/
			/**
			* @brief	Allocates a piece of memory within the block. Caller must ensure the block has enough empty space.
			*/
			/************************************************************************************************************************/
			uint8* Alloc(uint32 amount);

			/************************************************************************************************************************/
			/**
			* @brief	Releases all allocations within a block but doesn't actually free the memory.
			*/
			/************************************************************************************************************************/
			void Clear();
		};
	
	private:
		uint32 m_BlockSize;
		Vector<MemBlock*> m_Blocks;
		
		MemBlock* m_FreeBlock;
		uint32 m_NextBlockIdx;

		std::atomic<uint32> m_TotalAllocBytes;
		uint32* m_LastFrame;

#if GE_DEBUG_MODE
		GE_THREAD_ID_TYPE m_OwnerThread;
#endif

	public:
		FrameAlloc(uint32 blockSize = 1024 * 1024);
		~FrameAlloc();

		/************************************************************************************************************************/
		/**
		* @brief	Allocates a new block of memory of the specified size.
		* @param[in]	Amount	Amount of memory to allocate, in bytes.
		* @note		Not thread safe.
		*/
		/************************************************************************************************************************/
		uint8* Alloc(uint32 amount);

		/************************************************************************************************************************/
		/**
		* @brief	Allocates and constructs a new object.
		* @note		Not thread safe.
		*/
		/************************************************************************************************************************/
		template<class T, class... Args>
		T* Alloc(Args&&... args)
		{
			return new ((T*)Alloc(sizeof(T))) T(std::forward<Args>(args)...);
		}

		/************************************************************************************************************************/
		/**
		* @brief	Deallocates a previously allocated block of memory.
		* @note		No deallocation is actually done here. This method is only used for debug purposes so it is easier to track
		*			down memory leaks and corruption.
		* @note		Thread safe.
		*/
		/************************************************************************************************************************/
		void Dealloc(uint8* data);

		/************************************************************************************************************************/
		/**
		* @brief	Deallocates and destructs a previously allocated object.
		* @note		No deallocation is actually done here. This method is only used to call the destructor and for debug purposes
		*			so it is easier to track down memory leaks and corruption.
		* @note		Thread safe.
		*/
		/************************************************************************************************************************/
		template<class T>
		void Dealloc(T* obj)
		{
			if( obj != nullptr )
			{
				obj->~T();
			}

			Dealloc( (uint8*)obj );
		}

		/************************************************************************************************************************/
		/**
		* @brief	Starts a new frame. Next call to ::Clear will only clear memory allocated past this point.
		*/
		/************************************************************************************************************************/
		void MarkFrame();

		/************************************************************************************************************************/
		/**
		* @brief	Deallocates all allocated memory since the last call to MarkFrame() (or all the memory if there was no call
		*			to markFrame()).
		* @note		Not thread safe.
		*/
		/************************************************************************************************************************/
		void Clear();

		/************************************************************************************************************************/
		/**
		* @brief	Changes the frame allocator owner thread. After the owner thread has changed only allocations from that
		*			thread can be made.
		*/
		/************************************************************************************************************************/
		void SetOwnerThread(GE_THREAD_ID_TYPE thread);

		/************************************************************************************************************************/
		/**
		* @brief	Allocates a dynamic block of memory of the wanted size. The exact allocation size might be slightly higher in
		*			order to store block meta data.
		*/
		/************************************************************************************************************************/
		MemBlock* AllocBlock(uint32 wantedSize);

		/************************************************************************************************************************/
		/**
		* @brief	Frees a memory block.
		*/
		/************************************************************************************************************************/
		void DeallocBlock(MemBlock* block);
	};

	/************************************************************************************************************************/
	/**
	* @brief	Allocator for the standard library that internally uses a frame allocator.
	*/
	/************************************************************************************************************************/
	template <class T>
	class StdFrameAlloc
	{
	public:
		FrameAlloc* m_FrameAlloc;

	public:
		typedef T value_type;

		StdFrameAlloc() _NOEXCEPT : m_FrameAlloc(nullptr)
		{
		}

		StdFrameAlloc(FrameAlloc* alloc) _NOEXCEPT : m_FrameAlloc(alloc)
		{
		}

		template<class T> StdFrameAlloc(const StdFrameAlloc<T>& alloc) _NOEXCEPT : m_FrameAlloc(alloc.m_FrameAlloc)
		{
		}

		template<class T> bool operator==(const StdFrameAlloc<T>&) const _NOEXCEPT { return true; }
		template<class T> bool operator!=(const StdFrameAlloc<T>&) const _NOEXCEPT { return false; }

		/************************************************************************************************************************/
		/**
		* @brief	Allocate but don't initialize number elements of type T.
		*/
		/************************************************************************************************************************/
		T* allocate(const size_t num) const
		{
			if( num == 0 )
			{
				return nullptr;
			}

			if( num > static_cast<size_t>(-1) / sizeof(T) )
			{
				throw std::bad_array_new_length();
			}

			void* const pv = m_FrameAlloc->Alloc((uint32)(num * sizeof(T)));
			if( !pv )
			{
				throw std::bad_alloc();
			}

			return static_cast<T*>(pv);
		}

		/************************************************************************************************************************/
		/**
		* @brief	Deallocate storage p of deleted elements.
		*/
		/************************************************************************************************************************/
		void deallocate(T* p, size_t) const _NOEXCEPT
		{
			m_FrameAlloc->Dealloc((uint8*)p);
		}
	};

	/************************************************************************************************************************/
	/**
	* @brief	Return that all specializations of this allocator are interchangeable.
	*/
	/************************************************************************************************************************/
	template <class T1, class T2>
	bool operator== (const StdFrameAlloc<T1>&, const StdFrameAlloc<T2>&) _NOEXCEPT
	{
		return true;
	}

	/************************************************************************************************************************/
	/**
	* @brief	Return that all specializations of this allocator are interchangeable.
	*/
	/************************************************************************************************************************/
	template <class T1, class T2>
	bool operator!= (const StdFrameAlloc<T1>&, const StdFrameAlloc<T2>&) _NOEXCEPT
	{
		return false;
	}
}
