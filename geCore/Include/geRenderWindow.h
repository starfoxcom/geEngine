/*****************************************************************************/
/**
 * @file    geRenderWindow.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2018/05/25
 * @brief   Operating system window with a specific position, size and style.
 *
 * Operating system window with a specific position, size and style.
 * Each window serves as a surface that can be rendered into by RenderAPI
 * operations.
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
#include "geRenderTarget.h"
#include "geVideoModeInfo.h"

#include <geVector2I.h>

namespace geEngineSDK {
  class RenderWindowManager;

  /**
   * @brief Types of events that a RenderWindow can be notified of.
   */
  namespace WINDOW_EVENT_TYPE {
    enum E {
      /**
       * Triggered when window size changes.
       */
      kResized,

      /**
       * Triggered when window position changes.
       */
      kMoved,

      /**
       * Triggered when window receives input focus.
       */
      kFocusReceived,

      /**
       * Triggered when window loses input focus.
       */
      kFocusLost,

      /**
       * Triggered when the window is minimized.
       */
      kMinimized,

      /**
       * Triggered when the window is expanded to cover the current screen.
       */
      kMaximized,

      /**
       * Triggered when the window leaves minimized or maximized state.
       */
      kRestored,

      /**
       * Triggered when the mouse pointer leaves the window area.
       */
      kMouseLeft,

      /**
       * Triggered when the user wants to close the window.
       */
      kCloseRequested,
    };
  }

  /**
   * @brief Structure that is used for initializing a render window.
   */
  struct GE_CORE_EXPORT RENDER_WINDOW_DESC
  {
    RENDER_WINDOW_DESC()
      : fullscreen(false),
        vsync(false),
        vsyncInterval(1),
        hidden(false),
        depthBuffer(true),
        multisampleCount(0),
        multisampleHint(""),
        gamma(false),
        left(-1),
        top(-1),
        title(""),
        showTitleBar(true),
        showBorder(true),
        allowResize(true),
        toolWindow(false),
        modal(false),
        hideUntilSwap(false)
    {}

    /**
     * Output monitor, frame buffer resize and refresh rate.
     */
    VideoMode videoMode;

    /**
     * Should the window be opened in fullscreen mode.
     */
    bool fullscreen;

    /**
     * Should the window wait for vertical sync before swapping buffers.
     */
    bool vsync;

    /**
     * Determines how many vsync intervals occur per frame.
     * FPS = refreshRate / interval. Usually 1 when vsync active.
     */
    uint32 vsyncInterval;
    
    /**
     * Should the window be hidden initially.
     */
    bool hidden;
    
    /**
     * Should the window be created with a depth/stencil buffer.
     */
    bool depthBuffer;
    
    /**
     * If higher than 1, texture containing multiple samples per pixel is
     * created.
     */
    uint32 multisampleCount;
    
    /**
     * Hint about what kind of multisampling to use. Render system specific.
     */
    String multisampleHint;
    
    /**
     * Should the written color pixels be gamma corrected before write.
     */
    bool gamma;
    
    /**
     * Window origin on X axis in pixels. -1 == screen center.
     * Relative to monitor provided in videoMode.
     */
    int32 left;
    
    /**
     * Window origin on Y axis in pixels. -1 == screen center.
     * Relative to monitor provided in videoMode.
     */
    int32 top;
    
    /**
     * Title of the window.
     */
    String title;
    
    /**
     * Determines if the title-bar should be shown or not.
     */
    bool showTitleBar;
    
    /**
     * Determines if the window border should be shown or not.
     */
    bool showBorder;
    
    /**
     * Determines if the user can resize the window by dragging on the window
     * edges.
     */
    bool allowResize;

    /**
     * Tool windows have no task bar entry and always remain on top of their
     * parent window.
     */
    bool toolWindow;

    /**
     * When a modal window is open all other windows will be locked until modal
     * window is closed.
     */
    bool modal;

    /**
     * Window will be created as hidden and only be shown when the first
     * framebuffer swap happens.
     */
    bool hideUntilSwap;

    /**
     * Platform-specific creation options.
     */
    NameValuePairList platformSpecific;
  };

  /**
   * @brief Contains various properties that describe a render window.
   */
  class GE_CORE_EXPORT RenderWindowProperties : public RenderTargetProperties
  {
   public:
    RenderWindowProperties(const RENDER_WINDOW_DESC& desc);
    virtual ~RenderWindowProperties() = default;

    /**
     * @brief True if window is running in fullscreen mode.
     */
    bool m_isFullScreen = false;

    /**
     * @brief Horizontal origin of the window in pixels.
     */
    int32 m_left = 0;

    /**
     * @brief Vertical origin of the window in pixels.
     */
    int32 m_top = 0;

    /**
     * @brief Indicates whether the window currently has keyboard focus.
     */
    bool m_hasFocus = false;

    /**
     * @brief True if the window is hidden.
     */
    bool m_isHidden = false;

    /**
     * @brief True if the window is modal (blocks interaction with any
     *        non-modal window until closed).
     */
    bool m_isModal = false;

    /**
     * @brief True if the window is maximized.
     */
    bool m_isMaximized = false;
  };

  /**
   * @brief Operating system window with a specific position, size and style.
   *        Each window serves as a surface that can be rendered into by
   *        RenderAPI operations.
   */
  class GE_CORE_EXPORT RenderWindow : public RenderTarget
  {
   public:
    virtual ~RenderWindow() = default;

    /**
     * @brief Converts screen position into window local position.
     */
    virtual Vector2I
    screenToWindowPos(const Vector2I& screenPos) const = 0;

    /**
     * @brief Converts window local position to screen position.
     */
    virtual Vector2I
    windowToScreenPos(const Vector2I& windowPos) const = 0;

    /**
     * @brief Resize the window to specified width and height in pixels.
     * @param[in] width   Width of the window in pixels.
     * @param[in] height  Height of the window in pixels.
     */
    virtual void
    resize(uint32 width, uint32 height);

    /**
     * @brief Move the window to specified screen coordinates.
     * @param[in] left  Position of the left border of the window on screen.
     * @param[in] top   Position of the top border of the window on screen.
     * @note This is an @ref asyncMethod "asynchronous method".
     */
    virtual void
    move(int32 left, int32 top);

    /**
     * @brief Hides the window.
     * @note This is an @ref asyncMethod "asynchronous method".
     */
    virtual void
    hide();

    /**
     * @brief Shows a previously hidden window.
     * @note This is an @ref asyncMethod "asynchronous method".
     */
    virtual void
    show();

    /**
     * @copydoc geCoreThread::RenderWindow::minimize
     * @note This is an @ref asyncMethod "asynchronous method".
     */
    virtual void
    minimize();

    /**
     * @copydoc geCoreThread::RenderWindow::maximize
     * @note This is an @ref asyncMethod "asynchronous method".
     */
    virtual void
    maximize();

    /**
     * @copydoc geCoreThread::RenderWindow::restore
     * @note This is an @ref asyncMethod "asynchronous method".
     */
    virtual void
    restore();

    /**
     * @copydoc geCoreThread::RenderWindow::setFullscreen
                  (uint32, uint32, float, uint32)
     * @note This is an @ref asyncMethod "asynchronous method".
     */
    virtual void
    setFullscreen(uint32 width,
                  uint32 height,
                  float refreshRate = 60.0f,
                  uint32 monitorIdx = 0);

    /**
     * @copydoc geCoreThread::RenderWindow::setFullscreen(const VideoMode&)
     * @note This is an @ref asyncMethod "asynchronous method".
     */
    virtual void
    setFullscreen(const VideoMode& videoMode);

    /**
     * @copydoc geCoreThread::RenderWindow::setWindowed
     * @note This is an @ref asyncMethod "asynchronous method".
     */
    virtual void
    setWindowed(uint32 width, uint32 height);

    /**
     * @brief Retrieves a core implementation of a render window usable only
     *        from the core thread.
     */
    SPtr<geCoreThread::RenderWindow>
    getCore() const;

    /**
     * @brief Returns properties that describe the render window.
     */
    const RenderWindowProperties&
    getProperties() const;

    /**
     * @brief Closes and destroys the window.
     */
    void
    destroy() override;

    /**
     * @brief Creates a new render window using the specified options.
     *        Optionally makes the created window a child of another window.
     */
    static SPtr<RenderWindow>
    create(RENDER_WINDOW_DESC& desc,
           SPtr<RenderWindow> parentWindow = nullptr);

    /**
     * @brief Triggers when the OS requests that the window is closed
     *        (e.g. user clicks on the X button in the title bar).
     */
    Event<void()> onCloseRequested;

    /**
     * @brief Notifies the window that a specific event occurred.
     *        Usually called by the platform specific main event loop.
     */
    void
    _notifyWindowEvent(WINDOW_EVENT_TYPE::E type);

    /**
     * @brief Method that triggers whenever the window changes size or position.
     */
    virtual void
    _windowMovedOrResized() {}

   protected:
    friend class RenderWindowManager;

    RenderWindow(const RENDER_WINDOW_DESC& desc, uint32 windowId);

    /**
     * @brief Returns render window properties that may be edited.
     */
    RenderWindowProperties&
    getMutableProperties();

    /**
     * @brief Updates window properties from the synced property data.
     */
    virtual void
    syncProperties() = 0;

   protected:
    RENDER_WINDOW_DESC m_desc;
    uint32 m_windowId;
  };

  namespace geCoreThread {
    /**
     * @brief Core thread counterpart of bs::RenderWindow.
     */
    class GE_CORE_EXPORT RenderWindow : public RenderTarget
    {
     public:
      RenderWindow(const RENDER_WINDOW_DESC& desc, uint32 windowId);
      virtual ~RenderWindow();

      /**
       * @brief Switches the window to fullscreen mode.
       *        Child windows cannot go into fullscreen mode.
       * @param[in] width     Width of the window frame buffer in pixels.
       * @param[in] height    Height of the window frame buffer in pixels.
       * @param[in] refreshRate Refresh rate of the window in Hertz.
       * @param[in] monitorIdx  Index of the monitor to go fullscreen on.
       * @note  If the exact provided mode isn't available, closest one is used
       *        instead.
       */
      virtual void
      setFullscreen(uint32 width,
                    uint32 height,
                    float refreshRate = 60.0f,
                    uint32 monitorIdx = 0) {
        GE_UNREFERENCED_PARAMETER(width);
        GE_UNREFERENCED_PARAMETER(height);
        GE_UNREFERENCED_PARAMETER(refreshRate);
        GE_UNREFERENCED_PARAMETER(monitorIdx);
      }

      /**
       * @brief Switches the window to fullscreen mode.
       *        Child windows cannot go into fullscreen mode.
       * @param[in]	videoMode	Mode retrieved from VideoModeInfo in RenderAPI.
       */
      virtual void
      setFullscreen(const VideoMode& videoMode) {
        GE_UNREFERENCED_PARAMETER(videoMode);
      }

      /**
       * @brief Switches the window to windowed mode.
       * @param[in] width   Window width in pixels.
       * @param[in] height  Window height in pixels.
       */
      virtual void
      setWindowed(uint32 width, uint32 height) {
        GE_UNREFERENCED_PARAMETER(width);
        GE_UNREFERENCED_PARAMETER(height);
      }

      /**
       * @brief Hide or show the window.
       */
      virtual void
      setHidden(bool hidden);

      /**
       * @brief Makes the render target active or inactive. (for example in the
       *        case of a window, it will hide or restore the window).
       */
      virtual void
      setActive(bool state);

      /**
       * @brief Minimizes the window to the taskbar.
       */
      virtual void
      minimize() {}

      /**
       * @brief Maximizes the window over the entire current screen.
       */
      virtual void
      maximize() {}

      /**
       * @brief Restores the window to original position and size if it is
       *        minimized or maximized.
       */
      virtual void
      restore() {}

      /**
       * @brief Change the size of the window.
       */
      virtual void
      resize(uint32 width, uint32 height) = 0;

      /**
       * @brief Reposition the window.
       */
      virtual void
      move(int32 left, int32 top) = 0;

      /**
       * @brief Enables or disables vertical synchronization. When enabled the
       *        system will wait for monitor refresh before presenting the back
       *        buffer. This eliminates tearing but can result in increased
       *        input lag.
       * @param enabled   True to enable vsync, false to disable.
       * @param interval  Interval at which to perform the sync. Value of one
       *        means the sync will be performed for each monitor refresh,
       *        value of two means it will be performs for every second
       *        (half the rate), and so on.
       */
      virtual void
      setVSync(bool enabled, uint32 interval = 1) = 0;

      /**
       * @brief Returns properties that describe the render window.
       */
      const RenderWindowProperties&
      getProperties() const;

      /**
       * @brief Notifies the window that a specific event occurred. Usually
       *        called by the platform specific main event loop.
       */
      void
      _notifyWindowEvent(WINDOW_EVENT_TYPE::E type);

      /**
       * @brief Method that triggers whenever the window changes size or
       *        position.
       */
      virtual void
      _windowMovedOrResized() {}
     protected:
      friend class geEngineSDK::RenderWindow;
      friend class RenderWindowManager;
      friend class geEngineSDK::RenderWindowManager;

      /**
       * @brief Returns window properties that are always kept in sync between
       *        core and sim threads.
       * @note  Used for keeping up what are the most up to date settings.
       */
      virtual RenderWindowProperties&
      getSyncedProperties() = 0;

      /**
       * @brief Updates window properties from the synced property data.
       */
      virtual void
      syncProperties() = 0;

      RENDER_WINDOW_DESC m_desc;
      SpinLock m_lock;
      uint32 m_windowId;
    };
  }
}
