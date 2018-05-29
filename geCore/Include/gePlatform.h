/*****************************************************************************/
/**
 * @file    gePlatform.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2018/05/26
 * @brief   Provides access to various operating system functions.
 *
 * Provides access to various operating system functions, including the main
 * message pump.
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
#include "geInputFwd.h"

#include <geVector2I.h>
#include <geBox2DI.h>
#include <geEvent.h>

namespace geEngineSDK {
  /**
   * @brief Contains values representing default mouse cursor types.
   */
  namespace PLATFORM_CURSOR_TYPE {
    enum E {
      kArrow,
      kWait,
      kIBeam,
      kHelp,
      kHand,
      kSizeAll,
      kSizeNESW,
      kSizeNS,
      kSizeNWSE,
      kSizeWE
    };
  }

  /**
   * @brief Contains values representing window non client areas.
   * @note  These are used for things like resize/move and tell the OS where
   *        each of those areas are on our window.
   */
  namespace NON_CLIENT_AREA_BORDER_TYPE {
    enum E {
      kTopLeft,
      kTop,
      kTopRight,
      kLeft,
      kRight,
      kBottomLeft,
      kBottom,
      kBottomRight
    };
  }

  /**
   * @brief Types of mouse buttons provided by the OS.
   */
  namespace OS_MOUSE_BUTTON {
    enum E {
      kLeft,
      kMiddle,
      kRight,
      kCount
    };
  }

  /**
   * @brief Describes pointer (mouse, touch) states as reported by the OS.
   */
  struct GE_CORE_EXPORT OSPointerButtonStates
  {
    OSPointerButtonStates() {
      mouseButtons[0] = false;
      mouseButtons[1] = false;
      mouseButtons[2] = false;
      shift = false;
      ctrl = false;
    }

    bool mouseButtons[(uint32)OS_MOUSE_BUTTON::kCount];
    bool shift;
    bool ctrl;
  };

  /**
   * @brief Represents a specific non client area used for window resizing.
   */
  struct GE_CORE_EXPORT NonClientResizeArea
  {
    NON_CLIENT_AREA_BORDER_TYPE::E type;
    Box2DI area;
  };

  /**
   * @brief Contains a list of window move and resize non client areas.
   */
  struct GE_CORE_EXPORT WindowNonClientAreaData
  {
    Vector<NonClientResizeArea> resizeAreas;
    Vector<Box2DI> moveAreas;
  };

  /**
   * @brief Provides access to various operating system functions, including
   *        the main message pump.
   */
  class GE_CORE_EXPORT Platform
  {
   public:
    struct Pimpl;

    Platform() = default;
    virtual ~Platform();

    /**
     * @brief Retrieves the cursor position in screen coordinates.
     * @note  Thread safe.
     */
    static Vector2I
    getCursorPosition();

    /**
     * @brief Moves the cursor to the specified screen position.
     * @note  Thread safe.
     */
    static void
    setCursorPosition(const Vector2I& screenPos);

    /**
     * @brief Capture mouse to this window so that we get mouse input even if
     *        the mouse leaves the window area.
     * @note  Thread safe.
     */
    static void
    captureMouse(const RenderWindow& window);

    /**
     * @brief Releases the mouse capture set by captureMouse().
     * @note  Thread safe.
     */
    static void
    releaseMouseCapture();

    /**
     * @brief Checks if provided over screen position is over the specified
     *        window.
     */
    static bool
    isPointOverWindow(const RenderWindow& window, const Vector2I& screenPos);

    /**
     * @brief Limit cursor movement to the specified window.
     * @note  Thread safe.
     */
    static void
    clipCursorToWindow(const RenderWindow& window);

    /**
     * @brief Clip cursor to specific area on the screen.
     * @note  Thread safe.
     */
    static void
    clipCursorToRect(const Box2DI& screenRect);

    /**
     * @brief Disables cursor clipping.
     * @note  Thread safe.
     */
    static void
    clipCursorDisable();

    /**
     * @brief Hides the cursor.
     * @note  Thread safe.
     */
    static void
    hideCursor();

    /**
     * @brief Shows the cursor.
     * @note  Thread safe.
     */
    static void
    showCursor();

    /**
     * @brief Query if the cursor is hidden.
     * @note  Thread safe.
     */
    static bool
    isCursorHidden();

    /**
     * @brief Sets a cursor using a custom image.
     * @param[in] pixelData Cursor image data.
     * @param[in] hotSpot   Offset on the cursor image to where the actual
     *            input happens (for example tip of the Arrow cursor).
     * @note  Thread safe.
     */
    static void
    setCursor(PixelData& pixelData, const Vector2I& hotSpot);

    /**
     * @brief Sets an icon for the main application window.
     * @param[in] pixelData Icon image data. This will be resized to the
     *            required icon size, depending on platform implementation.
     * @note  Thread safe.
     */
    static void
    setIcon(const PixelData& pixelData);

    /**
     * @brief Sets custom caption non client areas for the specified window.
              Using custom client areas will override window move/drag
              operation and trigger when user interacts with the custom area.
     * @note  Thread safe.
     * @note  All provided areas are relative to the specified window. Mostly
              useful for frameless windows that don't have typical caption bar.
     */
    static void
    setCaptionNonClientAreas(const geCoreThread::RenderWindow& window,
                             const Vector<Box2DI>& nonClientAreas);

    /**
     * @brief Sets custom non client areas for the specified window. Using
     *        custom client areas will override window resize operation and
     *        trigger when user interacts with the custom area.
     * @note  Thread safe.
     * @note  All provided areas are relative to the specified window. Mostly
     *        useful for frameless windows that don't have typical border.
     */
    static void
    setResizeNonClientAreas(const geCoreThread::RenderWindow& window,
                            const Vector<NonClientResizeArea>& nonClientAreas);

    /**
     * @brief Resets the non client areas for the specified windows and allows
     *        the platform to use the default values.
     * @note  Thread safe.
     */
    static void
    resetNonClientAreas(const geCoreThread::RenderWindow& window);

    /**
     * @brief Causes the current thread to pause execution for the specified
     *        amount of time.
     * @param[in] duration  Duration in milliseconds. Providing zero will give
     *            up the current time-slice.
     * @note  This method relies on timer granularity being set to
     *        1 millisecond. If it is not, you can expect this method to
     *        potentially take significantly longer if you are providing it
     *        with low ms values (<10).
     */
    static void
    sleep(uint32 duration);

    /**
     * @brief Opens the provided folder using the default application, as
     *        specified by the operating system.
     * @param[in] path  Absolute path to the folder to open.
     */
    static void
    openFolder(const Path& path);

    /**
     * @brief Adds a string to the clipboard.
     * @note  Thread safe.
     */
    static void
    copyToClipboard(const String& string);

    /**
     * @brief Reads a string from the clipboard and returns it. If there is no
     *        string in the clipboard it returns an empty string.
     * @note  Both wide and normal strings will be read, but normal strings
     *        will be converted to a wide string before returning.
     * @note  Thread safe.
     */
    static String
    copyFromClipboard();

    /**
     * @brief Converts a keyboard key-code to a Unicode character.
     * @note  Normally this will output a single character, but it can happen
     *        it outputs multiple in case a accent/diacritic character could
     *        not be combined with the virtual key into a single character.
     */
    static String
    keyCodeToUnicode(uint32 keyCode);

    /**
     * @brief Message pump. Processes OS messages and returns when it's free.
     * @note  Core thread only.
     */
    static void
    _messagePump();

    /**
     * @brief Called during application start up from the sim thread.
     *        Must be called before any other operations are done.
     */
    static void
    _startUp();

    /**
     * @brief Called once per frame from the sim thread.
     */
    static void
    _update();

    /**
     * @brief Called once per frame from the core thread.
     */
    static void
    _coreUpdate();

    /**
     * @brief Called during application shut down from the sim thread.
     */
    static void
    _shutDown();

    /**
     * @brief Triggered whenever the pointer moves.
     * @note  Core thread only.
     */
    static Event<void(const Vector2I&,
                      const OSPointerButtonStates&)> onCursorMoved;

    /**
     * @brief Triggered whenever a pointer button is pressed.
     * @note  Core thread only.
     */
    static Event<void(const Vector2I&,
                      OS_MOUSE_BUTTON::E button,
                      const OSPointerButtonStates&)> onCursorButtonPressed;

    /**
     * @brief Triggered whenever pointer button is released.
     * @note  Core thread only.
     */
    static Event<void(const Vector2I&,
                      OS_MOUSE_BUTTON::E button,
                      const OSPointerButtonStates&)> onCursorButtonReleased;

    /**
     * @brief Triggered whenever a pointer button is double clicked.
     * @note  Core thread only.
     */
    static Event<void(const Vector2I&,
                      const OSPointerButtonStates&)> onCursorDoubleClick;

    /**
     * @brief Triggered whenever an input command is entered.
     * @note  Core thread only.
     */
    static Event<void(INPUT_COMMAND_TYPE::E)> onInputCommand;

    /**
     * @brief Triggered whenever the mouse wheel is scrolled.
     * @note  Core thread only.
     */
    static Event<void(float)> onMouseWheelScrolled;

    /**
     * @brief Triggered whenever a character is entered.
     * @note  Core thread only.
     */
    static Event<void(uint32)> onCharInput;

    /**
     * @brief Triggered whenever mouse capture state for the window is changed
     *        (it receives or loses it).
     * @note  Core thread only.
     */
    static Event<void()> onMouseCaptureChanged;
   protected:
    static Pimpl* m_data;
  };
}
