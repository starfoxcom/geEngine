/********************************************************************/
/**
 * @file   geGlobalFrameAlloc.cpp
 * @author Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date   2016/03/06
 * @brief  Global Frame allocator objects
 *
 * Functions used to create a Frame allocator for each thread
 *
 * @bug	   No known bugs.
 */
/********************************************************************/
#include "gePrerequisitesUtil.h"
#include "geGlobalFrameAlloc.h"
#include "geFrameAlloc.h"

namespace geEngineSDK
{
	GE_THREADLOCAL FrameAlloc* _GlobalFrameAlloc = nullptr;

	inline FrameAlloc& g_FrameAlloc()
	{
		if( _GlobalFrameAlloc == nullptr )
		{
			//HACK: This will leak memory but since it should exist throughout the entirety of runtime it should only leak on shutdown when the OS will free it anyway.
			_GlobalFrameAlloc = new FrameAlloc();
		}

		return *_GlobalFrameAlloc;
	}

	uint8* ge_frame_alloc(uint32 numBytes)
	{
		return g_FrameAlloc().Alloc(numBytes);
	}

	void ge_frame_free(void* data)
	{
		//HACK: Test if this casting is working correctly on PS4
		g_FrameAlloc().Dealloc(*(uint32**)data);
	}

	inline void ge_frame_mark()
	{
		g_FrameAlloc().MarkFrame();
	}

	inline void ge_frame_clear()
	{
		g_FrameAlloc().Clear();
	}
}
