#include "RTSDepthFisrtSearchPathfinding.h"
#include "RTSNode.h"
#include "RTSTiledMap.h"

RTSDepthFirstSearchPathfinding::RTSDepthFirstSearchPathfinding(RTSTiledMap* _pTiledMap)
{
  m_pTiledMap = _pTiledMap;
}

RTSDepthFirstSearchPathfinding::~RTSDepthFirstSearchPathfinding()
{
  ge_delete(m_pCurrent);
}

void RTSDepthFirstSearchPathfinding::init()
{
}

bool RTSDepthFirstSearchPathfinding::startSearch()
{
  //Reset the queues for new search
  resetSearch();

  m_nextNodes.emplace_back(m_startPos, m_pTiledMap->getType(m_startPos.x, m_startPos.y));

  m_currentState = SEARCH_STATE::onSearch;

  return true;
}

RTSPathfinding::SEARCH_STATE RTSDepthFirstSearchPathfinding::updateSearch()
{

  //On next Nodes list empty
  if (m_nextNodes.empty())
  {
    return SEARCH_STATE::goalNotReached;
  }


  //Add the node to the closed queue
  m_visited.push_back(m_nextNodes.back());

  //Set current node
  m_pCurrent = ge_new<RTSNode>(m_visited.back());

  //On current node position is our target position
  if (m_pCurrent->m_position == m_targetPos)
  {
    return SEARCH_STATE::goalReached;
  }

  //delete node from open list
  m_nextNodes.pop_back();

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
        m_pTiledMap->getType(possibleConnection.x, possibleConnection.y));

      //Set parent for the one added to the open queue
      m_nextNodes.back().m_parent = m_pCurrent;
    }
  }
  return SEARCH_STATE::onSearch;
}

bool RTSDepthFirstSearchPathfinding::resetSearch()
{
  m_visited = m_nextNodes = {};

  m_currentState = SEARCH_STATE::idle;

  return true;
}

bool RTSDepthFirstSearchPathfinding::addConnection(Vector2I _possibleConnection)
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
