/*****************************************************************************/
/**
 * @file    geWin32Platform.cpp
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2018/05/26
 * @brief   Win32 specific functionality. Contains the main message loop.
 *
 * Various Win32 specific functionality. Contains the main message loop.
 *
 * @bug     No known bugs.
 */
/*****************************************************************************/

/*****************************************************************************/
/**
 * Includes
 */
/*****************************************************************************/
#include "Win32/geWin32Platform.h"
#include "geDropTarget.h"
#include "geRenderWindow.h"
#include "gePixelUtil.h"
#include "geRenderWindowManager.h"
#include "Win32/geWin32DropTarget.h"

#include "geCoreApplication.h"

#include <Win32/geWin32PlatformUtility.h>
#include <geDebug.h>
#include <geUnicode.h>
#include <shellapi.h>
#include <timeapi.h>

namespace geEngineSDK {
  /**
   * @brief Encapsulate native cursor data so we can avoid including windows.h
   *        as it pollutes the global namespace.
   */
  struct GE_CORE_EXPORT NativeCursorData
  {
    HCURSOR cursor;
  };

  /**
   * @brief Encapsulate drop target data so we can avoid including windows.h as
   *        it pollutes the global namespace.
   */
  struct GE_CORE_EXPORT NativeDropTargetData
  {
    Map<const RenderWindow*, Win32DropTarget*> dropTargetsPerWindow;
    Vector<Win32DropTarget*> dropTargetsToInitialize;
    Vector<Win32DropTarget*> dropTargetsToDestroy;
  };

  struct Platform::Pimpl
  {
    bool m_isCursorHidden = false;
    NativeCursorData m_cursor;
    bool m_usingCustomCursor = false;
    Map<const geCoreThread::RenderWindow*,
        WindowNonClientAreaData> m_nonClientAreas;

    bool m_isTrackingMouse = false;
    NativeDropTargetData m_dropTargets;

    bool m_requiresStartUp = false;
    bool m_requiresShutDown = false;

    bool m_cursorClipping = false;
    HWND m_clipWindow = nullptr;
    RECT m_clipRect;

    bool m_isActive = false;

    Mutex m_sync;
  };

  Event<void(const Vector2I&,
             const OSPointerButtonStates&)> Platform::onCursorMoved;

  Event<void(const Vector2I&,
             OS_MOUSE_BUTTON::E button,
             const OSPointerButtonStates&)> Platform::onCursorButtonPressed;

  Event<void(const Vector2I&,
             OS_MOUSE_BUTTON::E button,
             const OSPointerButtonStates&)> Platform::onCursorButtonReleased;

  Event<void(const Vector2I&,
             const OSPointerButtonStates&)> Platform::onCursorDoubleClick;

  Event<void(INPUT_COMMAND_TYPE::E)> Platform::onInputCommand;

  Event<void(float)> Platform::onMouseWheelScrolled;

  Event<void(uint32)> Platform::onCharInput;

  Event<void()> Platform::onMouseCaptureChanged;

  Platform::Pimpl* Platform::m_data = ge_new<Platform::Pimpl>();

  /**
   * @brief Checks if any of the windows of the current application are active.
   */
  bool
  isAppActive(Platform::Pimpl* data) {
    Lock lock(data->m_sync);
    return data->m_isActive;
  }

  /**
   * @brief Enables or disables cursor clipping depending on the stored data.
   */
  void
  applyClipping(Platform::Pimpl* data) {
    if (data->m_cursorClipping) {
      if (data->m_clipWindow) {
        //Clip cursor to the window
        RECT clipWindowRect;
        if (GetWindowRect(data->m_clipWindow, &clipWindowRect)) {
          ClipCursor(&clipWindowRect);
        }
      }
      else {
        ClipCursor(&data->m_clipRect);
      }
    }
    else {
      ClipCursor(nullptr);
    }
  }

  Platform::~Platform() {
    ge_delete(m_data);
    m_data = nullptr;
  }

  Vector2I
  Platform::getCursorPosition() {
    Vector2I screenPos;

    POINT cursorPos;
    GetCursorPos(&cursorPos);

    screenPos.x = cursorPos.x;
    screenPos.y = cursorPos.y;

    return screenPos;
  }

  void
  Platform::setCursorPosition(const Vector2I& screenPos) {
    SetCursorPos(screenPos.x, screenPos.y);
  }

  void
  Platform::captureMouse(const RenderWindow& /*window*/) {
    SPtr<RenderWindow> primaryWindow = g_coreApplication().getPrimaryWindow();
    uint64 hwnd;
    primaryWindow->getCustomAttribute("WINDOW", &hwnd);
    PostMessage(reinterpret_cast<HWND>(hwnd),
                WM_GE_SETCAPTURE,
                WPARAM(reinterpret_cast<HWND>(hwnd)), 0);
  }

  void
  Platform::releaseMouseCapture() {
    SPtr<RenderWindow> primaryWindow = g_coreApplication().getPrimaryWindow();
    uint64 hwnd;
    primaryWindow->getCustomAttribute("WINDOW", &hwnd);
    PostMessage(reinterpret_cast<HWND>(hwnd),
                WM_GE_RELEASECAPTURE,
                WPARAM(reinterpret_cast<HWND>(hwnd)), 0);
  }

  bool
  Platform::isPointOverWindow(const RenderWindow& window,
                              const Vector2I& screenPos) {
    SPtr<RenderWindow> primaryWindow = g_coreApplication().getPrimaryWindow();

    POINT point;
    point.x = screenPos.x;
    point.y = screenPos.y;

    uint64 hwndToCheck;
    window.getCustomAttribute("WINDOW", &hwndToCheck);

    HWND hwndUnderPos = WindowFromPoint(point);
    return hwndUnderPos == reinterpret_cast<HWND>(hwndToCheck);
  }

  void
  Platform::hideCursor() {
    if (m_data->m_isCursorHidden) {
      return;
    }

    m_data->m_isCursorHidden = true;

    //ShowCursor(FALSE) doesn't work. Presumably because we're in the wrong
    //thread, and using WM_SETCURSOR in message loop to hide the cursor is a
    //smarter solution anyway.
    SPtr<RenderWindow> primaryWindow = g_coreApplication().getPrimaryWindow();
    uint64 hwnd;
    primaryWindow->getCustomAttribute("WINDOW", &hwnd);
    PostMessage(reinterpret_cast<HWND>(hwnd),
                 WM_SETCURSOR,
                 WPARAM(reinterpret_cast<HWND>(hwnd)),
                 static_cast<LPARAM>(MAKELONG(HTCLIENT, WM_MOUSEMOVE)));
  }

  void
  Platform::showCursor() {
    if (!m_data->m_isCursorHidden) {
      return;
    }

    m_data->m_isCursorHidden = false;

    //ShowCursor(FALSE) doesn't work. Presumably because we're in the wrong
    //thread, and using WM_SETCURSOR in message loop to hide the cursor is a
    //smarter solution anyway.

    SPtr<RenderWindow> primaryWindow = g_coreApplication().getPrimaryWindow();
    uint64 hwnd;
    primaryWindow->getCustomAttribute("WINDOW", &hwnd);

    PostMessage(reinterpret_cast<HWND>(hwnd),
                WM_SETCURSOR,
                WPARAM(reinterpret_cast<HWND>(hwnd)),
                (LPARAM)MAKELONG(HTCLIENT, WM_MOUSEMOVE));
  }

  bool
  Platform::isCursorHidden() {
    return m_data->m_isCursorHidden;
  }

  void
  Platform::clipCursorToWindow(const RenderWindow& window) {
    uint64 hwnd;
    window.getCustomAttribute("WINDOW", &hwnd);

    m_data->m_cursorClipping = true;
    m_data->m_clipWindow = reinterpret_cast<HWND>(hwnd);

    if (isAppActive(m_data)) {
      applyClipping(m_data);
    }
  }

  void
  Platform::clipCursorToRect(const Box2DI& screenRect) {
    m_data->m_cursorClipping = true;
    m_data->m_clipWindow = nullptr;

    m_data->m_clipRect.left = screenRect.m_min.x;
    m_data->m_clipRect.top = screenRect.m_min.y;
    m_data->m_clipRect.right = screenRect.m_min.x + screenRect.m_max.x;
    m_data->m_clipRect.bottom = screenRect.m_min.y + screenRect.m_max.y;

    if (isAppActive(m_data)) {
      applyClipping(m_data);
    }
  }

  void
  Platform::clipCursorDisable() {
    m_data->m_cursorClipping = false;
    m_data->m_clipWindow = nullptr;

    if (isAppActive(m_data)) {
      applyClipping(m_data);
    }
  }

  //TODO: Add support for animated custom cursor
  void
  Platform::setCursor(PixelData& pixelData, const Vector2I& hotSpot) {
    if (m_data->m_usingCustomCursor) {
      SetCursor(nullptr);
      DestroyIcon(m_data->m_cursor.cursor);
    }

    m_data->m_usingCustomCursor = true;

    Vector<LinearColor> pixels = pixelData.getColors();
    uint32 width = pixelData.getWidth();
    uint32 height = pixelData.getHeight();

    HBITMAP hBitmap = Win32PlatformUtility::createBitmap(pixels.data(), width, height, false);
    HBITMAP hMonoBitmap = CreateBitmap(width, height, 1, 1, nullptr);

    ICONINFO iconinfo = { 0 };
    iconinfo.fIcon = FALSE;
    iconinfo.xHotspot = static_cast<DWORD>(hotSpot.x);
    iconinfo.yHotspot = static_cast<DWORD>(hotSpot.y);
    iconinfo.hbmMask = hMonoBitmap;
    iconinfo.hbmColor = hBitmap;

    m_data->m_cursor.cursor = CreateIconIndirect(&iconinfo);

    DeleteObject(hBitmap);
    DeleteObject(hMonoBitmap);

    //Make sure we notify the message loop to perform the actual cursor update
    SPtr<RenderWindow> primaryWindow = g_coreApplication().getPrimaryWindow();
    uint64 hwnd;
    primaryWindow->getCustomAttribute("WINDOW", &hwnd);

    PostMessage(reinterpret_cast<HWND>(hwnd),
                WM_SETCURSOR,
                WPARAM(reinterpret_cast<HWND>(hwnd)),
                static_cast<LPARAM>(MAKELONG(HTCLIENT, WM_MOUSEMOVE)));
  }

  void Platform::setIcon(const PixelData& pixelData)
  {
    Vector<LinearColor> pixels = pixelData.getColors();
    uint32 width = pixelData.getWidth();
    uint32 height = pixelData.getHeight();

    auto hBitmap = Win32PlatformUtility::createBitmap(pixels.data(), width, height, false);
    HBITMAP hMonoBitmap = CreateBitmap(width, height, 1, 1, nullptr);

    ICONINFO iconinfo = { 0 };
    iconinfo.fIcon = TRUE;
    iconinfo.xHotspot = 0;
    iconinfo.yHotspot = 0;
    iconinfo.hbmMask = hMonoBitmap;
    iconinfo.hbmColor = hBitmap;

    HICON icon = CreateIconIndirect(&iconinfo);

    DeleteObject(hBitmap);
    DeleteObject(hMonoBitmap);

    // Make sure we notify the message loop to perform the actual cursor update
    SPtr<RenderWindow> primaryWindow = g_coreApplication().getPrimaryWindow();
    uint64 hwnd;
    primaryWindow->getCustomAttribute("WINDOW", &hwnd);

    PostMessage(reinterpret_cast<HWND>(hwnd),
                WM_SETICON,
                WPARAM(ICON_BIG),
                reinterpret_cast<LPARAM>(icon));
  }

  void
  Platform::setCaptionNonClientAreas(const geCoreThread::RenderWindow& window,
                                     const Vector<Box2DI>& nonClientAreas) {
    Lock lock(m_data->m_sync);
    m_data->m_nonClientAreas[&window].moveAreas = nonClientAreas;
  }

  void
  Platform::setResizeNonClientAreas(const geCoreThread::RenderWindow& window,
                                    const Vector<NonClientResizeArea>& nonClientAreas) {
    Lock lock(m_data->m_sync);
    m_data->m_nonClientAreas[&window].resizeAreas = nonClientAreas;
  }

  void
  Platform::resetNonClientAreas(const geCoreThread::RenderWindow& window) {
    Lock lock(m_data->m_sync);
    auto iterFind = m_data->m_nonClientAreas.find(&window);
    if (end(m_data->m_nonClientAreas) != iterFind) {
      m_data->m_nonClientAreas.erase(iterFind);
    }
  }

  void
  Platform::sleep(uint32 duration) {
    Sleep(static_cast<DWORD>(duration));
  }

  void
  Win32Platform::registerDropTarget(DropTarget* target) {
    const RenderWindow* window = target->_getOwnerWindow();

    Win32DropTarget* win32DropTarget = nullptr;
    auto iterFind = m_data->m_dropTargets.dropTargetsPerWindow.find(window);
    if (m_data->m_dropTargets.dropTargetsPerWindow.end() == iterFind) {
      uint64 hwnd;
      window->getCustomAttribute("WINDOW", &hwnd);

      win32DropTarget = ge_new<Win32DropTarget>(reinterpret_cast<HWND>(hwnd));
      m_data->m_dropTargets.dropTargetsPerWindow[window] = win32DropTarget;
      {
        Lock lock(m_data->m_sync);
        m_data->m_dropTargets.dropTargetsToInitialize.push_back(win32DropTarget);
      }
    }
    else {
      win32DropTarget = iterFind->second;
    }

    win32DropTarget->registerDropTarget(target);
  }

  void
  Win32Platform::unregisterDropTarget(DropTarget* target) {
    auto itFind = m_data->m_dropTargets.dropTargetsPerWindow.find(target->_getOwnerWindow());
    if (m_data->m_dropTargets.dropTargetsPerWindow.end() == itFind) {
      LOGWRN("Attempting to destroy a drop target but cannot find its parent window.");
    }
    else {
      Win32DropTarget* win32DropTarget = itFind->second;
      win32DropTarget->unregisterDropTarget(target);

      if (win32DropTarget->getNumDropTargets() == 0) {
        m_data->m_dropTargets.dropTargetsPerWindow.erase(itFind);
        {
          Lock lock(m_data->m_sync);
          m_data->m_dropTargets.dropTargetsToDestroy.push_back(win32DropTarget);
        }
      }
    }
  }

  void
  Platform::copyToClipboard(const String& string) {
    WString wStr = UTF8::toWide(string);
    HANDLE hData = GlobalAlloc(GMEM_MOVEABLE | GMEM_DDESHARE,
                               (wStr.size() + 1) * sizeof(WString::value_type));
    auto buffer = reinterpret_cast<WString::value_type*>(GlobalLock(hData));

    wStr.copy(buffer, wStr.size());
    buffer[wStr.size()] = '\0';

    GlobalUnlock(hData);

    if (OpenClipboard(nullptr)) {
      EmptyClipboard();
      SetClipboardData(CF_UNICODETEXT, hData);
      CloseClipboard();
    }

    GlobalFree(hData);
  }

  String Platform::copyFromClipboard()
  {
    if (OpenClipboard(nullptr)) {
      HANDLE hData = GetClipboardData(CF_UNICODETEXT);

      if (nullptr != hData) {
        auto buffer = reinterpret_cast<WString::value_type*>(GlobalLock(hData));
        WString wideString(buffer);
        GlobalUnlock(hData);

        CloseClipboard();
        return UTF8::fromWide(wideString);
      }

      CloseClipboard();
    }

    return u8"";
  }

  String
  Platform::keyCodeToUnicode(uint32 keyCode) {
    static HKL keyboardLayout = GetKeyboardLayout(0);
    static UINT8 keyboarState[256];

    if (FALSE == GetKeyboardState(keyboarState)) {
      return nullptr;
    }

    UINT virtualKey = MapVirtualKeyExW(keyCode, 1, keyboardLayout);

    wchar_t output[2];
    int count = ToUnicodeEx(virtualKey,
                            keyCode,
                            keyboarState,
                            output,
                            2,
                            0,
                            keyboardLayout);
    if (0 < count) {
      return UTF8::fromWide(WString(output, count));
    }

    return StringUtil::BLANK;
  }

  void
  Platform::openFolder(const Path& path) {
    WString pathString = UTF8::toWide(path.toString());
    ShellExecuteW(nullptr, L"open", pathString.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
  }

  void
  Platform::_messagePump() {
    MSG  msg;
    while (PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE)) {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    }
  }

  void
  Platform::_startUp() {
    Lock lock(m_data->m_sync);
    if (timeBeginPeriod(1) == TIMERR_NOCANDO) {
      LOGWRN("Unable to set timer resolution to 1ms. This can cause "
             "significant waste in performance for waiting threads.");
    }
    m_data->m_requiresStartUp = true;
  }

  void
  Platform::_update() {
    for (auto& dropTarget : m_data->m_dropTargets.dropTargetsPerWindow) {
      dropTarget.second->update();
    }
  }

  void
  Platform::_coreUpdate() {
    {
      Lock lock(m_data->m_sync);
      if (m_data->m_requiresStartUp) {
        OleInitialize(nullptr);
        m_data->m_requiresStartUp = false;
      }
    }

    {
      Lock lock(m_data->m_sync);
      for (auto& dropTargetToDestroy : m_data->m_dropTargets.dropTargetsToDestroy) {
        dropTargetToDestroy->unregisterWithOS();
        dropTargetToDestroy->Release();
      }
      m_data->m_dropTargets.dropTargetsToDestroy.clear();
    }

    {
      Lock lock(m_data->m_sync);
      for (auto& dropTargetToInit : m_data->m_dropTargets.dropTargetsToInitialize) {
        dropTargetToInit->registerWithOS();
      }
      m_data->m_dropTargets.dropTargetsToInitialize.clear();
    }

    _messagePump();

    {
      Lock lock(m_data->m_sync);
      if (m_data->m_requiresShutDown) {
        OleUninitialize();
        m_data->m_requiresShutDown = false;
      }
    }
  }

  void Platform::_shutDown() {
    Lock lock(m_data->m_sync);
    timeEndPeriod(1);
    m_data->m_requiresShutDown = true;
  }

  bool isShiftPressed = false;
  bool isCtrlPressed = false;

  /**
   * @brief Translate engine non client area to win32 non client area.
   */
  LRESULT translateNonClientAreaType(NON_CLIENT_AREA_BORDER_TYPE::E type) {
    LRESULT dir = HTCLIENT;
    switch (type)
    {
      case NON_CLIENT_AREA_BORDER_TYPE::kLeft:
        dir = HTLEFT;
        break;
      case NON_CLIENT_AREA_BORDER_TYPE::kTopLeft:
        dir = HTTOPLEFT;
        break;
      case NON_CLIENT_AREA_BORDER_TYPE::kTop:
        dir = HTTOP;
        break;
      case NON_CLIENT_AREA_BORDER_TYPE::kTopRight:
        dir = HTTOPRIGHT;
        break;
      case NON_CLIENT_AREA_BORDER_TYPE::kRight:
        dir = HTRIGHT;
        break;
      case NON_CLIENT_AREA_BORDER_TYPE::kBottomRight:
        dir = HTBOTTOMRIGHT;
        break;
      case NON_CLIENT_AREA_BORDER_TYPE::kBottom:
        dir = HTBOTTOM;
        break;
      case NON_CLIENT_AREA_BORDER_TYPE::kBottomLeft:
        dir = HTBOTTOMLEFT;
        break;
    }
    return dir;
  }

  /**
   * @brief Method triggered whenever a mouse event happens.
   */
  void
  getMouseData(HWND hWnd,
               WPARAM wParam,
               LPARAM lParam,
               bool nonClient,
               Vector2I& mousePos,
               OSPointerButtonStates& btnStates) {
    POINT clientPoint;

    clientPoint.x = GET_X_LPARAM(lParam);
    clientPoint.y = GET_Y_LPARAM(lParam);

    if (!nonClient) {
      ClientToScreen(hWnd, &clientPoint);
    }

    mousePos.x = clientPoint.x;
    mousePos.y = clientPoint.y;

    btnStates.mouseButtons[0] = (wParam & MK_LBUTTON) != 0;
    btnStates.mouseButtons[1] = (wParam & MK_MBUTTON) != 0;
    btnStates.mouseButtons[2] = (wParam & MK_RBUTTON) != 0;
    btnStates.shift = (wParam & MK_SHIFT) != 0;
    btnStates.ctrl = (wParam & MK_CONTROL) != 0;
  }

  /**
   * @brief Converts a virtual key code into an input command, if possible.
   *        Returns true if conversion was done.
   * @param[in]   virtualKeyCode  Virtual key code to try to translate to a
   *                              command.
   * @param[out]  command Input command. Only valid if function returns true.
   */
  bool
  getCommand(uint32 virtualKeyCode, INPUT_COMMAND_TYPE::E& command) {
    switch (virtualKeyCode)
    {
      case VK_LEFT:
        command = isShiftPressed ? INPUT_COMMAND_TYPE::kSelectLeft
                                 : INPUT_COMMAND_TYPE::kCursorMoveLeft;
        return true;
      case VK_RIGHT:
        command = isShiftPressed ? INPUT_COMMAND_TYPE::kSelectRight
                                 : INPUT_COMMAND_TYPE::kCursorMoveRight;
        return true;
      case VK_UP:
        command = isShiftPressed ? INPUT_COMMAND_TYPE::kSelectUp
                                 : INPUT_COMMAND_TYPE::kCursorMoveUp;
        return true;
      case VK_DOWN:
        command = isShiftPressed ? INPUT_COMMAND_TYPE::kSelectDown
                                 : INPUT_COMMAND_TYPE::kCursorMoveDown;
        return true;
      case VK_ESCAPE:
        command = INPUT_COMMAND_TYPE::kEscape;
        return true;
      case VK_RETURN:
        command = isShiftPressed ? INPUT_COMMAND_TYPE::kReturn
                                 : INPUT_COMMAND_TYPE::kConfirm;
        return true;
      case VK_BACK:
        command = INPUT_COMMAND_TYPE::kBackspace;
        return true;
      case VK_DELETE:
        command = INPUT_COMMAND_TYPE::kDelete;
        return true;
      case VK_TAB:
        command = INPUT_COMMAND_TYPE::kTab;
        return true;
    }

    return false;
  }

  LRESULT CALLBACK
  Win32Platform::_win32WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    if (WM_CREATE == uMsg) {
      //Store pointer to Win32Window in user data area
      SetWindowLongPtr(hWnd,
                      GWLP_USERDATA,
                      reinterpret_cast<LONG_PTR>(((LPCREATESTRUCT)lParam)->lpCreateParams));

      auto newWindow = reinterpret_cast<geCoreThread::RenderWindow*>
                                        (GetWindowLongPtr(hWnd, GWLP_USERDATA));
      if (nullptr != newWindow) {
        const RenderWindowProperties& props = newWindow->getProperties();
        if (!props.m_isHidden) {
          ShowWindow(hWnd, SW_SHOWNORMAL);
        }
      }
      else {
        ShowWindow(hWnd, SW_SHOWNORMAL);
      }

      return 0;
    }

    auto win = reinterpret_cast<geCoreThread::RenderWindow*>
                                (GetWindowLongPtr(hWnd, GWLP_USERDATA));
    if (!win) {
      return DefWindowProc(hWnd, uMsg, wParam, lParam);
    }

    switch (uMsg)
    {
      case WM_ACTIVATE:
      {
        switch (wParam)
        {
          case WA_ACTIVE:
          case WA_CLICKACTIVE:
            {
              Lock lock(m_data->m_sync);
              m_data->m_isActive = true;
            }
          applyClipping(m_data);
          break;

          case WA_INACTIVE:
            {
              Lock lock(m_data->m_sync);
              m_data->m_isActive = false;
            }
          ClipCursor(nullptr);
          break;
          }
        return 0;
      }

      case WM_SETFOCUS:
      {
        if (!win->getProperties().m_hasFocus) {
          win->_notifyWindowEvent(WINDOW_EVENT_TYPE::kFocusReceived);
        }
        return 0;
      }

      case WM_KILLFOCUS:
      {
        if (win->getProperties().m_hasFocus) {
          win->_notifyWindowEvent(WINDOW_EVENT_TYPE::kFocusLost);
        }
        return 0;
      }

      case WM_SYSCHAR:
        if (VK_SPACE != wParam) {
          return 0;
        }
        break;

      case WM_MOVE:
        win->_notifyWindowEvent(WINDOW_EVENT_TYPE::kMoved);
        return 0;

      case WM_DISPLAYCHANGE:
        win->_notifyWindowEvent(WINDOW_EVENT_TYPE::kResized);
        break;

      case WM_SIZE:
        win->_notifyWindowEvent(WINDOW_EVENT_TYPE::kResized);
        if (SIZE_MAXIMIZED == wParam) {
          win->_notifyWindowEvent(WINDOW_EVENT_TYPE::kMaximized);
        }
        else if (SIZE_MINIMIZED == wParam) {
          win->_notifyWindowEvent(WINDOW_EVENT_TYPE::kMinimized);
        }
        else if (SIZE_RESTORED == wParam) {
          win->_notifyWindowEvent(WINDOW_EVENT_TYPE::kRestored);
        }
        return 0;

      case WM_SETCURSOR:
        if (isCursorHidden()) {
          SetCursor(nullptr);
        }
        else {
          switch (LOWORD(lParam))
          {
            case HTTOPLEFT:
              SetCursor(LoadCursor(nullptr, IDC_SIZENWSE));
              return 0;

            case HTTOP:
              SetCursor(LoadCursor(nullptr, IDC_SIZENS));
              return 0;

            case HTTOPRIGHT:
              SetCursor(LoadCursor(nullptr, IDC_SIZENESW));
              return 0;

            case HTLEFT:
              SetCursor(LoadCursor(nullptr, IDC_SIZEWE));
              return 0;

            case HTRIGHT:
              SetCursor(LoadCursor(nullptr, IDC_SIZEWE));
              return 0;

            case HTBOTTOMLEFT:
              SetCursor(LoadCursor(nullptr, IDC_SIZENESW));
              return 0;

            case HTBOTTOM:
              SetCursor(LoadCursor(nullptr, IDC_SIZENS));
              return 0;

            case HTBOTTOMRIGHT:
              SetCursor(LoadCursor(nullptr, IDC_SIZENWSE));
              return 0;
          }

          SetCursor(m_data->m_cursor.cursor);
        }
        return true;

      case WM_GETMINMAXINFO:
      {
        auto pMinMaxInfo = reinterpret_cast<MINMAXINFO*>(lParam);

        //Prevent the window from going smaller than some minimum size
        pMinMaxInfo->ptMinTrackSize.x = 100;
        pMinMaxInfo->ptMinTrackSize.y = 100;

        //Ensure maximizes window has proper size and doesn't cover the entire screen
        const POINT ptZero = { 0, 0 };
        HMONITOR primaryMonitor = MonitorFromPoint(ptZero, MONITOR_DEFAULTTOPRIMARY);

        MONITORINFO scrInfo;
        scrInfo.cbSize = sizeof(MONITORINFO);
        GetMonitorInfo(primaryMonitor, &scrInfo);

        pMinMaxInfo->ptMaxPosition.x = scrInfo.rcWork.left - scrInfo.rcMonitor.left;
        pMinMaxInfo->ptMaxPosition.y = scrInfo.rcWork.top - scrInfo.rcMonitor.top;
        pMinMaxInfo->ptMaxSize.x = scrInfo.rcWork.right - scrInfo.rcWork.left;
        pMinMaxInfo->ptMaxSize.y = scrInfo.rcWork.bottom - scrInfo.rcWork.top;
      }
      break;

      case WM_CLOSE:
      {
        win->_notifyWindowEvent(WINDOW_EVENT_TYPE::kCloseRequested);
        return 0;
      }

      case WM_NCHITTEST:
      {
        auto iterFind = m_data->m_nonClientAreas.find(win);
        if (m_data->m_nonClientAreas.end() == iterFind) {
          break;
        }

        POINT mousePos;
        mousePos.x = GET_X_LPARAM(lParam);
        mousePos.y = GET_Y_LPARAM(lParam);

        ScreenToClient(hWnd, &mousePos);

        Vector2I mousePosInt;
        mousePosInt.x = mousePos.x;
        mousePosInt.y = mousePos.y;

        Vector<NonClientResizeArea>& resizeAreasPerWindow = iterFind->second.resizeAreas;
        for (auto area : resizeAreasPerWindow) {
          if (area.area.isInside(mousePosInt)) {
            return translateNonClientAreaType(area.type);
          }
        }

        Vector<Box2DI>& moveAreasPerWindow = iterFind->second.moveAreas;
        for (auto area : moveAreasPerWindow) {
          if (area.isInside(mousePosInt)) {
            return HTCAPTION;
          }
        }

        return HTCLIENT;
      }

      case WM_NCLBUTTONDBLCLK:
        //Maximize/Restore on double-click
        if (HTCAPTION == wParam) {
          WINDOWPLACEMENT windowPlacement;
          windowPlacement.length = sizeof(WINDOWPLACEMENT);
          GetWindowPlacement(hWnd, &windowPlacement);

          if (SW_MAXIMIZE == windowPlacement.showCmd) {
            ShowWindow(hWnd, SW_RESTORE);
          }
          else {
            ShowWindow(hWnd, SW_MAXIMIZE);
          }
          return 0;
        }
        break;

      case WM_MOUSELEAVE:
      {
        //NOTE: Right now I track only mouse leaving client area. So it's
        //possible for the "mouse left window" callback to trigger, while the
        //mouse is still in the non-client area of the window.
        
        //TrackMouseEvent ends when this message is received and needs to be
        //re-applied
        m_data->m_isTrackingMouse = false;

        Lock lock(m_data->m_sync);
        win->_notifyWindowEvent(WINDOW_EVENT_TYPE::kMouseLeft);
      }
      return 0;

      case WM_LBUTTONUP:
      {
        ReleaseCapture();
        Vector2I intMousePos;
        OSPointerButtonStates btnStates;
        getMouseData(hWnd, wParam, lParam, false, intMousePos, btnStates);

        if (!onCursorButtonReleased.empty()) {
          onCursorButtonReleased(intMousePos, OS_MOUSE_BUTTON::kLeft, btnStates);
        }

        return 0;
      }

      case WM_MBUTTONUP:
      {
        ReleaseCapture();
        Vector2I intMousePos;
        OSPointerButtonStates btnStates;
        getMouseData(hWnd, wParam, lParam, false, intMousePos, btnStates);

        if (!onCursorButtonReleased.empty()) {
          onCursorButtonReleased(intMousePos, OS_MOUSE_BUTTON::kMiddle, btnStates);
        }

        return 0;
      }

      case WM_RBUTTONUP:
      {
        ReleaseCapture();
        Vector2I intMousePos;
        OSPointerButtonStates btnStates;
        getMouseData(hWnd, wParam, lParam, false, intMousePos, btnStates);

        if (!onCursorButtonReleased.empty()) {
          onCursorButtonReleased(intMousePos, OS_MOUSE_BUTTON::kRight, btnStates);
        }

        return 0;
      }

      case WM_LBUTTONDOWN:
      {
        SetCapture(hWnd);
        Vector2I intMousePos;
        OSPointerButtonStates btnStates;
        getMouseData(hWnd, wParam, lParam, false, intMousePos, btnStates);

        if (!onCursorButtonPressed.empty()) {
          onCursorButtonPressed(intMousePos, OS_MOUSE_BUTTON::kLeft, btnStates);
        }
      }
      return 0;

      case WM_MBUTTONDOWN:
      {
        SetCapture(hWnd);
        Vector2I intMousePos;
        OSPointerButtonStates btnStates;
        getMouseData(hWnd, wParam, lParam, false, intMousePos, btnStates);

        if (!onCursorButtonPressed.empty()) {
          onCursorButtonPressed(intMousePos, OS_MOUSE_BUTTON::kMiddle, btnStates);
        }
      }
      return 0;

      case WM_RBUTTONDOWN:
      {
        SetCapture(hWnd);
        Vector2I intMousePos;
        OSPointerButtonStates btnStates;
        getMouseData(hWnd, wParam, lParam, false, intMousePos, btnStates);

        if (!onCursorButtonPressed.empty()) {
          onCursorButtonPressed(intMousePos, OS_MOUSE_BUTTON::kRight, btnStates);
        }
      }
      return 0;

      case WM_LBUTTONDBLCLK:
      {
        Vector2I intMousePos;
        OSPointerButtonStates btnStates;
        getMouseData(hWnd, wParam, lParam, false, intMousePos, btnStates);

        if (!onCursorDoubleClick.empty()) {
          onCursorDoubleClick(intMousePos, btnStates);
        }
      }
      return 0;

      case WM_NCMOUSEMOVE:
      case WM_MOUSEMOVE:
      {
        //Set up tracking so we get notified when mouse leaves the window
        if (!m_data->m_isTrackingMouse) {
          TRACKMOUSEEVENT tme = { sizeof(tme) };
          tme.dwFlags = TME_LEAVE;
          tme.hwndTrack = hWnd;
          TrackMouseEvent(&tme);

          m_data->m_isTrackingMouse = true;
        }

        Vector2I intMousePos;
        OSPointerButtonStates btnStates;
        getMouseData(hWnd, wParam, lParam, uMsg == WM_NCMOUSEMOVE, intMousePos, btnStates);

        if (!onCursorMoved.empty()) {
          onCursorMoved(intMousePos, btnStates);
        }
        
        return 0;
      }

      case WM_MOUSEWHEEL:
      {
        int16 wheelDelta = GET_WHEEL_DELTA_WPARAM(wParam);

        float wheelDeltaFlt = wheelDelta / static_cast<float>(WHEEL_DELTA);
        if (!onMouseWheelScrolled.empty()) {
          onMouseWheelScrolled(wheelDeltaFlt);
        }

        return true;
      }

      case WM_SYSKEYDOWN:
      case WM_KEYDOWN:
      {
        if (VK_SHIFT == wParam) {
          isShiftPressed = true;
          break;
        }

        if (VK_CONTROL == wParam) {
          isCtrlPressed = true;
          break;
        }

        INPUT_COMMAND_TYPE::E command = INPUT_COMMAND_TYPE::kBackspace;
        if (getCommand((unsigned int)wParam, command)) {
          if (!onInputCommand.empty()) {
            onInputCommand(command);
          }
          return 0;
        }
        break;
      }

      case WM_SYSKEYUP:
      case WM_KEYUP:
      {
        if (VK_SHIFT == wParam) {
          isShiftPressed = false;
        }

        if (VK_CONTROL == wParam) {
          isCtrlPressed = false;
        }

        return 0;
      }

      case WM_CHAR:
      {
        //TODO: Not handling IME input

        //Ignore rarely used special command characters, usually triggered by
        //ctrl+key combinations. (We want to keep ctrl+key free for shortcuts
        //instead)
        if (23 >= wParam) {
          break;
        }

        switch (wParam)
        {
          case VK_ESCAPE:
            break;

          default:  //Displayable character
          {
            uint32 finalChar = static_cast<uint32>(wParam);

            if (!onCharInput.empty()) {
              onCharInput(finalChar);
            }

            return 0;
          }
        }

        break;
      }

      case WM_GE_SETCAPTURE:
        SetCapture(hWnd);
        break;

      case WM_GE_RELEASECAPTURE:
        ReleaseCapture();
        break;

      case WM_CAPTURECHANGED:
        if (!onMouseCaptureChanged.empty()) {
          onMouseCaptureChanged();
        }
        return 0;
      }

    return DefWindowProc(hWnd, uMsg, wParam, lParam);
  }
}
