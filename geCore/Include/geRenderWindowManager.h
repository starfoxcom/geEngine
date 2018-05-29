/*****************************************************************************/
/**
 * @file    geRenderWindowManager.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2018/05/26
 * @brief   Handles creation and internal updates relating to render windows.
 *
 * Handles creation and internal updates relating to render windows.
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
#include "geRenderWindow.h"

#include <geModule.h>
#include <geEvent.h>

namespace geEngineSDK {
  using std::atomic_uint;

  class GE_CORE_EXPORT RenderWindowManager : public Module<RenderWindowManager>
  {
   public:
    RenderWindowManager();
    ~RenderWindowManager() = default;

    /**
     * @brief Creates a new render window using the specified options.
     *        Optionally makes the created window a child of another window.
     */
    SPtr<RenderWindow>
    create(RENDER_WINDOW_DESC& desc, SPtr<RenderWindow> parentWindow);

    /**
     * @brief Called once per frame. Dispatches events.
     */
    void
    _update();

    /**
     * @brief Called by the core thread when window is destroyed.
     */
    void
    notifyWindowDestroyed(RenderWindow* window);

    /**
     * @brief Called by the core thread when window receives focus.
     */
    void
    notifyFocusReceived(geCoreThread::RenderWindow* window);

    /**
     * @brief Called by the core thread when window loses focus.
     */
    void
    notifyFocusLost(geCoreThread::RenderWindow* window);

    /**
     * @brief Called by the core thread when window is moved or resized.
     */
    void
    notifyMovedOrResized(geCoreThread::RenderWindow* window);

    /**
     * @brief Called by the core thread when mouse leaves a window.
     */
    void
    notifyMouseLeft(geCoreThread::RenderWindow* window);

    /**
     * @brief Called by the core thread when the user requests for the window
     *        to close.
     */
    void
    notifyCloseRequested(geCoreThread::RenderWindow* coreWindow);

    /**
     * @brief Called by the sim thread when window properties change.
     */
    void
    notifySyncDataDirty(geCoreThread::RenderWindow* coreWindow);

    /**
     * @brief Returns a list of all open render windows.
     */
    Vector<RenderWindow*>
    getRenderWindows() const;

    /**
     * @brief Returns the window that is currently the top-most modal window.
     *        Returns null if no modal windows are active.
     */
    RenderWindow*
    getTopMostModal() const;

    /**
     * @brief Event that is triggered when a window gains focus.
     */
    Event<void(RenderWindow&)> onFocusGained;

    /**
     * @brief Event that is triggered when a window loses focus.
     */
    Event<void(RenderWindow&)> onFocusLost;

    /**
     * @brief Event that is triggered when mouse leaves a window.
     */
    Event<void(RenderWindow&)> onMouseLeftWindow;

   protected:
    friend class RenderWindow;

    /**
     * @brief Finds a sim thread equivalent of the provided core thread window
     *        implementation.
     */
    RenderWindow*
    getNonCore(const geCoreThread::RenderWindow* window) const;

    /**
     * @copydoc create
     */
    virtual SPtr<RenderWindow>
    createImpl(RENDER_WINDOW_DESC& desc,
               uint32 windowId,
               const SPtr<RenderWindow>& parentWindow) = 0;

   protected:
    mutable Mutex m_windowMutex;
    Map<uint32, RenderWindow*> m_windows;
    Vector<RenderWindow*> m_modalWindowStack;

    RenderWindow* m_windowInFocus;
    RenderWindow* m_newWindowInFocus;
    Vector<RenderWindow*> m_movedOrResizedWindows;
    Vector<RenderWindow*> m_mouseLeftWindows;
    Vector<RenderWindow*> m_closeRequestedWindows;
    UnorderedSet<RenderWindow*> m_dirtyProperties;
  };

  namespace geCoreThread {
    /**
     * @brief Handles creation and internal updates relating to render windows.
     * @note  Core thread only.
     */
    class GE_CORE_EXPORT RenderWindowManager : public Module<RenderWindowManager>
    {
     public:
      RenderWindowManager();

      /**
       * @brief Called once per frame. Dispatches events.
       */
      void
      _update();

      /**
       * @brief Called by the core thread when window properties change.
       */
      void
      notifySyncDataDirty(RenderWindow* window);

      /**
       * @brief Returns a list of all open render windows.
       */
      Vector<RenderWindow*>
      getRenderWindows() const;

     protected:
      friend class RenderWindow;
      friend class geEngineSDK::RenderWindow;
      friend class geEngineSDK::RenderWindowManager;

      /**
       * @brief Called whenever a window is created.
       */
      void
      windowCreated(RenderWindow* window);

      /**
       * @brief Called by the core thread when window is destroyed.
       */
      void
      windowDestroyed(RenderWindow* window);

      mutable Mutex m_windowMutex;
      Vector<RenderWindow*> m_createdWindows;
      UnorderedSet<RenderWindow*> m_dirtyProperties;
      atomic_uint m_nextWindowId;
    };
  }
}
