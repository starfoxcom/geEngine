/*****************************************************************************/
/**
 * @file    geWin32Windows.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2015/02/18
 * @brief   Objects used to represents a Windows native window.
 *
 * Objects used to represents a Windows native window.
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
#include "gePrerequisitesUtil.h"
#include "Win32/geMinWindows.h"

namespace geEngineSDK {
  /**
   * @brief Descriptor used for creating a platform specific native window.
   */
  struct GE_UTILITY_EXPORT WINDOW_DESC
  {
    WINDOW_DESC()
      : module(nullptr),
        monitor(nullptr),
        parent(nullptr),
        external(nullptr),
        creationParams(nullptr),
        width(0),
        height(0),
        fullscreen(false),
        hidden(false),
        left(-1),
        top(-1),
        title(""),
        showTitleBar(true),
        showBorder(true),
        allowResize(true),
        outerDimensions(false),
        enableDoubleClick(true),
        toolWindow(false),
        backgroundPixels(nullptr),
        backgroundWidth(0),
        backgroundHeight(0),
        alphaBlending(false),
        modal(false),
        wndProc(nullptr) {}

    /**
     * @brief Instance to the local module.
     */
    HINSTANCE module;

    /**
     * @brief Handle of the monitor onto which to display the window.
     */
    HMONITOR monitor;

    /**
     * @brief Optional handle to the parent window if this window is to be a
     *        child of an existing window.
     */
    HWND parent;

    /**
     * @brief Optional external window handle if the window was created externally.
     */
    HWND external;

    /**
     * @brief Parameter that will be passed through the WM_CREATE message.
     */
    void* creationParams;
    
    /**
     * @brief Width of the window in pixels.
     */
    uint32 width;

    /**
     * @brief Height of the window in pixels.
     */
    uint32 height;

    /**
     * @brief Should the window be opened in fullscreen mode.
     */
    bool fullscreen;
    
    /**
     * @brief Should the window be hidden initially.
     */
    bool hidden;

    /**
     * @brief Window origin on X axis in pixels. -1 == screen center.
     *        Relative to provided monitor.
     */
    int32 left;

    /**
     * @brief Window origin on Y axis in pixels. -1 == screen center.
     *        Relative to provided monitor.
     */
    int32 top;

    /**
     * @brief Title of the window.
     */
    String title;

    /**
     * @brief Determines if the title-bar should be shown or not.
     */
    bool showTitleBar;

    /**
     * @brief Determines if the window border should be shown or not.
     */
    bool showBorder;

    /**
     * @brief Determines if the user can resize the window by dragging on the window edges.
     */
    bool allowResize;

    /**
     * @brief Do our dimensions include space for things like title-bar and border.
     */
    bool outerDimensions;

    /**
     * @brief Does window accept double-clicks.
     */
    bool enableDoubleClick;

    /**
     * @brief Tool windows have a different style than normal windows and can
     *        be created with no border or title bar.
     */
    bool toolWindow;

    /**
     * @brief Optional background image to apply to the window. This must be a buffer of size
     * backgroundWidth * backgroundHeight.
     */
    Color* backgroundPixels;

    /**
     * @brief Width of the background image. Only relevant if backgroundPixels is not null.
     */
    uint32 backgroundWidth;

    /**
     * @brief Width of the background image. Only relevant if backgroundPixels is not null.
     */
    uint32 backgroundHeight;

    /**
     * @brief If true the window will support transparency based on the alpha channel
     *        of the background image.
     */
    bool alphaBlending;
    
    /**
     * @brief When a modal window is open all other windows will be locked
     *        until modal window is closed.
     */
    bool modal;

    /**
     * @brief Pointer to a function that handles windows message processing.
     */
    WNDPROC wndProc;
  };

  /**
   * @brief Represents a Windows native window.
   */
  class GE_UTILITY_EXPORT Win32Window
  {
   public:
    explicit Win32Window(const WINDOW_DESC& desc);
    ~Win32Window();

    /**
     * @brief Returns position of the left-most border of the window, relative to the screen.
     */
    int32
    getLeft() const;

    /**
     * @brief Returns position of the top-most border of the window, relative to the screen.
     */
    int32
    getTop() const;

    /**
     * @brief Returns width of the window in pixels.
     */
    uint32
    getWidth() const;

    /**
     * @brief Returns height of the window in pixels.
     */
    uint32
    getHeight() const;

    /**
     * @brief Returns the native window handle.
     */
    HWND
    getHWnd() const;

    /**
     * @brief Hide or show the window.
     */
    void
    setHidden(bool hidden);

    /**
     * @brief Restores or minimizes the window.
     */
    void
    setActive(bool state);

    /**
     * @brief Minimizes the window to the task bar.
     */
    void
    minimize();

    /**
     * @brief Maximizes the window over the entire current screen.
     */
    void
    maximize();

    /**
     * @brief Restores the window to original position and size if it is
     *        minimized or maximized.
     */
    void
    restore();

    /**
     * @brief Change the size of the window.
     */
    void
    resize(uint32 width, uint32 height);

    /**
     * @brief Reposition the window.
     */
    void
    move(int32 left, int32 top);

    /**
     * @brief Converts screen position into window local position.
     */
    Vector2I
    screenToWindowPos(const Vector2I& screenPos) const;

    /**
     * @brief Converts window local position to screen position.
     */
    Vector2I
    windowToScreenPos(const Vector2I& windowPos) const;

    /**
     * @brief Returns the window style flags used for creating it.
     */
    uint32
    getStyle() const;

    /**
     * @brief Returns the extended window style flags used for creating it.
     */
    uint32
    getStyleEx() const;

    /**
     * @brief Called when window is moved or resized externally.
     */
    void
    _windowMovedOrResized();

    /**
     * @brief Enables all open windows. Enabled windows can receive mouse and
     *        keyboard input. This includes even windows disabled because there
     *        is a modal window on top of them.
     */
    static void
    _enableAllWindows();

    /**
     * @brief Restores disabled state of all windows that were disabled due to
     *        modal windows being on top of them. Companion method to
     *        _enableAllWindows() that can help restore original state after it
     *        is called.
     */
    static void
    _restoreModalWindows();

   private:
    friend class Win32WindowManager;

    struct Pimpl;
    Pimpl* m_windowData;

    static Vector<Win32Window*> s_allWindows;
    static Vector<Win32Window*> s_modalWindowStack;
    static Mutex s_windowsMutex;
  };
}
