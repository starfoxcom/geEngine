#include "RTSWorld.h"
#include "RTSTiledMap.h"

#include "RTSUnitType.h"

#include "RTSPathfinding.h"

#include "RTSNode.h"

#include "RTSBreathFirstSearchPathfinding.h"
#include "RTSDepthFisrtSearchPathfinding.h"
#include "RTSBestFirstSearchPathfinding.h"
#include "RTSDijkstraPathfinding.h"
#include "RTSAStarPathfinding.h"

RTSWorld::RTSWorld() {
  m_pTiledMap = nullptr;
  m_pathLine = nullptr;
  m_pRectangleWalker = nullptr;
  m_pRectangleTarget = nullptr;
  m_pRectangleVisited = nullptr;
  m_pRectangleNextNode = nullptr;
  m_activeWalkerIndex = -1;	//-1 = Invalid index
}

RTSWorld::~RTSWorld() {
  destroy();
}

bool
RTSWorld::init(sf::RenderTarget* pTarget) {
  GE_ASSERT(nullptr == m_pTiledMap && "World was already initialized");
  destroy();

  m_pTarget = pTarget;

  //Initialize the map (right now it's an empty map)
  m_pTiledMap = ge_new<RTSTiledMap>();
  GE_ASSERT(m_pTiledMap);
  m_pTiledMap->init(m_pTarget, Vector2I(4096, 4096));

  //Create the path finding classes and push them to the walker list
  //m_walkersList.push_back(ge_new<RTSBreadthFirstSearchMapGridWalker>(m_pTiledMap));

  m_walkersList.push_back(ge_new<RTSDepthFirstSearchPathfinding>(m_pTiledMap));
  m_walkersList.push_back(ge_new<RTSBreathFirstSearchPathfinding>(m_pTiledMap));
  m_walkersList.push_back(ge_new<RTSBestFirstSearchPathfinding>(m_pTiledMap));
  m_walkersList.push_back(ge_new<RTSDijkstraPathfinding>(m_pTiledMap));
  m_walkersList.push_back(ge_new<RTSAStarPathfinding>(m_pTiledMap));

  //Init the walker objects

  for (SIZE_T it = 0; it < m_walkersList.size(); ++it) {
    m_walkersList[it]->init();
  }

  //Set the first walker as the active walker
  setCurrentWalker(m_walkersList.size() > 0 ? 0 : -1);


  RTSGame::RTSUnitType unitTypes;
  unitTypes.loadAnimationData(m_pTarget, 1);

  m_pRectangleWalker = ge_new<sf::RectangleShape>(sf::Vector2f(10.f, 10.f));
  m_pRectangleWalker->setFillColor(sf::Color::Green);
  m_pRectangleWalker->setOrigin(5, 5);

  m_pRectangleTarget = ge_new<sf::RectangleShape>(sf::Vector2f(10.f, 10.f));
  m_pRectangleTarget->setFillColor(sf::Color::Red);
  m_pRectangleTarget->setOrigin(5, 5);

  m_pRectangleVisited = ge_new<sf::RectangleShape>(sf::Vector2f(10.f, 10.f));
  m_pRectangleVisited->setFillColor(sf::Color::Magenta);
  m_pRectangleVisited->setOrigin(5, 5);

  m_pRectangleNextNode = ge_new<sf::RectangleShape>(sf::Vector2f(10.f, 10.f));
  m_pRectangleNextNode->setFillColor(sf::Color::Blue);
  m_pRectangleNextNode->setOrigin(5, 5);

  return true;
}

void
RTSWorld::destroy() {
 //Destroy all the walkers
  while (m_walkersList.size() > 0) {
    ge_delete(m_walkersList.back());
    m_walkersList.pop_back();
  }

  //As the last step, destroy the full map
  if (nullptr != m_pTiledMap) {
    ge_delete(m_pTiledMap);
    m_pTiledMap = nullptr;
  }

  if (nullptr != m_pRectangleWalker)
  {
    ge_delete(m_pRectangleWalker);
    m_pRectangleWalker = nullptr;
  }

  if (nullptr != m_pRectangleTarget)
  {
    ge_delete(m_pRectangleTarget);
    m_pRectangleTarget = nullptr;
  }

  if (nullptr != m_pRectangleVisited)
  {
    ge_delete(m_pRectangleVisited);
    m_pRectangleVisited = nullptr;
  }

  if (nullptr != m_pRectangleNextNode)
  {
    ge_delete(m_pRectangleNextNode);
    m_pRectangleNextNode = nullptr;
  }

  if (nullptr != m_pathLine)
  {
    ge_delete(m_pathLine);
    m_pathLine = nullptr;
  }
}

void
RTSWorld::update(float deltaTime) {
  m_pTiledMap->update(deltaTime);
}

void
RTSWorld::render() {
  m_pTiledMap->render();

  //Screen cords
  int32 screenX, screenY;

  drawStarterNodes(m_pRectangleWalker, m_activeWalker->getStartPos());
  
  drawStarterNodes(m_pRectangleTarget, m_activeWalker->getTargetPos());
  
  if (m_activeWalker->getCurrentState() == RTSPathfinding::onSearch)
  {
    m_activeWalker->setCurrentState(m_activeWalker->updateSearch());
  }
  else if (m_activeWalker->getCurrentState() == RTSPathfinding::goalReached)
  {
    
    clearPathLine();

    Vector<Vector2I> backTrace = m_activeWalker->backTrace(m_activeWalker->getCurrentNode());

    m_pathLine = ge_new<sf::VertexArray>(sf::LineStrip, backTrace.size());

    int32 i = 0;

    for (auto it = backTrace.begin(); it != backTrace.end(); ++it, ++i)
    {
      getTiledMap()->getMapToScreenCoords(it->x, it->y, screenX, screenY);

      (*m_pathLine)[i].position = sf::Vector2f(
        static_cast<float>(screenX + GameOptions::TILEHALFSIZE.x),
        static_cast<float>(screenY + GameOptions::TILEHALFSIZE.y));

      (*m_pathLine)[i].color = sf::Color::White;
    }

    m_pTarget->draw(*m_pathLine);
  }

  drawAlgorithmNodes(m_pRectangleNextNode, m_activeWalker->getNextNodes());

  drawAlgorithmNodes(m_pRectangleVisited, m_activeWalker->getVisitedNodes());

  m_pTarget->draw(*m_pRectangleWalker);
  m_pTarget->draw(*m_pRectangleTarget);

}

void
RTSWorld::updateResolutionData() {
  if (nullptr != m_pTiledMap) {
    Vector2I appResolution = g_gameOptions().s_Resolution;
    
    m_pTiledMap->setStart(0, 0);
    m_pTiledMap->setEnd(appResolution.x, appResolution.y - 175);
    
    //This ensures a clamp if necessary
    m_pTiledMap->moveCamera(0, 0);
  }
}

void
RTSWorld::setCurrentWalker(const int8 index) {
  //Revisamos que el walker exista (en modo de debug)
  GE_ASSERT(m_walkersList.size() > static_cast<SIZE_T>(index));

  m_activeWalker = m_walkersList[index];
  m_activeWalkerIndex = index;
}

RTSPathfinding* RTSWorld::getCurrentWalker()
{
  return m_activeWalker;
}

sf::VertexArray * RTSWorld::getPathLine()
{
  return m_pathLine;
}

void RTSWorld::clearPathLine()
{
  if (m_pathLine != nullptr)
  {
    ge_delete(m_pathLine);
    m_pathLine = nullptr;
  }
}

void RTSWorld::drawStarterNodes(sf::RectangleShape *_pRectangle, Vector2I _position)
{

  int32 screenX, screenY;

  //Get map to screen cords for starting position shape
  m_pTiledMap->getMapToScreenCoords(
    _position.x, _position.y,
    screenX, screenY);

  //Set starting position shape
  _pRectangle->setPosition(
    static_cast<float>(screenX + GameOptions::TILEHALFSIZE.x),
    static_cast<float>(screenY + GameOptions::TILEHALFSIZE.y));
}

void RTSWorld::drawAlgorithmNodes(sf::RectangleShape * _pRectangle, Vector<RTSNode> _nodes)
{
  Vector<sf::RectangleShape*> drawNodes;

  int32 screenX, screenY;

  for (int32 i = 0; i < _nodes.size(); ++i)
  {
    m_pTiledMap->getMapToScreenCoords(
      _nodes[i].m_position.x, _nodes[i].m_position.y,
      screenX, screenY);

    //Set starting position shape
    _pRectangle->setPosition(
      static_cast<float>(screenX + GameOptions::TILEHALFSIZE.x),
      static_cast<float>(screenY + GameOptions::TILEHALFSIZE.y));

    drawNodes.push_back(_pRectangle);

    m_pTarget->draw(*drawNodes.back());
  }

  for (int32 i = 0; i < drawNodes.size(); ++i)
  {
  }
}



