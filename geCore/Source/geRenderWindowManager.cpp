/*****************************************************************************/
/**
 * @file    geRenderWindowManager.cpp
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2018/05/26
 * @brief   Handles creation and internal updates relating to render windows.
 *
 * Handles creation and internal updates relating to render windows.
 *
 * @bug     No known bugs.
 */
/*****************************************************************************/

/*****************************************************************************/
/**
 * Includes
 */
/*****************************************************************************/
#include "geRenderWindowManager.h"
#include "gePlatform.h"
#include "geCoreApplication.h"

namespace geEngineSDK {
  using std::memory_order_relaxed;
  using std::find;
  using std::swap;

  RenderWindowManager::RenderWindowManager()
    : m_windowInFocus(nullptr),
      m_newWindowInFocus(nullptr)
  {}

  SPtr<RenderWindow>
  RenderWindowManager::create(RENDER_WINDOW_DESC& desc,
                              SPtr<RenderWindow> parentWindow) {
    uint32 id = geCoreThread::RenderWindowManager::instance().
      m_nextWindowId.fetch_add(1, memory_order_relaxed);

    SPtr<RenderWindow> renderWindow = createImpl(desc, id, parentWindow);
    renderWindow->_setThisPtr(renderWindow);
    {
      Lock lock(m_windowMutex);
      m_windows[renderWindow->m_windowId] = renderWindow.get();
    }

    if (renderWindow->getProperties().m_isModal) {
      m_modalWindowStack.push_back(renderWindow.get());
    }
    renderWindow->initialize();

    return renderWindow;
  }

  void
  RenderWindowManager::notifyWindowDestroyed(RenderWindow* window) {
    {
      Lock lock(m_windowMutex);

      auto iterFind = find(begin(m_movedOrResizedWindows),
                           end(m_movedOrResizedWindows),
                           window);

      if (m_movedOrResizedWindows.end() != iterFind) {
        m_movedOrResizedWindows.erase(iterFind);
      }

      if (m_newWindowInFocus == window) {
        m_newWindowInFocus = nullptr;
      }

      m_windows.erase(window->m_windowId);
      m_dirtyProperties.erase(window);
    }

    {
      auto iterFind = find(begin(m_modalWindowStack),
                           end(m_modalWindowStack),
                           window);

      if (m_modalWindowStack.end() != iterFind) {
        m_modalWindowStack.erase(iterFind);
      }
    }
  }

  void
  RenderWindowManager::notifyFocusReceived(geCoreThread::RenderWindow* coreWindow) {
    Lock lock(m_windowMutex);
    RenderWindow* window = getNonCore(coreWindow);
    m_newWindowInFocus = window;
  }

  void
  RenderWindowManager::notifyFocusLost(geCoreThread::RenderWindow* /*coreWindow*/) {
    Lock lock(m_windowMutex);
    m_newWindowInFocus = nullptr;
  }

  void
  RenderWindowManager::notifyMovedOrResized(geCoreThread::RenderWindow* coreWindow) {
    Lock lock(m_windowMutex);
    RenderWindow* window = getNonCore(coreWindow);
    if (nullptr == window) {
      return;
    }

    auto iterFind = find(begin(m_movedOrResizedWindows),
                         end(m_movedOrResizedWindows),
                         window);

    if (end(m_movedOrResizedWindows) == iterFind) {
      m_movedOrResizedWindows.push_back(window);
    }
  }

  void
  RenderWindowManager::notifyMouseLeft(geCoreThread::RenderWindow* coreWindow) {
    Lock lock(m_windowMutex);

    RenderWindow* window = getNonCore(coreWindow);
    auto iterFind = find(begin(m_mouseLeftWindows),
                         end(m_mouseLeftWindows),
                         window);

    if (end(m_mouseLeftWindows) == iterFind) {
      m_mouseLeftWindows.push_back(window);
    }
  }

  void
  RenderWindowManager::notifyCloseRequested(geCoreThread::RenderWindow* coreWindow) {
    Lock lock(m_windowMutex);

    RenderWindow* window = getNonCore(coreWindow);
    auto iterFind = find(begin(m_closeRequestedWindows),
                         end(m_closeRequestedWindows),
                         window);

    if (end(m_closeRequestedWindows) == iterFind) {
      m_closeRequestedWindows.push_back(window);
    }
  }

  void
  RenderWindowManager::notifySyncDataDirty(geCoreThread::RenderWindow* coreWindow) {
    Lock lock(m_windowMutex);
    RenderWindow* window = getNonCore(coreWindow);

    if (nullptr != window) {
      m_dirtyProperties.insert(window);
    }
  }

  void
  RenderWindowManager::_update() {
    RenderWindow* newWinInFocus = nullptr;
    Vector<RenderWindow*> movedOrResizedWindows;
    Vector<RenderWindow*> mouseLeftWindows;
    Vector<RenderWindow*> closeRequestedWindows;
    {
      Lock lock(m_windowMutex);
      newWinInFocus = m_newWindowInFocus;

      swap(m_movedOrResizedWindows, movedOrResizedWindows);
      swap(m_mouseLeftWindows, mouseLeftWindows);

      for (auto& dirtyPropertyWindow : m_dirtyProperties) {
        dirtyPropertyWindow->syncProperties();
      }
      m_dirtyProperties.clear();

      swap(m_closeRequestedWindows, closeRequestedWindows);
    }

    if (m_windowInFocus != newWinInFocus) {
      if (nullptr != m_windowInFocus) {
        onFocusLost(*m_windowInFocus);
      }

      if (nullptr != newWinInFocus) {
        onFocusGained(*newWinInFocus);
      }

      m_windowInFocus = newWinInFocus;
    }

    for (auto& window : movedOrResizedWindows) {
      window->onResized();
    }

    if (!onMouseLeftWindow.empty()) {
      for (auto& window : mouseLeftWindows) {
        onMouseLeftWindow(*window);
      }
    }

    SPtr<RenderWindow>
    primaryWindow = g_coreApplication().getPrimaryWindow();
    for (auto& entry : closeRequestedWindows) {
      //Default behavior for primary window is to quit the app on close
      if (primaryWindow.get() == entry && entry->onCloseRequested.empty()) {
        g_coreApplication().quitRequested();
        continue;
      }

      entry->onCloseRequested();
    }
  }

  Vector<RenderWindow*>
  RenderWindowManager::getRenderWindows() const {
    Lock lock(m_windowMutex);

    Vector<RenderWindow*> windows;
    for (auto& windowPair : m_windows) {
      windows.push_back(windowPair.second);
    }
    return windows;
  }

  RenderWindow*
  RenderWindowManager::getTopMostModal() const {
    if (m_modalWindowStack.empty()) {
      return nullptr;
    }
    return m_modalWindowStack.back();
  }

  RenderWindow*
  RenderWindowManager::getNonCore(const geCoreThread::RenderWindow* window) const {
    auto iterFind = m_windows.find(window->m_windowId);

    if (m_windows.end() != iterFind) {
      return iterFind->second;
    }
    return nullptr;
  }

  namespace geCoreThread {
    RenderWindowManager::RenderWindowManager() {
      m_nextWindowId = 0;
    }

    void
    RenderWindowManager::_update() {
      Lock lock(m_windowMutex);

      for (auto& dirtyPropertyWindow : m_dirtyProperties) {
        dirtyPropertyWindow->syncProperties();
      }
      m_dirtyProperties.clear();
    }

    void
    RenderWindowManager::windowCreated(RenderWindow* window) {
      Lock lock(m_windowMutex);
      m_createdWindows.push_back(window);
    }

    void
    RenderWindowManager::windowDestroyed(RenderWindow* window) {
      {
        Lock lock(m_windowMutex);
        auto iterFind = find(begin(m_createdWindows),
                             end(m_createdWindows),
                             window);

        if (m_createdWindows.end() == iterFind) {
          GE_EXCEPT(InternalErrorException,
                    "Trying to destroy a window that is not in the created "
                    "windows list.");
        }

        m_createdWindows.erase(iterFind);
        m_dirtyProperties.erase(window);
      }
    }

    Vector<RenderWindow*>
    RenderWindowManager::getRenderWindows() const {
      Lock lock(m_windowMutex);
      return m_createdWindows;
    }

    void
    RenderWindowManager::notifySyncDataDirty(RenderWindow* window) {
      Lock lock(m_windowMutex);
      m_dirtyProperties.insert(window);
    }
  }
}
