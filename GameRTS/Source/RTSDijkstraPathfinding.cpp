#include "RTSDijkstraPathfinding.h"
#include "RTSNode.h"
#include "RTSTiledMap.h"

RTSDijkstraPathfinding::RTSDijkstraPathfinding(RTSTiledMap* _pTiledMap)
{
  m_pTiledMap = _pTiledMap;
  m_pCurrent = nullptr;
}

RTSDijkstraPathfinding::~RTSDijkstraPathfinding()
{
}

void RTSDijkstraPathfinding::init()
{
}

bool RTSDijkstraPathfinding::startSearch()
{
  //Reset the queues for new search
  resetSearch();

  //On starting position is not an obstacle
  if (m_pTiledMap->getType(m_startPos.x, m_startPos.y) != TERRAIN_TYPE::kObstacle)
  {

    m_nextNodes.emplace_back(
      m_startPos,
      m_pTiledMap->getCost(m_startPos.x, m_startPos.y),
      0,
      m_pTiledMap->getType(m_startPos.x, m_startPos.y));
  }

  //Otherwise keep state on idle
  else
  {
    m_currentState = SEARCH_STATE::idle;
    return true;
  }

  //Set state to on search
  m_currentState = SEARCH_STATE::onSearch;

  return true;
}

RTSPathfinding::SEARCH_STATE RTSDijkstraPathfinding::updateSearch()
{

  //On next Nodes list empty
  if (m_nextNodes.empty())
  {

    //Goal not reached
    return SEARCH_STATE::goalNotReached;
  }

  //Node counter
  int32 i = 0;

  //Best node index
  int32 bestI = 0;

  //Set the best cost variable as high as possible;
  int32 bestCost = 999999;

  for (auto it = m_nextNodes.begin(); it < m_nextNodes.end(); ++it, ++i)
  {

    //On the possible best magnitude is better than the actual best magnitude
    if (it->m_totalCost < bestCost)
    {

      //Set the best node index with the node counter
      bestI = i;

      bestCost = it->m_totalCost;
    }

  }

  //Add the node to the closed queue
  m_visited.push_back(m_nextNodes[bestI]);

  //On current node pointer is not null
  if (nullptr != m_pCurrent)
  {

    //Delete the current node pointer
    ge_delete(m_pCurrent);

    //Set the node pointer to null
    m_pCurrent = nullptr;
  }

  //Set current node
  m_pCurrent = ge_new<RTSNode>(m_visited.back());

  //On current node position is our target position
  if (m_pCurrent->m_position == m_targetPos)
  {

    //Goal reached
    return SEARCH_STATE::goalReached;
  }

  //Delete node from open list
  m_nextNodes.erase(m_nextNodes.begin() + bestI);

  //Possible connection node position
  Vector2I possibleConnection;

  //For all next positions
  for (int32 k = 0; k < m_nextPositions.size(); ++k)
  {

    //Set the possible connection position
    possibleConnection = m_pCurrent->m_position + m_nextPositions[k];

    //On position of the possible connection not on the open and closed container
    if (addConnection(possibleConnection))
    {

      //Emplace the node to the container
      m_nextNodes.emplace_back(
        possibleConnection,
        m_pTiledMap->getCost(possibleConnection.x, possibleConnection.y),
        m_pCurrent->m_totalCost + m_pTiledMap->getCost(possibleConnection.x, possibleConnection.y),
        m_pTiledMap->getType(possibleConnection.x, possibleConnection.y));

      //Set parent for the one added to the open queue
      m_nextNodes.back().m_parent = ge_new<RTSNode>(*m_pCurrent);
    }
  }

  //Goal reached
  return SEARCH_STATE::onSearch;
}

bool RTSDijkstraPathfinding::addConnection(Vector2I _possibleConnection)
{

  //
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
