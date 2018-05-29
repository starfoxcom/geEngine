/*****************************************************************************/
/**
 * @file    geWin32DropTarget.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2018/05/26
 * @brief   This class queues all messages received by the OS.
 *
 * This class queues all messages receives by the OS (from the core thread),
 * and then executes the queue on sim thread. You should be wary of which
 * methods are allowed to be called from which thread.
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

#include <Win32/geMinWindows.h>
#include <geVector2I.h>
#include <oleidl.h>

namespace geEngineSDK {
  /**
   * @brief Type of drag and drop event.
   */
  namespace DROP_OP_TYPE {
    enum E {
      kDragOver,
      kDrop,
      kLeave
    };
  }

  /**
   * @brief Type of data that a drag and drop operation contains.
   */
  namespace DROP_OP_DATA_TYPE {
    enum E {
      kFileList,
      kNone
    };
  }

  class Win32DropTarget : public IDropTarget
  {
    /**
     * @brief Structure describing a drag and drop operation.
     */
    struct DropTargetOp
    {
      DropTargetOp(DROP_OP_TYPE::E _type, const Vector2I& _pos)
        : type(_type),
          position(_pos),
          dataType(DROP_OP_DATA_TYPE::kNone)
      {}

      DROP_OP_TYPE::E type;
      Vector2I position;

      DROP_OP_DATA_TYPE::E dataType;
      Vector<Path>* fileList;
    };

   public:
    Win32DropTarget(HWND hWnd);
    ~Win32DropTarget();

    void
    registerWithOS();

    void
    unregisterWithOS();

    HRESULT GE_STDCALL
    QueryInterface(REFIID iid, void** ppvObject) override;

    /**
     * @brief COM requirement. Increments objects reference count.
     */
    ULONG GE_STDCALL
    AddRef() override;

    /**
     * @brief COM requirement. Decreases the objects reference count and
     *        deletes the object if its zero.
     */
    ULONG GE_STDCALL
    Release() override;

    /**
     * @brief Called by the OS when user enters the drop target area while
     *        dragging an object.
     * @note  Called on core thread.
     */
    HRESULT GE_STDCALL
    DragEnter(IDataObject* pDataObj,
              DWORD grfKeyState,
              POINTL pt,
              DWORD* pdwEffect) override;

    /**
     * @brief Called by the OS while user continues to drag an object over the
     *        drop target.
     * @note  Called on core thread.
     */
    HRESULT GE_STDCALL
    DragOver(DWORD grfKeyState, POINTL pt, DWORD* pdwEffect) override;

    /**
    * @brief Called by the OS when user leaves the drop target.
    * @note  Called on core thread.
    */
    HRESULT GE_STDCALL
    DragLeave() override;

    /**
     * @brief Called by the OS when the user ends the drag operation while over
     *        the drop target.
     * @note  Called on core thread.
     */
    HRESULT GE_STDCALL
    Drop(IDataObject* pDataObj,
         DWORD grfKeyState,
         POINTL pt,
         DWORD* pdwEffect) override;

    /**
     * @brief Registers a new drop target to monitor.
     * @note  Sim thread only.
     */
    void
    registerDropTarget(DropTarget* dropTarget);

    /**
     * @brief Unregisters an existing drop target and stops monitoring it.
     * @note  Sim thread only.
     */
    void
    unregisterDropTarget(DropTarget* dropTarget);

    /**
     * @brief Gets the total number of monitored drop targets.
     * @note  Sim thread only.
     */
    uint32
    getNumDropTargets() const;

    /**
     * @brief Called every frame by the sim thread. Internal method.
     */
    void
    update();

   private:
    /**
     * @brief Check if we support the data in the provided drag and drop data object.
     */
    bool
    isDataValid(IDataObject* data);

    /**
     * @brief Gets a file list from data. Caller must ensure that the data actually contains a file list.
     */
    Vector<Path>*
    getFileListFromData(IDataObject* data);

   private:
    Vector<DropTarget*> m_dropTargets;

    LONG m_refCount;
    HWND m_hWnd;
    bool m_acceptDrag;

    Vector<DropTargetOp> m_queuedDropOps;
    Vector<Vector<Path>*> m_fileLists;

    Mutex m_sync;
  };
}
