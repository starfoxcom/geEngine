/*****************************************************************************/
/**
 * @file    geWin32DropTarget.cpp
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

/*****************************************************************************/
/**
 * Includes
 */
/*****************************************************************************/
#include "Win32/geWin32DropTarget.h"
#include "geDropTarget.h"
#include "Win32/geWin32Platform.h"

#include <geUnicode.h>
#include <shellapi.h>

namespace geEngineSDK {
  using std::find;

  DropTarget::DropTarget(const RenderWindow* ownerWindow, const Box2DI& area)
    : m_area(area),
      m_active(false),
      m_ownerWindow(ownerWindow),
      m_dropType(DROP_TARGET_TYPE::kNone) {
    Win32Platform::registerDropTarget(this);
  }

  DropTarget::~DropTarget() {
    Win32Platform::unregisterDropTarget(this);
    _clear();
  }

  void
  DropTarget::setArea(const Box2DI& area) {
    m_area = area;
  }

  Win32DropTarget::Win32DropTarget(HWND hWnd)
    : m_refCount(1),
      m_hWnd(hWnd),
      m_acceptDrag(false)
  {}

  Win32DropTarget::~Win32DropTarget() {
    Lock lock(m_sync);

    for (auto& fileList : m_fileLists) {
      ge_delete(fileList);
    }

    m_fileLists.clear();
    m_queuedDropOps.clear();
  }

  void
  Win32DropTarget::registerWithOS() {
    CoLockObjectExternal(this, TRUE, FALSE);
    RegisterDragDrop(m_hWnd, this);
  }

  void
  Win32DropTarget::unregisterWithOS() {
    RevokeDragDrop(m_hWnd);
    CoLockObjectExternal(this, FALSE, FALSE);
  }

  HRESULT GE_STDCALL
    Win32DropTarget::QueryInterface(REFIID iid, void** ppvObject) {
    if (IID_IDropTarget == iid || IID_IUnknown == iid) {
      AddRef();
      *ppvObject = this;
      return S_OK;
    }

    *ppvObject = nullptr;
    return E_NOINTERFACE;
  }

  ULONG GE_STDCALL
  Win32DropTarget::AddRef() {
    return InterlockedIncrement(&m_refCount);
  }

  ULONG GE_STDCALL
  Win32DropTarget::Release() {
    LONG count = InterlockedDecrement(&m_refCount);
    if (0 == count) {
      ge_delete(this);
      return 0;
    }

    return count;
  }

  HRESULT GE_STDCALL
  Win32DropTarget::DragEnter(IDataObject* pDataObj,
                             DWORD /*grfKeyState*/,
                             POINTL pt,
                             DWORD* pdwEffect) {
    *pdwEffect = DROPEFFECT_LINK;

    m_acceptDrag = isDataValid(pDataObj);
    if (!m_acceptDrag) {
      return S_OK;
    }

    {
      Lock lock(m_sync);
      m_fileLists.push_back(getFileListFromData(pDataObj));

      ScreenToClient(m_hWnd, reinterpret_cast<POINT*>(&pt));
      m_queuedDropOps.emplace_back(DROP_OP_TYPE::kDragOver,
                                   Vector2I(static_cast<int32>(pt.x),
                                            static_cast<int32>(pt.y)));

      DropTargetOp& op = m_queuedDropOps.back();
      op.dataType = DROP_OP_DATA_TYPE::kFileList;
      op.fileList = m_fileLists.back();
    }

    return S_OK;
  }

  HRESULT GE_STDCALL
  Win32DropTarget::DragOver(DWORD /*grfKeyState*/,
                            POINTL pt,
                            DWORD* pdwEffect) {
    *pdwEffect = DROPEFFECT_LINK;

    if (!m_acceptDrag) {
      return S_OK;
    }

    {
      Lock lock(m_sync);
      ScreenToClient(m_hWnd, reinterpret_cast<POINT*>(&pt));
      m_queuedDropOps.emplace_back(DROP_OP_TYPE::kDragOver,
                                   Vector2I(static_cast<int32>(pt.x),
                                            static_cast<int32>(pt.y)));

      DropTargetOp& op = m_queuedDropOps.back();
      op.dataType = DROP_OP_DATA_TYPE::kFileList;
      op.fileList = m_fileLists.back();
    }

    return S_OK;
  }

  HRESULT GE_STDCALL
  Win32DropTarget::DragLeave() {
    {
      Lock lock(m_sync);
      m_queuedDropOps.emplace_back(DROP_OP_TYPE::kLeave, Vector2I());

      DropTargetOp& op = m_queuedDropOps.back();
      op.dataType = DROP_OP_DATA_TYPE::kFileList;
      op.fileList = m_fileLists.back();
    }

    return S_OK;
  }

  HRESULT GE_STDCALL
  Win32DropTarget::Drop(IDataObject* pDataObj,
                        DWORD /*grfKeyState*/,
                        POINTL pt,
                        DWORD* pdwEffect) {
    *pdwEffect = DROPEFFECT_LINK;
    m_acceptDrag = false;

    if (!isDataValid(pDataObj)) {
      return S_OK;
    }

    {
      Lock lock(m_sync);
      m_fileLists.push_back(getFileListFromData(pDataObj));

      ScreenToClient(m_hWnd, (POINT *)&pt);
      m_queuedDropOps.emplace_back(DROP_OP_TYPE::kDrop,
                                   Vector2I(static_cast<int32>(pt.x),
                                            static_cast<int32>(pt.y)));

      DropTargetOp& op = m_queuedDropOps.back();
      op.dataType = DROP_OP_DATA_TYPE::kFileList;
      op.fileList = m_fileLists.back();
    }

    return S_OK;
  }

  void
  Win32DropTarget::registerDropTarget(DropTarget* dropTarget) {
    m_dropTargets.push_back(dropTarget);
  }

  void
  Win32DropTarget::unregisterDropTarget(DropTarget* dropTarget) {
    auto findIter = find(begin(m_dropTargets), end(m_dropTargets), dropTarget);
    if (findIter != m_dropTargets.end()) {
      m_dropTargets.erase(findIter);
    }
  }

  uint32
  Win32DropTarget::getNumDropTargets() const {
    return static_cast<uint32>(m_dropTargets.size());
  }

  void
  Win32DropTarget::update() {
    Lock lock(m_sync);

    for (auto& op : m_queuedDropOps) {
      for (auto& target : m_dropTargets) {
        if (DROP_OP_TYPE::kLeave != op.type) {
          if (target->_isInside(op.position)) {
            if (!target->_isActive()) {
              target->_setFileList(*op.fileList);
              target->_setActive(true);
              target->onEnter(op.position.x, op.position.y);
            }

            if (DROP_OP_TYPE::kDragOver == op.type) {
              target->onDragOver(op.position.x, op.position.y);
            }
            else if (DROP_OP_TYPE::kDrop == op.type) {
              target->_setFileList(*op.fileList);
              target->onDrop(op.position.x, op.position.y);
            }
          }
          else {
            if (target->_isActive()) {
              target->onLeave();
              target->_clear();
              target->_setActive(false);
            }
          }
        }
        else {
          if (target->_isActive()) {
            target->onLeave();
            target->_clear();
            target->_setActive(false);
          }
        }
      }

      if (op.type == DROP_OP_TYPE::kLeave || op.type == DROP_OP_TYPE::kDrop) {
        while (!m_fileLists.empty()) {
          bool done = m_fileLists[0] == op.fileList;
          ge_delete(m_fileLists[0]);
          m_fileLists.erase(m_fileLists.begin());
          if (done) {
            break;
          }
        }
      }
    }

    m_queuedDropOps.clear();
  }

  bool
  Win32DropTarget::isDataValid(IDataObject* data) {
    //TODO: Currently only supports file drag & drop, so only CF_HDROP is used
    FORMATETC fmtetc = { CF_HDROP,
                         nullptr,
                         DVASPECT_CONTENT,
                         -1,
                         TYMED_HGLOBAL };
    return data->QueryGetData(&fmtetc) == S_OK ? true : false;
  }

  /**
   * @brief Gets a file list from data. Caller must ensure that the data
   *        actually contains a file list.
   */
  Vector<Path>*
  Win32DropTarget::getFileListFromData(IDataObject* data) {
    FORMATETC fmtetc = { CF_HDROP,
                         nullptr,
                         DVASPECT_CONTENT,
                         -1,
                         TYMED_HGLOBAL };
    STGMEDIUM stgmed;

    auto* files = ge_new<Vector<Path>>();
    if (data->GetData(&fmtetc, &stgmed) == S_OK) {
      PVOID pData = GlobalLock(stgmed.hGlobal);
      auto hDrop = reinterpret_cast<HDROP>(pData);
      UINT numFiles = DragQueryFileW(hDrop, 0xFFFFFFFF, nullptr, 0);

      files->resize(numFiles);
      for (UINT i = 0; i < numFiles; ++i)
      {
        UINT numChars = DragQueryFileW(hDrop, i, nullptr, 0) + 1;
        wchar_t* buffer = reinterpret_cast<wchar_t*>(ge_alloc(numChars * sizeof(wchar_t)));

        DragQueryFileW(hDrop, i, buffer, numChars);

        (*files)[i] = UTF8::fromWide(WString(buffer));

        ge_free(buffer);
      }

      GlobalUnlock(stgmed.hGlobal);
      ReleaseStgMedium(&stgmed);
    }

    return files;
  }
}