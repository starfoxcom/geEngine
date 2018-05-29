/*****************************************************************************/
/**
 * @file    geWin32Platform.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2018/05/26
 * @brief   Win32 specific functionality. Contains the main message loop.
 *
 * Various Win32 specific functionality. Contains the main message loop.
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
#include "gePrerequisitesCore.h"
#include "gePlatform.h"

#include <Win32/geMinWindows.h>
#include <windowsx.h>

#define WM_GE_SETCAPTURE      WM_USER + 101
#define WM_GE_RELEASECAPTURE  WM_USER + 102

namespace geEngineSDK {
  class GE_CORE_EXPORT Win32Platform : public Platform
  {
   public:
    /**
     * @brief Called when a new DropTarget gets created.
     */
    static void
    registerDropTarget(DropTarget* target);

    /**
     * @brief Called just before a DropTarget gets destroyed.
     */
    static void
    unregisterDropTarget(DropTarget* target);

    /**
     * @brief Main message loop callback that processes messages received from
     *        windows.
     */
    static LRESULT CALLBACK
    _win32WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
  };
}
