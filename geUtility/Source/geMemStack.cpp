/********************************************************************/
/**
 * @file   geMemStack.cpp
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

/************************************************************************************************************************/
/* Includes																												*/
/************************************************************************************************************************/
#include "gePrerequisitesUtil.h"
#include "geMemStack.h"

namespace geEngineSDK
{
	GE_THREADLOCAL MemStackInternal<1024 * 1024>* MemStack::ThreadMemStack = nullptr;

	void MemStack::BeginThread()
	{
		if( ThreadMemStack != nullptr )
		{
			EndThread();
		}

		ThreadMemStack = ge_new<MemStackInternal<1024 * 1024>>();
	}

	void MemStack::EndThread()
	{
		if( ThreadMemStack != nullptr )
		{
			ge_delete(ThreadMemStack);
			ThreadMemStack = nullptr;
		}
	}

	uint8* MemStack::Alloc(uint32 numBytes)
	{
		GE_ASSERT(ThreadMemStack != nullptr && "Stack allocation failed. Did you call BeginThread?");
		return ThreadMemStack->Alloc(numBytes);
	}

	void MemStack::DeallocLast(uint8* data)
	{
		GE_ASSERT(ThreadMemStack != nullptr && "Stack deallocation failed. Did you call BeginThread?");
		ThreadMemStack->Dealloc(data);
	}

	void* ge_stack_alloc(uint32 numBytes)
	{
		return (void*)MemStack::Alloc(numBytes);
	}

	void ge_stack_free(void* data)
	{
		return MemStack::DeallocLast((uint8*)data);
	}
}
