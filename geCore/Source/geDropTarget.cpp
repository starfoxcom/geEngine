/*****************************************************************************/
/**
 * @file    geDropTarget.cpp
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

/*****************************************************************************/
/**
 * Includes
 */
/*****************************************************************************/
#include "geDropTarget.h"

namespace geEngineSDK {
  void
  DropTarget::_clear() {
    m_fileList.clear();
  }

  bool
  DropTarget::_isInside(const Vector2I& pos) const {
    return m_area.isInside(pos);
  }

  void
  DropTarget::_setFileList(const Vector<Path>& fileList) {
    _clear();
    m_dropType = DROP_TARGET_TYPE::kFileList;
    m_fileList = fileList;
  }

  SPtr<DropTarget>
  DropTarget::create(const RenderWindow* window, const Box2DI& area) {
    auto target = GE_PVT_NEW(DropTarget, window, area);
    return ge_shared_ptr(target);
  }
}
