#pragma once

#include <gePrerequisitesUtil.h>
#include <geVector2I.h>

using namespace geEngineSDK;

class RTSTiledMap;
class RTSMapGridWalker;

class RTSWorld
{
 private:
  RTSTiledMap* m_pTiledMap;
  Vector<RTSMapGridWalker*> m_walkersList;
  RTSMapGridWalker* m_activeWalker;
  int8 m_activeWalkerIndex;

 public:
  RTSWorld();
  ~RTSWorld();

 public:
  bool
  init();

  void
  destroy();

  void
  update(float deltaTime);

  void
  render();

  RTSTiledMap*
  getTiledMap() {
    return m_pTiledMap;
  }

  void
  updateResolutionData();

  void
  setCurrentWalker(const int8 index);
};
