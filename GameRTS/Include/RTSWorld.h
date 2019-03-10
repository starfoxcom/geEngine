#pragma once

#include <gePrerequisitesUtil.h>
#include <geVector2I.h>

#include <SFML/Graphics.hpp>

using namespace geEngineSDK;

class RTSTiledMap;
class RTSMapGridWalker;

class RTSPathfinding;
class RTSBreathFirstSearchPathfinding;

class RTSWorld
{
 public:
  RTSWorld();
  ~RTSWorld();

 public:
  bool
  init(sf::RenderTarget* pTarget);

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

  RTSPathfinding* getCurrentWalker();

  sf::VertexArray* getPathLine();

  void clearPathLine();

 private:
  RTSTiledMap* m_pTiledMap;
  //List<RTSUnitType*> m_lstUnitTypes;
  //List<RTSUnit*> m_lstUnits;
  
  //Vector<RTSMapGridWalker*> m_walkersList;
  Vector<RTSPathfinding*> m_walkersList;
  RTSPathfinding* m_activeWalker;
  int8 m_activeWalkerIndex;

  sf::RenderTarget* m_pTarget;

  sf::RectangleShape* m_pRectangleWalker, *m_pRectangleTarget;

  sf::VertexArray* m_pathLine;
};
