#include "RTSWorld.h"
#include "RTSTiledMap.h"

#include "RTSUnitType.h"

#include "RTSPathfinding.h"

#include "RTSBreathFirstSearchPathfinding.h"
#include "RTSDepthFisrtSearchPathfinding.h"
#include "RTSBestFirstSearchPathfinding.h"

RTSWorld::RTSWorld() {
  m_pTiledMap = nullptr;
  m_pathLine = nullptr;
  m_pRectangleWalker = nullptr;
  m_pRectangleTarget = nullptr;
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

  //Get map to screen cords for starting position shape
  m_pTiledMap->getMapToScreenCoords(
    m_activeWalker->getStartPos().x, m_activeWalker->getStartPos().y,
    screenX, screenY);

  //Set starting position shape
  m_pRectangleWalker->setPosition(
    static_cast<float>(screenX + GameOptions::TILEHALFSIZE.x),
    static_cast<float>(screenY + GameOptions::TILEHALFSIZE.y));
  
  //Get map to screen cords for target position shape
  m_pTiledMap->getMapToScreenCoords(
    m_activeWalker->getTargetPos().x, m_activeWalker->getTargetPos().y, screenX, screenY);

  //Set target position shape
  m_pRectangleTarget->setPosition(
    static_cast<float>(screenX + GameOptions::TILEHALFSIZE.x),
    static_cast<float>(screenY + GameOptions::TILEHALFSIZE.y));
  
  if (m_activeWalker->getCurrentState() == RTSPathfinding::onSearch)
  {
    m_activeWalker->setCurrentState(m_activeWalker->updateSearch());
  }

  else if (m_activeWalker->getCurrentState() == RTSPathfinding::goalReached)
  { 
    if (m_pathLine == nullptr)
    {
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

    }

    m_pTarget->draw(*m_pathLine);
  }

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
    m_pathLine->clear();
    m_pathLine = nullptr;
  }
}
