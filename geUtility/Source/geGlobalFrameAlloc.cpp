/*****************************************************************************/
/**
 * @file    geGlobalFrameAlloc.cpp
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2016/03/06
 * @brief   Global Frame allocator objects
 *
 * Functions used to create a Frame allocator for each thread
 *
 * @bug     No known bugs.
 */
/*****************************************************************************/

/*****************************************************************************/
/**
* Includes
*/
/*****************************************************************************/
#include "gePrerequisitesUtil.h"
#include "geGlobalFrameAlloc.h"
#include "geFrameAlloc.h"

namespace geEngineSDK {
	GE_THREADLOCAL FrameAlloc* _GlobalFrameAlloc = nullptr;

	FrameAlloc&
  g_frameAlloc() {
    if (nullptr == _GlobalFrameAlloc) {
      /** HACK: This will leak memory but since it should exist throughout the
      entirety of runtime it should only leak on shutdown when the OS will free
      it anyway. */
      _GlobalFrameAlloc = new FrameAlloc();
    }
		return *_GlobalFrameAlloc;
	}

	uint8*
  ge_frame_alloc(SIZE_T numBytes) {
		return g_frameAlloc().alloc(numBytes);
	}

	void
  ge_frame_free(void* data) {
		//HACK: Test if this casting is working correctly on PS4
		g_frameAlloc().dealloc(*reinterpret_cast<uint32**>(data));
	}

	void
  ge_frame_mark() {
		g_frameAlloc().markFrame();
	}

	void
  ge_frame_clear() {
		g_frameAlloc().clear();
	}
}
