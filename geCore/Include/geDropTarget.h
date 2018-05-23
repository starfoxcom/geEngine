/*****************************************************************************/
/**
 * @file    geDropTarget.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2018/05/11
 * @brief   Allow you to register a certain portion of a window as a drop area.
 *
 * Drop targets allow you to register a certain portion of a window as a drop
 * target that accepts certain drop types from the OS (platform) specific drag
 * and drop system. Accepted drop types are provided by the OS and include
 * things like file and item dragging.
 *
 * You'll receive events with the specified drop area as long as it is active.
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

#include <geEvent.h>
#include <geBox2DI.h>

namespace geEngineSDK {
  /**
   * @brief Type of drop event type. This is used when dragging items over drop
   *        targets.
   */
  namespace DROP_TARGET_TYPE {
    enum E {
      kFileList,
      kNone
    };
  }

  /**
   * @brief Drop targets allow you to register a certain portion of a window as
   *        a drop target that accepts certain drop types from the OS
   *        (platform) specific drag and drop system. Accepted drop types are
   *        provided by the OS and include things like file and item dragging.
   * You will receive events with the specified drop area as long as it is
   * active.
   */
  class GE_CORE_EXPORT DropTarget final
  {
   public:
    ~DropTarget();

    /**
     * @brief Sets the drop target area, in local window coordinates.
     */
    void
    setArea(const Box2DI& area);

    /**
     * @brief Returns the drop target area, in local window coordinates.
     */
    const Box2DI&
    getArea() const {
      return m_area;
    }

    /**
     * @brief Gets the type of drop that this drop target is looking for.
     *        Only valid after a drop has been triggered.
     */
    DROP_TARGET_TYPE::E
    getDropType() const {
      return m_dropType;
    }

    /**
     * @brief Returns a list of files received by the drop target.
     *        Only valid after a drop of FileList type has been triggered.
     */
    const Vector<Path>&
    getFileList() const {
      return m_fileList;
    }

    /**
     * @brief Creates a new drop target. Any drop events that happen on the
     *        specified window's drop area will be reported through the
     *        target's events.
     * @param[in] window  Window to which the drop target will be attached to.
     * @param[in] area    Area, relative to the window, in which the drop
     *                    events are allowed.
     * @return  Newly created drop target.
     */
    static SPtr<DropTarget>
    create(const RenderWindow* window, const Box2DI& area);

    /**
     * @brief Triggered when a pointer is being dragged over the drop area.
     *        Provides window coordinates of the pointer position.
     */
    Event<void(int32, int32)> onDragOver;

    /**
     * @brief Triggered when the user completes a drop while pointer is over
     *        the drop area. Provides window coordinates of the pointer
     *        position.
     */
    Event<void(int32, int32)> onDrop;

    /**
     * @brief Triggered when a pointer enters the drop area.
     *        Provides window coordinates of the pointer position.
     */
    Event<void(int32, int32)> onEnter;

    /**
     * @brief Triggered when a pointer leaves the drop area.
     */
    Event<void()> onLeave;

    /**
     * @brief Clears all internal values.
     */
    void
    _clear();

    /**
     * @brief Sets the file list and marks the drop event as FileList.
     */
    void
    _setFileList(const Vector<Path>& fileList);

    /**
     * @brief Marks the drop area as inactive or active.
     */
    void
    _setActive(bool active) {
      m_active = active;
    }

    /**
     * @brief Checks is the specified position within the current drop area.
     *        Position should be in window local coordinates.
     */
    bool
    _isInside(const Vector2I& pos) const;

    /**
     * @brief Returns true if the drop target is active.
     */
    bool
    _isActive() const {
      return m_active;
    }

    /**
     * @brief Returns a render window this drop target is attached to.
     */
    const RenderWindow*
    _getOwnerWindow() const {
      return m_ownerWindow;
    }

   private:
    friend class Platform;

    DropTarget(const RenderWindow* ownerWindow, const Box2DI& area);

    Box2DI m_area;
    bool m_active;
    const RenderWindow* m_ownerWindow;

    DROP_TARGET_TYPE::E m_dropType;
    Vector<Path> m_fileList;
  };
}
