#pragma once

#include <gePrerequisitesUtil.h>
#include <geVector2I.h>

#include <SFML/Graphics.hpp>

using namespace geEngineSDK;

class RTSTiledMap;
class RTSMapGridWalker;

class RTSPathfinding;
class RTSBreathFirstSearchPathfinding;

class RTSNode;

namespace RTSGame
{

class RTSUnitType;
}

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

  void drawStarterNodes(sf::RectangleShape *_pRectangle, Vector2I _position);
 
  void drawAlgorithmNodes(
    sf::RectangleShape *_pRectangle, 
    Vector<RTSNode> _nodes);

 private:
  RTSTiledMap* m_pTiledMap;
  Vector<RTSGame::RTSUnitType*> m_lstUnitTypes;
  //List<RTSUnit*> m_lstUnits;
  
  //Vector<RTSMapGridWalker*> m_walkersList;
  Vector<RTSPathfinding*> m_walkersList;
  RTSPathfinding* m_activeWalker;
  int8 m_activeWalkerIndex;

  sf::RenderTarget* m_pTarget;

  sf::RectangleShape* m_pRectangleWalker, *m_pRectangleTarget, *m_pRectangleVisited, *m_pRectangleNextNode;

  sf::VertexArray* m_pathLine;
};
