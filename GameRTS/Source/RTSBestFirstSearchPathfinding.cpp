#include "RTSBestFirstSearchPathfinding.h"
#include "RTSNode.h"
#include "RTSTiledMap.h"

RTSBestFirstSearchPathfinding::RTSBestFirstSearchPathfinding(RTSTiledMap* _pTiledMap)
{
  m_pTiledMap = _pTiledMap;
  m_pCurrent = nullptr;
}

RTSBestFirstSearchPathfinding::~RTSBestFirstSearchPathfinding()
{
}

void RTSBestFirstSearchPathfinding::init()
{
}

bool RTSBestFirstSearchPathfinding::startSearch()
{
  //Reset the queues for new search
  resetSearch();
  if (m_pTiledMap->getType(m_startPos.x, m_startPos.y) != TERRAIN_TYPE::kObstacle)
  m_nextNodes.emplace_back(
    m_startPos, 
    m_startPos - m_targetPos, 
    m_pTiledMap->getType(m_startPos.x, m_startPos.y));

  m_currentState = SEARCH_STATE::onSearch;

  return true;
}

RTSPathfinding::SEARCH_STATE RTSBestFirstSearchPathfinding::updateSearch()
{

  //On next Nodes list empty
  if (m_nextNodes.empty())
  {
    return SEARCH_STATE::goalNotReached;
  }

  int32 i = 0;
  int32 bestI = 0;

  float bestMag = 9999;
  float possibleBestMag = 0;

  for (auto it = m_nextNodes.begin(); it < m_nextNodes.end(); it++, ++i)
  {
    possibleBestMag = it->m_distance.x + it->m_distance.y;

    if (possibleBestMag < bestMag)
    {
      bestMag = possibleBestMag;
      bestI = i;
    }

  }
  //Add the node to the closed queue
  m_visited.push_back(m_nextNodes[bestI]);

  if (nullptr != m_pCurrent)
  {
    ge_delete(m_pCurrent);
    m_pCurrent = nullptr;
  }

  //Set current node
  m_pCurrent = ge_new<RTSNode>(m_visited.back());

  //On current node position is our target position
  if (m_pCurrent->m_position == m_targetPos)
  {
    return SEARCH_STATE::goalReached;
  }

  //delete node from open list
  m_nextNodes.erase(m_nextNodes.begin() + bestI);

  Vector2I possibleConnection;

  //for all next positions
  for (int32 k = 0; k < m_nextPositions.size(); ++k)
  {
    possibleConnection = m_pCurrent->m_position + m_nextPositions[k];

    //On 
    if (addConnection(possibleConnection))
    {
      m_nextNodes.emplace_back(
        possibleConnection,
        possibleConnection - m_targetPos,
        m_pTiledMap->getType(possibleConnection.x, possibleConnection.y));

      //Set parent for the one added to the open queue
      m_nextNodes.back().m_parent = ge_new<RTSNode>(*m_pCurrent);
    }
  }
  return SEARCH_STATE::onSearch;
}

bool RTSBestFirstSearchPathfinding::addConnection(Vector2I _possibleConnection)
{
  if (_possibleConnection.x > -1 &&
    _possibleConnection.y > -1 &&
    _possibleConnection.x < m_pTiledMap->getMapSize().x &&
    _possibleConnection.y < m_pTiledMap->getMapSize().y)
  {
    if (m_pTiledMap->getType(_possibleConnection.x, _possibleConnection.y) != TERRAIN_TYPE::kObstacle)
    {
      if (!checkList(m_nextNodes, _possibleConnection))
        if (!checkList(m_visited, _possibleConnection))
          return true;
    }
  }
  return false;
}
